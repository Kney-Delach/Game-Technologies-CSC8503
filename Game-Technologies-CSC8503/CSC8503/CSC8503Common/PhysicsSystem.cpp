/***************************************************************************
* Filename		: PhysicsSystem.cpp
* Name			: Ori Lazar
* Date			: 28/11/2019
* Description	:
.---.
  .'_:___".
  |__ --==|
  [  ]  :[|
  |__| I=[|
  / / ____|
 |-/.____.'
/___\ /___\
***************************************************************************/
#include "PhysicsSystem.h"
#include "PhysicsObject.h"
#include "GameObject.h"
#include "CollisionDetection.h"
#include "../../Common/Quaternion.h"

#include "Constraint.h"

#include "Debug.h"

#include <functional>
using namespace NCL;
using namespace CSC8503;

PhysicsSystem::PhysicsSystem(GameWorld& g) : gameWorld(g)
{
	applyGravity	= true;
	useBroadPhase	= true; //todo: turn on only when in release mode	
	dTOffset		= 0.0f;
	globalDamping	= 0.95f;
	SetGravity(Vector3(0.0f, -9.8f, 0.0f));
}

PhysicsSystem::~PhysicsSystem()
{
}

void PhysicsSystem::SetGravity(const Vector3& g)
{
	gravity = g;
}

/*

If the 'game' is ever reset, the PhysicsSystem must be
'cleared' to remove any old collisions that might still
be hanging around in the collision list. If your engine
is expanded to allow objects to be removed from the world,
you'll need to iterate through this collisions list to remove
any collisions they are in.

*/
void PhysicsSystem::Clear()
{
	allCollisions.clear();
}

int constraintIterationCount = 5;

void PhysicsSystem::Update(float dt)
{
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::I)) 
	{
		constraintIterationCount--;
		std::cout << "Setting constraint iterations to " << constraintIterationCount << "\n";
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::O)) 
	{
		constraintIterationCount++;
		std::cout << "Setting constraint iterations to " << constraintIterationCount << "\n";
	}

	dTOffset += dt; //We accumulate time delta here - there might be remainders from previous frame!

	static const float idealFrameRate = 60;							//this is the framerate we'd like to maintain kplzthx
	static const float idealIterations = idealFrameRate * 2;			//we want n per second;
	static const int perFrameIts = (int)(idealIterations / idealFrameRate);
	float iterationDt = 1.0f / idealIterations;				//So each iteration we'll get an advance of this time
	const float currentFrameRate = 1.0f / dt;							//what's our current fps?
	const int realIterations = (int)(dTOffset / iterationDt);				//how many iterations of the desired dt can we actually fit in our timestep?
	if (currentFrameRate < idealFrameRate * 0.5f)
	{
		iterationDt = dTOffset; //run one big update if the framerate tanks
	}
	else if (realIterations > perFrameIts + 1) //+1 as we sometimes accumulate an extra frame to take up the leftover time
	{
		//UH OH, we're accumulating too much time from somewhere, half the iteration count
		//Probably caused by not being able to quite maintain the fps
		iterationDt *= 2;
	}
	if (useBroadPhase)
	{
		UpdateObjectAABBs();
	}
	int iteratorCount = 0;
	while (dTOffset >= iterationDt)
	{
		IntegrateAccel(iterationDt); //Update accelerations from external forces
		if (useBroadPhase)
		{
			BroadPhase();
			NarrowPhase();
		}
		else
		{
			BasicCollisionDetection();
		}
		//This is our simple iterative solver - 
		//we just run things multiple times, slowly moving things forward
		//and then rechecking that the constraints have been met		
		const float constraintDt = iterationDt / (float)constraintIterationCount;
		for (int i = 0; i < constraintIterationCount; ++i)
		{
			UpdateConstraints(constraintDt);
		}
		IntegrateVelocity(iterationDt); //update positions from new velocity changes

		dTOffset -= iterationDt;
		iteratorCount++;
	}
	ClearForces();	//Once we've finished with the forces, reset them to zero
	UpdateCollisionList(); //Remove any old collisions
}



