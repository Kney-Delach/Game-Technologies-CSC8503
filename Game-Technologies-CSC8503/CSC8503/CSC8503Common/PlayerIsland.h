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
			void DrawPlayerScore() const;
			void SetParent(PlayerObject* player) { parentPlayer = player; }
			PlayerObject* GetParent() const { return parentPlayer; }
			int GetScore() const { return score; }
			int GetApplesCount() const { return (int) collectedApples.size(); }
			void SetMaxCollectables(const int apples, const int corn, const int hats);
			void DrawFinalScore(int index) const;
			void SetWinnerStatus(bool result) { hasWon = result; }
			void UpdateScore(int add) { score += add; }
		protected:
			PlayerObject* parentPlayer = nullptr;
			std::vector<GameObject*> collectedApples;
			std::vector<GameObject*> collectedCorn;
			std::vector<GameObject*> collectedHats;
			int score;
			int maxApples;
			int maxCorn;
			int maxHats;
			bool hasWon;
		};
	}
}