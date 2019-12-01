/***************************************************************************
* Filename		: PhysicsObject.cpp
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
#include "PhysicsObject.h"
#include "PhysicsSystem.h"
#include "../CSC8503Common/Transform.h"

using namespace NCL;
using namespace CSC8503;

PhysicsObject::PhysicsObject(Transform* parentTransform, const CollisionVolume* parentVolume)
{
	transform	= parentTransform;
	volume		= parentVolume;

	inverseMass = 1.0f;
	elasticity	= 0.8f;
	friction	= 0.8f;
}

PhysicsObject::~PhysicsObject()
{

}

////////////////////////////////////////////////////
// 28.11.2019 - linear motion //////////////////////
////////////////////////////////////////////////////

// at the end of each frame, reset forces so that they aren't applied multiple times next frame
void PhysicsObject::ClearForces()
{
	force = Vector3();
	torque = Vector3();
}

// param v3 represents a direction and an amount of Newtons of force to apply in that direction.
void PhysicsObject::AddForce(const Vector3& addedForce)
{
	force += addedForce; // objects may have multiple forces applied per frame
}

////////////////////////////////////////////////////
// 28.11.2019 - Angular Motion /////////////////////
////////////////////////////////////////////////////

void PhysicsObject::AddForceAtPosition(const Vector3& addedForce, const Vector3& position)
{
	Vector3 localPos = position - transform->GetWorldPosition(); // calculates position relative to object's center of mass.

	force += addedForce;
	torque += Vector3::Cross(localPos, addedForce);  // uses cross product to determine the axis around which force will cause object to spin
}

//////////////////////////////////////////////////////////
// 30.11.2019 - Collision Resolution /////////////////////
//////////////////////////////////////////////////////////
void PhysicsObject::ApplyLinearImpulse(const Vector3& force)
{
	linearVelocity += force * inverseMass;
}

void PhysicsObject::ApplyAngularImpulse(const Vector3& force)
{
	angularVelocity += inverseInteriaTensor * force;
}

////////////////////////////////////////////////////
/// TBD ////////////////////////////////////////////
////////////////////////////////////////////////////
void PhysicsObject::AddTorque(const Vector3& addedTorque)
{
	torque += addedTorque;
}

void PhysicsObject::InitCubeInertia()
{
	const Vector3 dimensions = transform->GetLocalScale();
	const Vector3 fullWidth = dimensions * 2;
	const Vector3 dimsSqr = fullWidth * fullWidth;
	inverseInertia.x = (12.0f * inverseMass) / (dimsSqr.y + dimsSqr.z);
	inverseInertia.y = (12.0f * inverseMass) / (dimsSqr.x + dimsSqr.z);
	inverseInertia.z = (12.0f * inverseMass) / (dimsSqr.x + dimsSqr.y);
}

void PhysicsObject::InitSphereInertia(bool isHollow)
{
	float radius = transform->GetLocalScale().GetMaxElement();
	float i = 0;
	if(isHollow)
	{
		i = 1.5f * inverseMass / (radius * radius);
	}
	else
	{
		i = 2.5f * inverseMass / (radius * radius);
	}
	inverseInertia = Vector3(i, i, i);
}

void PhysicsObject::UpdateInertiaTensor()
{
	Quaternion q = transform->GetWorldOrientation();
	
	Matrix3 invOrientation	= Matrix3(q.Conjugate());
	Matrix3 orientation		= Matrix3(q);

	inverseInteriaTensor = orientation * Matrix3::Scale(inverseInertia) *invOrientation;
}