// 30.11.2019
// ------------------------------------------------------------------------------
// NOTE: This function iterates over every gameobject TWICE!
// Step thorugh every pair of objects once (the inner for loop offset ensures this),
// and determine whether they collide,
// and if so, add them to the collision set for later processing.
// The set will guarantee that a particular pair will only be added once,
// so objects colliding for multiple frames won't flood the set with duplicates.
void PhysicsSystem::BasicCollisionDetection()
{
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;
	gameWorld.GetObjectIterators(first, last);

	for (auto i = first; i != last; ++i)
	{
		if ((*i)->GetPhysicsObject() == nullptr)
			continue;

		for (auto j = i + 1; j != last; ++j)
		{
			if ((*j)->GetPhysicsObject() == nullptr)
				continue;

			CollisionDetection::CollisionInfo info;
			if (CollisionDetection::ObjectIntersection(*i, *j, info)) // returns true if collision has taken place 
			{
				switch (info.a->GetPhysicsObject()->GetCollisionType() & info.b->GetPhysicsObject()->GetCollisionType())
				{
					case ObjectCollisionType::IMPULSE:
					{
						ImpulseResolveCollision(*info.a, *info.b, info.point);
						break;
					}
					case ObjectCollisionType::SPRING:
					{
						ResolveSpringCollision(*info.a, *info.b, info.point);
						break;
					}
					case ObjectCollisionType::COLLECTABLE:
					{
						break;
					}
					case ObjectCollisionType::JUMP_PAD:
					{
						ResolveJumpPadCollision(*info.a, *info.b, info.point);
						break;
					}
					case ObjectCollisionType::IMPULSE | ObjectCollisionType::SPRING:
					{
						if (info.a->GetName() == "AI" && info.b->GetName() == "AI")
						{
							ResolveSpringCollision(*info.a, *info.b, info.point);
							break;
						}
						ImpulseResolveCollision(*info.a, *info.b, info.point);
						break;
					}
					case ObjectCollisionType::IMPULSE | ObjectCollisionType::SPRING | ObjectCollisionType::JUMP_PAD:
					{
						ImpulseResolveCollision(*info.a, *info.b, info.point);
						break;
					}
				}				
				info.framesLeft = numCollisionFrames;
				allCollisions.insert(info);
			}
		}
	}
}

// 30.11.2019
// ------------------------------------------------------------------------------
// used to keep track of collisions across multiple frames
// first time added -> colliding
// frame removed -> no longer colliding
// used to build up gameplay interactions inside (OnCollisionBegin | OnCollisionEnd)
void PhysicsSystem::UpdateCollisionList()
{
	for (std::set<CollisionDetection::CollisionInfo>::iterator i = allCollisions.begin(); i != allCollisions.end(); )
	{
		if ((*i).framesLeft == numCollisionFrames)
		{
			i->a->OnCollisionBegin(i->b);
			i->b->OnCollisionBegin(i->a);
		}
		(*i).framesLeft = (*i).framesLeft - 1;
		if ((*i).framesLeft < 0)
		{
			i->a->OnCollisionEnd(i->b);
			i->b->OnCollisionEnd(i->a);
			i = allCollisions.erase(i);
		}
		else
		{
			++i;
		}
	}
}

// 8.12.2019
void PhysicsSystem::UpdateObjectAABBs()
{
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;
	gameWorld.GetObjectIterators(first, last);

	for (auto i = first; i != last; ++i)
	{	
		(*i)->UpdateBroadphaseAABB();
	}
}

