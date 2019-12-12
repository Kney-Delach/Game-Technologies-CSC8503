#include "GameState.h"

#include <iostream>
#include "../../Common/Window.h"
#include "../CSC8503Common/Debug.h"

#include "../GameTech/GooseGame.h"
namespace NCL
{
	namespace CSC8503
	{
		int GameState::Update(float dt)
		{
			return -1;
		}

		////////////////////////////////
		//// Main Menu
		////////////////////////////////
		int MainMenuState::Update(float dt)
		{
			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::UP))
			{
				if (selectedChoice > 0)
					selectedChoice--;
				else
					selectedChoice = maxChoices;
			}
			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::DOWN))
			{
				if (selectedChoice < maxChoices)
					selectedChoice++;
				else
					selectedChoice = 0;
			}
			RenderMenu();

			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::RETURN))
			{
				if (selectedChoice == 3)
					return -2;
				return (selectedChoice + 1);
			}
			return -1;
		}

		void MainMenuState::RenderMenu()
		{
			static const Vector4 green(0, 1, 0, 1);
			static const Vector4 black(0,0,0,1);

			static const Vector2 pos1(960, 480);
			static const Vector2 pos2(960, 540);
			static const Vector2 pos3(960, 600);
			static const Vector2 pos4(960, 660);
			if(selectedChoice == 0)
			{
				Debug::Print("Single Player", pos4, green);
				Debug::Print("Two Players", pos3, black);
				Debug::Print("Leaderboards", pos2, black);
				Debug::Print("Quit", pos1, black);
			}
			else if (selectedChoice == 1)
			{
				Debug::Print("Single Player", pos4, black);
				Debug::Print("Two Players", pos3, green);
				Debug::Print("Leaderboards", pos2, black);
				Debug::Print("Quit", pos1, black);
			}
			else if (selectedChoice == 2)
			{
				Debug::Print("Single Player", pos4, black);
				Debug::Print("Two Players", pos3, black);
				Debug::Print("Leaderboards", pos2, green);
				Debug::Print("Quit", pos1, black);
			}
			else if (selectedChoice == 3)
			{
				Debug::Print("Single Player", pos4, black);
				Debug::Print("Two Players", pos3, black);
				Debug::Print("Leaderboards", pos2, black);
				Debug::Print("Quit", pos1, green);
			}
		}

		////////////////////////////////
		//// Single Player
		////////////////////////////////

		int SinglePlayerState::Update(float dt)
		{
			if(singlePlayerGame)
			{
				singlePlayerGame->UpdateGame(dt);
				RenderMenu();
			}
			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
			{
				return -2;
			}
			return -1;
		}

		void SinglePlayerState::OnAwake()
		{
			singlePlayerGame = new GooseGame();
		}

		void SinglePlayerState::OnSleep()
		{
			delete singlePlayerGame;
			singlePlayerGame = nullptr;
		}

		void SinglePlayerState::RenderMenu()
		{
			static const Vector4 green(0, 1, 0, 1);
			static const Vector4 black(0, 0, 0, 1);

			static const Vector2 pos4(960, 1000);
			Debug::Print("Single Player Game", pos4, green);
		}

		////////////////////////////////
		//// Multi Player
		////////////////////////////////


		int MultiPlayerState::Update(float dt)
		{
			RenderMenu();
			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
			{
				return -2;
			}
			return -1;
		}

		void MultiPlayerState::RenderMenu()
		{
			static const Vector4 green(0, 1, 0, 1);
			static const Vector4 black(0, 0, 0, 1);

			static const Vector2 pos4(960, 1000);
			Debug::Print("Multi-Player Game", pos4, green);
		}
		
		////////////////////////////////
		//// Leaderboards
		////////////////////////////////


		int LeaderboardsState::Update(float dt)
		{
			RenderMenu();
			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::RETURN))
			{
				return -2;
			}
			return -1;
		}

		void LeaderboardsState::RenderMenu()
		{
			static const Vector4 green(0, 1, 0, 1);
			static const Vector4 black(0, 0, 0, 1);

			static const Vector2 pos1(960, 120);
			Debug::Print("BACK TO MAIN MENU", pos1, green);
		}
	}
}
