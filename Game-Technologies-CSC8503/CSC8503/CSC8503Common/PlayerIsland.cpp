/***************************************************************************
* Filename		: PlayerIsland.cpp
* Name			: Ori Lazar
* Date			: 06/12/2019
* Description	: Represents the spawn "home" island of a player.
    .---.
  .'_:___".
  |__ --==|
  [  ]  :[|
  |__| I=[|
  / / ____|
 |-/.____.'
/___\ /___\
***************************************************************************/
#include "PlayerIsland.h"
#include "CollectableObject.h"


namespace NCL
{
	namespace CSC8503
	{
		PlayerIsland::PlayerIsland(const std::string name)
			: GameObject(name)
		{
		}

		PlayerIsland::~PlayerIsland()
		{
		}

		void PlayerIsland::OnCollisionBegin(GameObject* other)
		{
			// only this island's owner can interact with this island
			if (parentPlayer == other && other->GetPhysicsObject()->UsesGravity() && other->GetPhysicsObject()->GetCollisionType() & ObjectCollisionType::COLLECTABLE)
			{
				for (GameObject*& object : ((PlayerObject*)other)->GetInventory())
				{
					//handle the object
					CollectableObject* obj = (CollectableObject*)object;
					obj->SetParent(this);
					switch(obj->GetCollectableType())
					{
						case CollectableType::APPLE:
						{
							collectedApples.push_back(object);
							obj->SetObjectID(collectedApples.size()-1);
							object->UpdateIslandStoredObjectPosition(GetConstTransform().GetWorldPosition(),0, collectedApples.size() - 1); // apples are in 0th row
							break;
						}
						case CollectableType::CORN:
						{
							collectedCorn.push_back(object);
							obj->SetObjectID(collectedCorn.size() - 1);
							object->UpdateIslandStoredObjectPosition(GetConstTransform().GetWorldPosition(), 1, collectedCorn.size() - 1); // corn are in 1st row
							break;
						}
						case CollectableType::HAT:
						{
							collectedHats.push_back(object);
							obj->SetObjectID(collectedHats.size() - 1);
							object->UpdateIslandStoredObjectPosition(GetConstTransform().GetWorldPosition(), 2, collectedHats.size() - 1); // hats are in 2nd row
							break;
						}
					}
				}
				((PlayerObject*)other)->GetInventory().clear(); // empty player inventory
			}
		}

		void PlayerIsland::DrawCollectedObjectsToUI() const
		{
		}//todo: implement me || maybe make this a debug draw override?
	}
}