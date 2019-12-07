/***************************************************************************
* Filename		: CollectableObject.h
* Name			: Ori Lazar
* Date			: 06/12/2019
* Description	: Declaration of overrides for Objects which are collectable by the player.
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
		enum CollectableType { APPLE = 1, CORN = 2, HAT = 4};
		
		class CollectableObject : public GameObject
		{
		public:
			CollectableObject(const int collType = 1, const std::string name = "Generic Collectable");
			virtual ~CollectableObject();

			virtual void OnCollisionBegin(GameObject* other) override;

			void SetParent(GameObject* newParent) { parent = newParent; }
			GameObject* GetParent() const { return parent; }

			int GetCollectableType() const { return collectableType; }

			int GetObjectID() const { return objectID; }
			void SetObjectID(const unsigned int id) { objectID = id; }

			void DropToInitialSpawn();

		protected:
			GameObject* parent;
			unsigned int objectID;
			int collectableType;
			Vector3 spawnPosition;
		};
	}
}