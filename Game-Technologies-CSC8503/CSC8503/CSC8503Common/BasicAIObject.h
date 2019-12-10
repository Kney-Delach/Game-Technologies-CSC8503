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
		class NavigationGrid;
		class NavigationTable;

		enum AIType { BASIC = 1 };
		
		class BasicAIObject : public GameObject
		{
		public:
			BasicAIObject(const Vector3& spawnPos, const int type = 1, const std::string name = "Basic AI");
			virtual ~BasicAIObject();
			
			virtual void OnCollisionBegin(GameObject* other) override;
			
			int GetAIType() const { return aiType; }
			
			int GetObjectID() const { return objectID; }
			void SetObjectID(const unsigned int id) { objectID = id; }
			
			void SetNavigationGrid(NavigationGrid* grid) { navigationGrid = grid; }
			void SetNavigationTable(NavigationTable* table) { navigationTable = table; }

			void Move();
			void ReturnHome();

			void DebugDraw();
			
			void SetTarget(GameObject* other);
			void ClearTarget() { target = nullptr; }
		protected:
			GameObject* target;
			unsigned int objectID;
			int aiType;
			Vector3 spawnPosition;
			NavigationGrid* navigationGrid;
			NavigationTable* navigationTable;
		};
	}
}
