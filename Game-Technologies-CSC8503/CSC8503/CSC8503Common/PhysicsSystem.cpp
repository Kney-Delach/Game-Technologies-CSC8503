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
	applyGravity	= false;
	useBroadPhase	= false;	
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

/*

This is the core of the physics engine update

*/
void PhysicsSystem::Update(float dt)
{
	GameTimer testTimer;
	testTimer.GetTimeDeltaSeconds();

	frameDT = dt;

	dTOffset += dt; //We accumulate time delta here - there might be remainders from previous frame!

	float iterationDt = 1.0f / 120.0f; //Ideally we'll have 120 physics updates a second 

	if (dTOffset > 8 * iterationDt)
	{ //the physics engine cant catch up!
		iterationDt = 1.0f / 15.0f; //it'll just have to run bigger timesteps...
		//std::cout << "Setting physics iterations to 15" << iterationDt << std::endl;
	}
	else if (dTOffset > 4  * iterationDt)
	{ //the physics engine cant catch up!
		iterationDt = 1.0f / 30.0f; //it'll just have to run bigger timesteps...
		//std::cout << "Setting iteration dt to 4 case " << iterationDt << std::endl;
	}
	else if (dTOffset > 2* iterationDt)
	{ //the physics engine cant catch up!
		iterationDt = 1.0f / 60.0f; //it'll just have to run bigger timesteps...
		//std::cout << "Setting iteration dt to 2 case " << iterationDt << std::endl;
	}
	else
	{
		//std::cout << "Running normal update " << iterationDt << std::endl;
	}

	int constraintIterationCount = 10;
	iterationDt = dt;

	if (useBroadPhase)
	{
		UpdateObjectAABBs();
	}

	while(dTOffset > iterationDt *0.5)
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
		float constraintDt = iterationDt /  (float)constraintIterationCount;

		for (int i = 0; i < constraintIterationCount; ++i)
{
			UpdateConstraints(constraintDt);	
		}
		
		IntegrateVelocity(iterationDt); //update positions from new velocity changes
		dTOffset -= iterationDt; 
	}
	ClearForces();	//Once we've finished with the forces, reset them to zero

	UpdateCollisionList(); //Remove any old collisions
	//std::cout << iteratorCount << " , " << iterationDt << std::endl;
	//float time = testTimer.GetTimeDeltaSeconds();
	//std::cout << "Physics time taken: " << time << std::endl;
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
				//std::cout << " Collision between " << (*i)->GetName() << " and " << (*j)->GetName() << "\n";
				ImpulseResolveCollision(*info.a, *info.b, info.point); // resolves collisions through impulse resolution
				info.framesLeft = numCollisionFrames;
				allCollisions.insert(info);
			}
		}	}
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
	//transformA.SetWorldPosition(transformA.GetWorldPosition() - (p.normal * p.penetration * (physicsObjectA->GetInverseMass() / totalMass)));
	//transformB.SetWorldPosition(transformB.GetWorldPosition() +	(p.normal * p.penetration * (physicsObjectB->GetInverseMass() / totalMass)));

	// todo : implement conservation of momentum via a change in the amount of linear / angular velocity of objects
	
	// 1 - collision points relative to each object's position. 
	const Vector3 relativePointA = p.localA;// - transformA.GetWorldPosition();
	const Vector3 relativePointB = p.localB;// - transformB.GetWorldPosition();

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
	{
		return;
	}

	// compute inertia
	const Vector3 inertiaA = Vector3::Cross(physicsObjectA->GetInertiaTensor() * (Vector3::Cross(relativePointA, p.normal)), relativePointA);
	const Vector3 inertiaB = Vector3::Cross(physicsObjectB->GetInertiaTensor() * (Vector3::Cross(relativePointB, p.normal)), relativePointB);

	const float angularEffect = Vector3::Dot(inertiaA + inertiaB, p.normal);

	//todo: Change this coefficient of restitution to non hard-coded value
	const float restitutionCoefficient = 0.66f; // disperses kinetic energy

	const float impulseJ = (- (1.0f + restitutionCoefficient) * impulseForce) / (totalMass + angularEffect);
	// full impulse 
	Vector3 fullImpulse = p.normal * impulseJ;
	//std::cout << "Impulse Force: " << fullImpulse << "\n";
	
	////////////////////////////////////////////////////////////////////////////////////
	//// Apply linear and angular impulses to both objects (in opposite directions) ////
	////////////////////////////////////////////////////////////////////////////////////
	physicsObjectA->ApplyLinearImpulse(-fullImpulse);
	physicsObjectB->ApplyLinearImpulse(fullImpulse);
	physicsObjectA->ApplyAngularImpulse(Vector3::Cross(relativePointA,-fullImpulse));
	physicsObjectB->ApplyAngularImpulse(Vector3::Cross(relativePointB,fullImpulse));
}

/*

Later, we replace the BasicCollisionDetection method with a broadphase
and a narrowphase collision detection method. In the broad phase, we
split the world up using an acceleration structure, so that we can only
compare the collisions that we absolutely need to. 

*/

void PhysicsSystem::BroadPhase()
{

}

/*

The broadphase will now only give us likely collisions, so we can now go through them,
and work out if they are truly colliding, and if so, add them into the main collision list
*/
void PhysicsSystem::NarrowPhase()
{

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
		
		if (applyGravity && inverseMass > 0) // don ’t move infinitely heavy things
			accel += gravity;
		
		linearVelocity += accel * dt; // integrate acceleration
		object->SetLinearVelocity(linearVelocity);

		// 28.11.2019 - Angular Motion
		// Angular calculation torque calculation
		Vector3 torque = object->GetTorque();
		Vector3 angularVelocity = object->GetAngularVelocity();
		
		object->UpdateInertiaTensor(); // update tensor vs orientation
		
		Vector3 angularAcceleration = object->GetInertiaTensor() * torque;
		
		angularVelocity += angularAcceleration * dt; // integration 
		object->SetAngularVelocity(angularVelocity);	}
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

	const float tempDamping = globalDamping; 
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
		//transform.SetLocalPosition(position);
		transform.SetWorldPosition(position); //todo: maybe this?

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
		object->SetAngularVelocity(angularVelocity);	}
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