// 30.11.2019
// ------------------------------------------------------------------------------
// Computes the correct response to a collision.
void PhysicsSystem::ImpulseResolveCollision(GameObject& a, GameObject& b, CollisionDetection::ContactPoint& p) const
{
	// physics objects of objects
	PhysicsObject* physicsObjectA = a.GetPhysicsObject();
	PhysicsObject* physicsObjectB = b.GetPhysicsObject();

	// transforms of objects
	Transform& transformA = a.GetTransform();
	Transform& transformB = b.GetTransform();

	// total inverse mass of the two objects (used to calculate impulse and projection)
	const float totalMass = physicsObjectA->GetInverseMass() + physicsObjectB->GetInverseMass();
	
	if (totalMass == 0.f) return; // don't process multiple static objects
	
	//////////////////////////////////////////////////
	//// separate objects using projection method ////
	//////////////////////////////////////////////////
	// translate each object along collision normal (proportional to penetration distance and object's inverse mass)
	// dividing by total mass -> total object movement (a & b) results in penetration movement away (with heavier object moving less)
	// heavier objects have lower (inverse) mass values (as computing using inverses) 
	transformA.SetWorldPosition(transformA.GetWorldPosition() - (p.normal * p.penetration * (physicsObjectA->GetInverseMass() / totalMass)));
	transformB.SetWorldPosition(transformB.GetWorldPosition() +	(p.normal * p.penetration * (physicsObjectB->GetInverseMass() / totalMass)));
	
	// 1 - collision points relative to each object's position. 
	const Vector3 relativePointA = p.localA;
	const Vector3 relativePointB = p.localB;

	// 2 - compute angular velocities (the further away from the centre of the object a point is, the faster it moves as the object rotates).
	const Vector3 angVelocityA = Vector3::Cross(physicsObjectA->GetAngularVelocity(), relativePointA);
	const Vector3 angVelocityB = Vector3::Cross(physicsObjectB->GetAngularVelocity(), relativePointB);

	// 3 - combine each object's linear and angular velocities to determine their total velocity.
	const Vector3 fullVelocityA = physicsObjectA->GetLinearVelocity() + angVelocityA;
	const Vector3 fullVelocityB = physicsObjectB->GetLinearVelocity() + angVelocityB;

	// 4 - compute velocities at which the two objects collide.
	const Vector3 contactVelocity = fullVelocityB - fullVelocityA;

	/////////////////////////////////////////////////
	//// compute impulse vector J ///////////////////
	/////////////////////////////////////////////////
	// compute impulse force
	const float impulseForce = Vector3::Dot(contactVelocity, p.normal);
	
	if (impulseForce > 0)
		return;	

	// compute inertia
	const Vector3 inertiaA = Vector3::Cross(physicsObjectA->GetInertiaTensor() * (Vector3::Cross(relativePointA, p.normal)), relativePointA);
	const Vector3 inertiaB = Vector3::Cross(physicsObjectB->GetInertiaTensor() * (Vector3::Cross(relativePointB, p.normal)), relativePointB);

	const float angularEffect = Vector3::Dot(inertiaA + inertiaB, p.normal);

	//todo: Change this coefficient of restitution to non hard-coded value
	const float restitutionCoefficient = physicsObjectA->GetElasticity() * physicsObjectB->GetElasticity(); // disperses kinetic energy

	const float impulseJ = (-(1.0f + restitutionCoefficient) * impulseForce) / (totalMass + angularEffect);

	// full impulse 
	Vector3 fullImpulse = p.normal * impulseJ;

	if (a.GetName() == "AI" && b.GetName() == "AI" || a.GetName() == "AI" && b.GetName() == "Goose" || a.GetName() == "Goose" && b.GetName() == "AI")
		fullImpulse.y = 0.f;
	////////////////////////////////////////////////////////////////////////////////////
	//// Apply linear and angular impulses to both objects (in opposite directions) ////
	////////////////////////////////////////////////////////////////////////////////////
	physicsObjectA->ApplyLinearImpulse(-fullImpulse);
	physicsObjectB->ApplyLinearImpulse(fullImpulse);
	physicsObjectA->ApplyAngularImpulse(Vector3::Cross(relativePointA,-fullImpulse));
	physicsObjectB->ApplyAngularImpulse(Vector3::Cross(relativePointB,fullImpulse));
}

// 2.12.2019
// ------------------------------------------------------------------------------
// Computes the correct response to a collision using Hooke's Law by the penalty method.
void PhysicsSystem::ResolveSpringCollision(GameObject& a, GameObject& b, CollisionDetection::ContactPoint& p) const
{
	// physics objects of objects
	PhysicsObject* physicsObjectA = a.GetPhysicsObject();
	PhysicsObject* physicsObjectB = b.GetPhysicsObject();

	// 1. model spring with resting length of 0, attached to collision points of the collision
	const Vector3 springPositionA = p.localA;
	const Vector3 springPositionB = p.localB;
	
	// 2. state temporary spring being extended along the collision normal n by penetration distance p.
	const Vector3 springExtensionDirection = p.normal;
	const float springExtensionLength = p.penetration;

	Vector3 springExtension = springExtensionDirection * springExtensionLength;
	if (a.GetName() == "AI" && b.GetName() == "AI" || a.GetName() == "AI" && b.GetName() == "Goose" || a.GetName() == "Goose" && b.GetName() == "AI")
		springExtension.y = 0.f;
	// 3. apply force proportional to penetration distance, at collision point on each object, in direction of collision normal.
	//    -> outputs acceleration and torque (like when applied force at specific point during raycasting)
	const Vector3 forceOnObjectA = -springExtension * (physicsObjectB->GetStiffness());
	physicsObjectA->AddForceAtRelativePosition(forceOnObjectA, springPositionA);
	const Vector3 forceOnObjectB = springExtension * (physicsObjectA->GetStiffness());
	physicsObjectB->AddForceAtRelativePosition(forceOnObjectB, springPositionB);
}

