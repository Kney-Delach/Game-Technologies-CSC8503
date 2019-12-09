/***************************************************************************
* Filename		: BasicAIObject.cpp
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
#include "BasicAIObject.h"

#include "CollisionDetection.h"
#include "Debug.h"

namespace NCL
{
	namespace CSC8503
	{
		BasicAIObject::BasicAIObject(const Vector3& spawnPos, const int type, const std::string name)
			: GameObject(name), aiType(type), objectID(0), spawnPosition(spawnPos)
		{}

		void BasicAIObject::OnCollisionBegin(GameObject* other)
		{
		}

		void BasicAIObject::ReturnHome()
		{
		}

		void BasicAIObject::TargetPlayer(GameObject* player)
		{
		}
	}
}