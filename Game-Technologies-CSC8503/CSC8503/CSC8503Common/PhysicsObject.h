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
	enum ObjectCollisionType { IMPULSE = 1, SPRING = 2, COLLECTABLE = 4, JUMP_PAD = 8 };
	
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
			void AddForceAtRelativePosition(const Vector3& force, const Vector3& position);

			// 30.11.2019 - collision resolution (impulses) 
			void ApplyAngularImpulse(const Vector3& force);
			void ApplyLinearImpulse(const Vector3& force);

			// 2.12.2019 - dynamic restitution coefficients
			float GetElasticity() const { return elasticity; }
			void SetElasticity(float elast) { elasticity = elast; }
			
			float GetFriction() const { return friction; }
			void SetFriction(float fric) { friction = fric; }
			
			float GetStiffness() const { return stiffness; }
			void SetStiffness(float stiff) { stiffness = stiff; }

			// 3.12.2019 - used to determine which collision response method to utilise
			//bool GetResolveAsSpring() const { return resolveAsSpring; }
			//void SetResolveAsSpring(bool resolve) { resolveAsSpring = resolve; }
			//bool GetResolveAsImpulse() const { return resolveAsImpulse; }
			//void SetResolveAsImpulse(bool resolve) { resolveAsImpulse = resolve; }

			// 6.12.2019 - used to determine which collision response method to utilise
			void SetCollisionType(const int value) { bitCollisionType = value; }
			int GetCollisionType() const { return bitCollisionType; }

			// 6.12.2019 - toggle gravity usage
			bool UsesGravity() const { return usesGravity; }
			void SetGravityUsage(bool value) { usesGravity = value; }
			
			// tbd....				
			void AddTorque(const Vector3& torque);
			void InitCubeInertia();
			void InitSphereInertia(bool isHollow = false);
			void UpdateInertiaTensor();
		protected:
			const CollisionVolume* volume;
			Transform* transform;

			// 28.11.2019 - linear motion 
			float inverseMass;
			
			Vector3 linearVelocity;
			Vector3 force;
			
			// 2.12.2019 - Dynamic Restitution Coefficients 
			float elasticity;
			float friction;
			float stiffness;

			//angular stuff
			Vector3 angularVelocity;
			Vector3 torque;
			Vector3 inverseInertia;
			Matrix3 inverseInteriaTensor;

			// 6.12.2019 - collision response bitwise comparison reference
			int bitCollisionType;

			bool usesGravity;
		};
	}
}