void PhysicsSystem::ResolveJumpPadCollision(GameObject& a, GameObject& b, CollisionDetection::ContactPoint& p) const
{
	const float totalMass = a.GetPhysicsObject()->GetInverseMass() + b.GetPhysicsObject()->GetInverseMass();
	if(totalMass == 0.f) return;

	PhysicsObject* dynamicPhysicsObject = a.GetPhysicsObject()->GetInverseMass() == 0 ? b.GetPhysicsObject() : a.GetPhysicsObject();
	dynamicPhysicsObject->AddForceAtRelativePosition(Vector3(0, 200.f, 0) * dynamicPhysicsObject->GetStiffness(), p.localA);
}

void PhysicsSystem::BroadPhase()
{
	broadphaseCollisions.clear();
	QuadTree<GameObject*>tree(Vector2(1024, 1024), 7, 6);
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;
	
	gameWorld.GetObjectIterators(first, last);
	for (auto i = first; i != last; ++i) 
	{
		Vector3 halfSizes;
		if (!(*i)->GetBroadphaseAABB(halfSizes))
		{
			continue;
		}
		Vector3 pos = (*i)->GetConstTransform().GetWorldPosition();
		tree.Insert(*i, pos, halfSizes);

		tree.OperateOnContents([&](std::list<QuadTreeEntry<GameObject*>>& data)
		{
			CollisionDetection::CollisionInfo info;
			for (auto i = data.begin(); i != data.end(); ++i)
			{
				for (auto j = std::next(i); j != data.end(); ++j)
				{
					// is this pair of items already in the collision set
					// if the same pair is in another quadtree node together etc
					info.a = min((*i).object, (*j).object);
					info.b = max((*i).object, (*j).object);
					if (info.a->GetPhysicsObject()->IsStatic() & info.b->GetPhysicsObject()->IsStatic()) 
					{
						continue;
					}
					broadphaseCollisions.insert(info);
				}			}
		});
	}
}

/*

The broadphase will now only give us likely collisions, so we can now go through them,
and work out if they are truly colliding, and if so, add them into the main collision list
*/
void PhysicsSystem::NarrowPhase()
{
	for (std::set<CollisionDetection::CollisionInfo>::iterator i = broadphaseCollisions.begin(); i != broadphaseCollisions.end(); ++i) 
	{
		CollisionDetection::CollisionInfo info = *i;
		if (CollisionDetection::ObjectIntersection(info.a, info.b, info)) 
		{
			info.framesLeft = numCollisionFrames;
			switch (info.a->GetPhysicsObject()->GetCollisionType() & info.b->GetPhysicsObject()->GetCollisionType())
			{
				case ObjectCollisionType::IMPULSE:
				{
					ImpulseResolveCollision(*info.a, *info.b, info.point);
					break;
				}
				case ObjectCollisionType::SPRING:
				{
					ResolveSpringCollision(*info.a, *info.b, info.point);
					break;
				}
				case ObjectCollisionType::COLLECTABLE:
				{
					break;
				}
				case ObjectCollisionType::JUMP_PAD:
				{
					ResolveJumpPadCollision(*info.a, *info.b, info.point);
					break;
				}
				case ObjectCollisionType::IMPULSE | ObjectCollisionType::SPRING:
				{
					if (info.a->GetName() == "AI" && info.b->GetName() == "AI")
					{
						ResolveSpringCollision(*info.a, *info.b, info.point);
						break;
					}	
					ImpulseResolveCollision(*info.a, *info.b, info.point);
					break;
				}
				case ObjectCollisionType::IMPULSE | ObjectCollisionType::SPRING | ObjectCollisionType::JUMP_PAD:
				{
					ImpulseResolveCollision(*info.a, *info.b, info.point);
					break;
				}
			}
			allCollisions.insert(info); // insert into our main set
		}
	
	}
}

