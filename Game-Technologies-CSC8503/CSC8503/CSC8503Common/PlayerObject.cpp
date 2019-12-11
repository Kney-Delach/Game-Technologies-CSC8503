/***************************************************************************
* Filename		: PlayerObject.h
* Name			: Ori Lazar
* Date			: 06/12/2019
* Description	: Player controlled gameobject
	.---.
  .'_:___".
  |__ --==|
  [  ]  :[|
  |__| I=[|
  / / ____|
 |-/.____.'
/___\ /___\
***************************************************************************/
#include "PlayerObject.h"
#include "CollectableObject.h"
#include "Debug.h"

//todo: implement different inventories for different collectables
namespace NCL
{
	namespace CSC8503
	{
		PlayerObject::PlayerObject(const std::string name)
			: GameObject(name)
		{
			isCarryingBonusItem = false;
		}

		PlayerObject::~PlayerObject()
		{
		}

		unsigned int PlayerObject::AddObjectToInventory(GameObject* object)
		{
			CollectableObject* obj = (CollectableObject*)object;
			switch (obj->GetCollectableType())
			{
				case CollectableType::APPLE:
				{
					collectedApples.push_back(object);
					return collectedApples.size() - 1;
				}
				case CollectableType::CORN:
				{
					isCarryingBonusItem = true;
					collectedCorn.push_back(object);
					return collectedCorn.size() - 1;
				}
				case CollectableType::HAT:
				{
					isCarryingBonusItem = true;
					collectedHats.push_back(object);
					return collectedHats.size() - 1;
				}
			}
		}

		void PlayerObject::UpdateInventoryTransformations(const float dt) const
		{
			int count = 1;
			for (unsigned int i = 0; i < collectedApples.size(); i++)
			{
				collectedApples[i]->UpdateObjectPosition(dt, GetConstTransform().GetWorldPosition(),count++);
			}
			for (unsigned int i = 0; i < collectedCorn.size(); i++)
			{
				collectedCorn[i]->UpdateObjectPosition(dt, GetConstTransform().GetWorldPosition(), count++);
			}
			for (unsigned int i = 0; i < collectedHats.size(); i++)
			{
				collectedHats[i]->UpdateObjectPosition(dt, GetConstTransform().GetWorldPosition(), count++);
			}
		}

		//todo: add an option to drop items to different locations on the map rather than all to their initial spawn point
		void PlayerObject::DropItems() 
		{
			isCarryingBonusItem = false;
			for(unsigned int i = 0; i < collectedApples.size(); i++)
			{
				dynamic_cast<CollectableObject*>(collectedApples[i])->DropToInitialSpawn();
			}
			for (unsigned int i = 0; i < collectedCorn.size(); i++)
			{
				dynamic_cast<CollectableObject*>(collectedCorn[i])->DropToInitialSpawn();
			}
			for (unsigned int i = 0; i < collectedHats.size(); i++)
			{
				dynamic_cast<CollectableObject*>(collectedHats[i])->DropToInitialSpawn();
			}
			collectedApples.clear();
			collectedCorn.clear();
			collectedHats.clear();
		}

		void PlayerObject::DrawInventoryToUI() const
		{
			const std::string out = std::string("Apples in Bag: ") + std::to_string(collectedApples.size());
			Debug::Print(out.c_str(), Vector2(10.f,1000.f));
			const std::string outCorn = std::string("Corn in Bag: ") + std::to_string(collectedCorn.size());
			Debug::Print(outCorn.c_str(), Vector2(10.f, 950.f));
			const std::string outHats = std::string("Farmer Hats in Bag: ") + std::to_string(collectedHats.size());
			Debug::Print(outHats.c_str(), Vector2(10.f, 900.f));
		}
	}
}
