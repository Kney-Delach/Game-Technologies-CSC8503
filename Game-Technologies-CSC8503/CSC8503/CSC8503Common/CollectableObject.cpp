/***************************************************************************
* Filename		: CollectableObject.cpp
* Name			: Ori Lazar
* Date			: 06/12/2019
* Description	: Implementation of overrides for Objects which are collectable by the player.
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
#include "CollectableObject.h"
#include "PlayerObject.h"
#include "BasicAIObject.h"
#include "../../Common/Assets.h" // for sound effects

namespace NCL
{
	namespace CSC8503
	{
		CollectableObject::CollectableObject(const int collType, const std::string name)
			: GameObject(name), objectID(0), collectableType(collType)
		{
			//ownerAI = nullptr;
		}

		CollectableObject::~CollectableObject()
		{
		}

		void CollectableObject::OnCollisionBegin(GameObject* other)
		{
			//only goose can move and has Collect impulse resolution
			if (parent == nullptr && other->GetPhysicsObject()->UsesGravity() && other->GetPhysicsObject()->GetCollisionType() & ObjectCollisionType::COLLECTABLE)
			{
				spawnPosition = this->GetConstTransform().GetWorldPosition();

				parent = other;
				objectID = ((PlayerObject*)other)->AddObjectToInventory((GameObject*)this);
				UpdateObjectPosition(0, parent->GetConstTransform().GetWorldPosition(), objectID);
				//todo: add collection sound effect
				if(name == "Apple")
				{
					// play apple sound effect
					PlaySound(TEXT("../../Assets/Audio/CollectApple.wav") , NULL, SND_FILENAME  | SND_ASYNC );
				}
				else if(name == "Hat")
				{
					PlaySound(TEXT("../../Assets/Audio/CollectApple.wav"), NULL, SND_FILENAME | SND_ASYNC);
					//if (ownerAI)
					//{
					//	ownerAI->SetTarget(other);
					//}
				}
			}
		}

		void CollectableObject::DropToInitialSpawn()
		{
			this->GetTransform().SetWorldPosition(spawnPosition);
			parent = nullptr;
			objectID = 0;
			PlaySound(TEXT("../../Assets/Audio/DropItems.wav"), NULL, SND_FILENAME | SND_ASYNC);

		}
	}
}