/*
Integration of acceleration and velocity is split up, so that we can
move objects multiple times during the course of a PhysicsUpdate,
without worrying about repeated forces accumulating etc. 

This function will update both linear and angular acceleration,
based on any forces that have been accumulated in the objects during
the course of the previous game frame.
*/
// 28.11.2019
void PhysicsSystem::IntegrateAccel(float dt)
{
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;
	gameWorld.GetObjectIterators(first, last);
	
	for (auto i = first; i != last; ++i) // for each gameobject 
	{
		PhysicsObject * object = (*i)->GetPhysicsObject();
		
		if (object == nullptr) // don't process objects that don't experience physics
			continue;
				
		float inverseMass = object->GetInverseMass();
		Vector3 linearVelocity = object->GetLinearVelocity();
		Vector3 force = object->GetForce();
		Vector3 accel = force * inverseMass;

		if (applyGravity && inverseMass > 0 && object->UsesGravity()) // don ’t move infinitely heavy things
			accel += gravity * 1.f;

		linearVelocity += accel * dt; // integrate acceleration
		object->SetLinearVelocity(linearVelocity);

		// 28.11.2019 - Angular Motion
		// Angular calculation torque calculation
		Vector3 torque = object->GetTorque();
		Vector3 angularVelocity = object->GetAngularVelocity();
		
		object->UpdateInertiaTensor(); // update tensor vs orientation
		
		Vector3 angularAcceleration = object->GetInertiaTensor() * torque;
		
		angularVelocity += angularAcceleration * dt; // integration 
		object->SetAngularVelocity(angularVelocity);
	}
}

/*
This function integrates linear and angular velocity into
position and orientation. It may be called multiple times
throughout a physics update, to slowly move the objects through
the world, looking for collisions.
*/
// 28.11.2019
void PhysicsSystem::IntegrateVelocity(float dt)
{
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;
	
	gameWorld.GetObjectIterators(first, last);

	const float tempDamping = 1 - globalDamping; 
	const float frameDamping = powf(tempDamping, dt); // simulates resistence 	
	for (auto i = first; i != last; ++i)  // for each gameobject
	{
		PhysicsObject * object = (*i)->GetPhysicsObject();
		if (object == nullptr) // don't process objects that don't experience physics
			continue;
		
		Transform& transform = (*i)->GetTransform();
		Vector3 position = transform.GetLocalPosition();
		Vector3 linearVel = object->GetLinearVelocity();
		position += linearVel * dt; // Position calculation
		
		transform.SetLocalPosition(position);
		transform.SetWorldPosition(position);

		linearVel = linearVel * frameDamping; // Linear Damping
		object->SetLinearVelocity(linearVel);

		// 28.11.2019 - Angular Motion
		// Angular calculation orientation 
		Quaternion orientation = transform.GetLocalOrientation();
		Vector3 angularVelocity = object->GetAngularVelocity();
		
		orientation = orientation + Quaternion(angularVelocity * dt * 0.5f, 0.0f) * orientation; // integration
		orientation.Normalise();
		
		transform.SetLocalOrientation(orientation);

		angularVelocity = angularVelocity * frameDamping; // Damp the angular velocity (simulate resistance) 
		object->SetAngularVelocity(angularVelocity);
	}
}

/*
Once we're finished with a physics update, we have to
clear out any accumulated forces, ready to receive new
ones in the next 'game' frame.
*/
void PhysicsSystem::ClearForces()
{
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;
	gameWorld.GetObjectIterators(first, last);

	for (auto i = first; i != last; ++i)
		(*i)->GetPhysicsObject()->ClearForces(); // clears object's forces for the next frame
}


/*

As part of the final physics tutorials, we add in the ability
to constrain objects based on some extra calculation, allowing
us to model springs and ropes etc. 
*/
void PhysicsSystem::UpdateConstraints(float dt)
{
	std::vector<Constraint*>::const_iterator first;
	std::vector<Constraint*>::const_iterator last;
	gameWorld.GetConstraintIterators(first, last);

	for (auto i = first; i != last; ++i)
	{
		(*i)->UpdateConstraint(dt);
	}
}