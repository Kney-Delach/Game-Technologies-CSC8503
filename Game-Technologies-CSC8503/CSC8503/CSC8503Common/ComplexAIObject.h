/***************************************************************************
* Filename		: BasicAIObject.h
* Name			: Ori Lazar
* Date			: 9/12/2019
* Description	: Stored the representation of an AI object that uses A* pathfinding
*                 to track down a player.
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

#include "GameObject.h"

namespace NCL
{
	namespace CSC8503
	{
		class PlayerIsland;
		class GameWorld;
		class StateMachine;
		class NavigationGrid;
		class NavigationTable;

		class ComplexAIObject : public GameObject
		{
		public:
			ComplexAIObject(const Vector3& spawnPos, const int type = 1, const std::string name = "Complex AI", float awkTime = 0.f);
			virtual ~ComplexAIObject();
			virtual void OnCollisionBegin(GameObject* other) override;
			void InitStateMachine();
			void SetWorld(GameWorld* gameWorld) { world = gameWorld; }
			void Update(float dt);
			void Move();
			virtual void DebugDraw() override;
			int GetAIType() const { return aiType; }
			int GetObjectID() const { return objectID; }
			void SetObjectID(const unsigned int id) { objectID = id; }
			void SetNavigationGrid(NavigationGrid* grid) { navigationGrid = grid; }
			void SetNavigationTable(NavigationTable* table) { navigationTable = table; }
			void InitCombatStateMachine();
			void SetTarget(GameObject* other)
			{
				target = other;
				moveTowardsTarget = true;
			}
			void SetPlayerIslandCollection(std::vector<PlayerIsland*>* collection) { playerIslandCollection = collection; }
			void ClearTarget() { target = nullptr; }
			void SetStunUse(bool val) { useStun = val; }
			void SetThrowUse(bool val) { useThrow = val; }
			void SetBurpUse(bool val) { useBurp = val; }
			void SetYellUse(bool val) { useYell = val; }
			float GetCooldownTimer() const { return cooldownTimer; }
			void SetCooldownTimer(float val) {cooldownTimer = val; }
			void StunTarget();
			void ThrowTowardsTarget();
			void BurpAtTarget();
			void YellAtTarget();
			int GetRecentAttackChoice() const { return recentAttackChoice; }
			void SetRecentAttackChoice(int choice) { recentAttackChoice = choice; }
			float GetAwakeTime() const { return awakeTime; }
		protected:
			GameObject* target;
			unsigned int objectID;
			int aiType;
			Vector3 spawnPosition;
			NavigationGrid* navigationGrid;
			NavigationTable* navigationTable;
			StateMachine* stateMachine;
			StateMachine* combatStateMachine;
			bool moveTowardsTarget;
			GameWorld* world;
			std::vector<PlayerIsland*>* playerIslandCollection = nullptr;
			int currentIslandIndex;
			int debugStartNodeIndex;
			int debugEndNodeIndex;
			bool attackTarget;
			bool useStun;
			bool useThrow;
			bool useBurp;
			bool useYell;
			int recentAttackChoice;
			float cooldownTimer;
			float awakeTime;
		};
	}
}
