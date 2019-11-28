/***************************************************************************
* Filename		: PhysicsObject.h
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
#pragma once
#include "../../Common/Vector3.h"
#include "../../Common/Matrix3.h"

using namespace NCL::Maths;

namespace NCL
{
	class CollisionVolume;
	
	namespace CSC8503
	{
		class Transform;

		class PhysicsObject
		{
		public:
			PhysicsObject(Transform* parentTransform, const CollisionVolume* parentVolume);
			~PhysicsObject();

			Vector3 GetAngularVelocity() const { return angularVelocity; }
			Vector3 GetTorque() const { return torque; }
			Vector3 GetForce() const { return force; }			
			void SetInverseMass(float invMass) { inverseMass = invMass; }
			float GetInverseMass() const { return inverseMass; }
			void SetAngularVelocity(const Vector3& v) { angularVelocity = v; }
			Matrix3 GetInertiaTensor() const { return inverseInteriaTensor; }

			// 28.11.2019 - linear motion 
			void ClearForces();
			void AddForce(const Vector3& force);
			Vector3 GetLinearVelocity() const { return linearVelocity; }
			void SetLinearVelocity(const Vector3& v) { linearVelocity = v; }

			// 28.11.2019 - angular motion 
			void AddForceAtPosition(const Vector3& force, const Vector3& position);

			// tbd....
			void ApplyAngularImpulse(const Vector3& force);
			void ApplyLinearImpulse(const Vector3& force);			
			void AddTorque(const Vector3& torque);
			void InitCubeInertia();
			void InitSphereInertia(bool isHollow = false);
			void UpdateInertiaTensor();
		protected:
			const CollisionVolume* volume;
			Transform*		transform;

			// 28.11.2019 - linear motion 
			float inverseMass;
			float elasticity;
			float friction;
			Vector3 linearVelocity;
			Vector3 force;
			

			//angular stuff
			Vector3 angularVelocity;
			Vector3 torque;
			Vector3 inverseInertia;
			Matrix3 inverseInteriaTensor;
		};
	}
}

