/***************************************************************************
* Filename		: PlayerIsland.h
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
#pragma once
#include "GameObject.h"
#include "PlayerObject.h"

namespace NCL
{
	namespace CSC8503
	{
		class PlayerIsland : public GameObject
		{
		public:
			PlayerIsland(const std::string name = "Player Island");
			virtual ~PlayerIsland();
			virtual void OnCollisionBegin(GameObject* other) override;
			void DrawCollectedObjectsToUI() const;
			void SetParent(PlayerObject* player) { parentPlayer = player; }
		protected:
			PlayerObject* parentPlayer = nullptr;
			std::vector<GameObject*> collectedApples;
			std::vector<GameObject*> collectedCorn;
			std::vector<GameObject*> collectedHats;
		};
	}
}
