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
#include "FloatToString.h"

//todo: implement different inventories for different collectables
namespace NCL
{
	namespace CSC8503
	{
		PlayerObject::PlayerObject(const std::string name)
			: GameObject(name), isCarryingBonusItem(false), isGrounded(false), fartCD(0.f), canFart(true), fartRadius(100.f)
		{
		}

		PlayerObject::~PlayerObject()
		{
		}

		void PlayerObject::UpdateCooldown(float dt)
		{
			static const Vector4 red = Vector4(1, 0, 0, 1);
			static const Vector4 green = Vector4(0, 1, 0, 1);
			
			if(fartCD <= 0)
			{
				const std::string out = std::string("Press Q to Fart!");
				Debug::Print(out, Vector2(5.f, 250.f), green);
				fartCD = 0;
				canFart = true;
				return;
			}
			const std::string out = std::string("Fart CD: ") + FloatToString<float>(fartCD, 2);
			Debug::Print(out, Vector2(5.f, 250.f), red);
			fartCD -= dt;
		}

		void PlayerObject::OnCollisionBegin(GameObject* other)
		{
			if (other->GetName() == "Ground")
			{
				isGrounded = true;
			}
		}

		void PlayerObject::OnCollisionEnd(GameObject* other)
		{
			if (other->GetName() == "Ground")
			{
				isGrounded = false;
			}
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
	}
}
