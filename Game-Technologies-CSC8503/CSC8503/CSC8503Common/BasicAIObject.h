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
		class PlayerObject;
		class GameWorld;
		class StateMachine;
		class NavigationGrid;
		class NavigationTable;

		enum AIType { BASIC = 1 };
		
		class BasicAIObject : public GameObject
		{
		public:
			BasicAIObject(const Vector3& spawnPos, const int type = 1, const std::string name = "Basic AI");
			virtual ~BasicAIObject();
			virtual void OnCollisionBegin(GameObject* other) override;
			void InitStateMachine();
			void SetWorld(GameWorld* gameWorld) { world = gameWorld; }
			void Update();
			void Move();
			virtual void DebugDraw() override;
			int GetAIType() const { return aiType; }			
			int GetObjectID() const { return objectID; }
			void SetObjectID(const unsigned int id) { objectID = id; }			
			void SetNavigationGrid(NavigationGrid* grid) { navigationGrid = grid; }
			void SetNavigationTable(NavigationTable* table) { navigationTable = table; }	
			void SetTarget(GameObject* other)
			{
				target = other;
				moveTowardsTarget = true;
			}
			void SetPlayerObjectCollection(std::vector<PlayerObject*>* playerCollection) { playerObjectCollection = playerCollection; }
			
			void ClearTarget() { target = nullptr; }
		private:
			bool FindTarget(const Vector3& offset, const Vector3& direction, bool drawLines = false);
		protected:
			GameObject* target;
			unsigned int objectID;
			int aiType;
			Vector3 spawnPosition;
			NavigationGrid* navigationGrid;
			NavigationTable* navigationTable;
			StateMachine* stateMachine;
			bool moveTowardsTarget;
			GameWorld* world;
			std::vector<PlayerObject*>* playerObjectCollection = nullptr;
			int debugStartNodeIndex;
			int debugEndNodeIndex;
		};
	}
}
