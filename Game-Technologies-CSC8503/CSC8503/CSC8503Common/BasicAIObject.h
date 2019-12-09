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
		enum AIType { FARMER = 1, ADVANCED = 2, SUPER_ADVANCED = 4 };
		
		class BasicAIObject : public GameObject
		{
		public:
			BasicAIObject(const int collType = 1, const std::string name = "Basic Simple AI");
			virtual ~BasicAIObject();

			virtual void OnCollisionBegin(GameObject* other) override;

			int GetAIType() const { return aiType; }

			int GetObjectID() const { return objectID; }
			void SetObjectID(const unsigned int id) { objectID = id; }

			void ReturnHome();
			void TargetPlayer(GameObject* player);

		protected:
			GameObject* parent;
			unsigned int objectID;
			int aiType;
			Vector3 spawnPosition;
		};
	}
}