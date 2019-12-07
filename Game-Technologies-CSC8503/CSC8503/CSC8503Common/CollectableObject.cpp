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

namespace NCL
{
	namespace CSC8503
	{
		CollectableObject::CollectableObject(const int collType, const std::string name)
			: GameObject(name), objectID(0), collectableType(collType)
		{
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
			}
		}

		void CollectableObject::DropToInitialSpawn()
		{
			this->GetTransform().SetWorldPosition(spawnPosition);
			parent = nullptr;
			objectID = 0;
		}
	}
}