/***************************************************************************
* Filename		: PhysicsSystem.h
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
#include "../CSC8503Common/GameWorld.h"
#include <set>

namespace NCL
{
	namespace CSC8503
	{
		class PhysicsSystem
		{
		public:
			PhysicsSystem(GameWorld& g);
			~PhysicsSystem();

			void UseGravity(bool state) { applyGravity = state; }
			void SetGlobalDamping(float d) { globalDamping = d; }

			void Clear();
			void Update(float dt);
			void SetGravity(const Vector3& g);
			const Vector3& GetConstGravity() const { return gravity; };
			const float& GetConstGlobalDamping() const { return globalDamping; };
		protected:
			void BasicCollisionDetection();
			void BroadPhase();
			void NarrowPhase();

			void ClearForces();

			void IntegrateAccel(float dt);
			void IntegrateVelocity(float dt);

			void UpdateConstraints(float dt);

			void UpdateCollisionList();
			void UpdateObjectAABBs();

			void ImpulseResolveCollision(GameObject& a , GameObject&b, CollisionDetection::ContactPoint& p) const;

			GameWorld& gameWorld;

			bool	applyGravity;
			Vector3 gravity;
			float	dTOffset;
			float	globalDamping;
			float	frameDT;

			std::set<CollisionDetection::CollisionInfo> allCollisions;
			std::set<CollisionDetection::CollisionInfo>		broadphaseCollisions;
			std::vector<CollisionDetection::CollisionInfo>	broadphaseCollisionsVec;
			bool useBroadPhase		= true;
			int numCollisionFrames	= 5;
		};
	}
}

