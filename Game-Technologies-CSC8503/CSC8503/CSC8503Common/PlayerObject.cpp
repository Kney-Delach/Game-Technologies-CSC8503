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
		}

		PlayerObject::~PlayerObject()
		{
		}

		unsigned int PlayerObject::AddObjectToInventory(GameObject* object)
		{
			inventory.push_back(object);
			return inventory.size() - 1;
		}

		void PlayerObject::UpdateInventoryTransformations(const float dt) const
		{
			for (unsigned int i = 0; i < inventory.size(); i++)
			{
				inventory[i]->UpdateObjectPosition(dt, GetConstTransform().GetWorldPosition(),i + 1);
			}
		}

		//todo: add an option to drop items to different locations on the map rather than all to their initial spawn point
		void PlayerObject::DropItems() 
		{
			for (unsigned int i = 0; i < inventory.size(); i++)
			{
				dynamic_cast<CollectableObject*>(inventory[i])->DropToInitialSpawn();
			}
			inventory.clear();
		}

		void PlayerObject::DrawInventoryToUI() const
		{
			const std::string out = std::string("Collected Items in Inventory Count: ") + std::to_string(inventory.size());
			Debug::Print(out.c_str(), Vector2(10.f,1000.f));
		}
	}
}
