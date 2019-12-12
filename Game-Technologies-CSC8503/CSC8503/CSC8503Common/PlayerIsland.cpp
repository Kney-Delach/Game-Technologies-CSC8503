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
#include "BasicAIObject.h"
#include "Debug.h"
#include <string>

namespace NCL
{
	namespace CSC8503
	{
		PlayerIsland::PlayerIsland(const std::string name)
			: GameObject(name), score(0)
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
				for (GameObject*& object : ((PlayerObject*)other)->GetInventoryApples())
				{
					CollectableObject* obj = (CollectableObject*)object;
					obj->SetParent(this);
					collectedApples.push_back(object);
					obj->SetObjectID(collectedApples.size() - 1);
					object->UpdateIslandStoredObjectPosition(GetConstTransform().GetWorldPosition(), 0, collectedApples.size() - 1); // apples are in 0th row
					score++;
				}

				for (GameObject*& object : ((PlayerObject*)other)->GetInventoryCorn())
				{
					CollectableObject* obj = (CollectableObject*)object;
					obj->SetParent(this);
					collectedCorn.push_back(object);
					obj->SetObjectID(collectedCorn.size() - 1);
					object->UpdateIslandStoredObjectPosition(GetConstTransform().GetWorldPosition(), 1, collectedCorn.size() - 1); // corn is in 1st row
					score += 5;
				}

				for (GameObject*& object : ((PlayerObject*)other)->GetInventoryHat())
				{
					CollectableObject* obj = (CollectableObject*)object;
					obj->SetParent(this);
					collectedHats.push_back(object);
					obj->SetObjectID(collectedHats.size() - 1);
					object->UpdateIslandStoredObjectPosition(GetConstTransform().GetWorldPosition(), 2, collectedHats.size() - 1); // hats are in 2nd row
					score += 10;
				}

				// empty player inventory
				((PlayerObject*)other)->GetInventoryApples().clear();
				((PlayerObject*)other)->GetInventoryCorn().clear();
				((PlayerObject*)other)->GetInventoryHat().clear();

				parentPlayer->SetBonusItemStatus(false);
			}
		}

		void PlayerIsland::DrawPlayerScore() const
		{
			static const Vector4 green(0, 1, 0, 1);
			static const Vector2 scorePos(5, 150);
			static const Vector2 appleCollectedPos(5, 125);
			static const Vector2 cornCollectedPos(5, 100);
			static const Vector2 hatsCollectedPos(5, 75);
			const std::string sc = "Score: " + std::to_string(score);
			const std::string apples = "Apples: " + std::to_string(collectedApples.size()) + "/" + std::to_string(maxApples);
			const std::string corn = "Corn: " + std::to_string(collectedCorn.size()) + "/" + std::to_string(maxCorn);
			const std::string hats = "Hats: " + std::to_string(collectedHats.size()) + "/" + std::to_string(maxHats);
			Debug::Print(sc, scorePos, green);
			Debug::Print(apples, appleCollectedPos, green);
			Debug::Print(corn, cornCollectedPos, green);
			Debug::Print(hats, hatsCollectedPos, green);
		}

		void PlayerIsland::SetMaxCollectables(const int apples, const int corn, const int hats)
		{
			maxApples = apples;
			maxCorn = corn;
			maxHats = hats;
		}

		void PlayerIsland::DrawFinalScore(int index) const
		{
			index++;
			
			static const Vector4 green(0, 1, 0, 1);
			static const Vector4 red(1, 0, 0, 1);

			Vector4 activeColor;
			
			if (hasWon)
				activeColor = green;
			else
				activeColor = red;
			
			static const Vector2 scorePos(250 * index, 150);
			static const Vector2 appleCollectedPos(250 * index, 125);
			static const Vector2 cornCollectedPos(250 * index, 100);
			static const Vector2 hatsCollectedPos(250 * index, 75);
			const std::string sc = "Score: " + std::to_string(score);
			const std::string apples = "Apples: " + std::to_string(collectedApples.size()) + "/" + std::to_string(maxApples);
			const std::string corn = "Corn: " + std::to_string(collectedCorn.size()) + "/" + std::to_string(maxCorn);
			const std::string hats = "Hats: " + std::to_string(collectedHats.size()) + "/" + std::to_string(maxHats);
			Debug::Print(sc, scorePos, Vector4(0,0,1,1));
			Debug::Print(apples, appleCollectedPos, activeColor);
			Debug::Print(corn, cornCollectedPos, activeColor);
			Debug::Print(hats, hatsCollectedPos, activeColor);
		}
	}
}
