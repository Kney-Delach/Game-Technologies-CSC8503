#include "GameState.h"

#include <iostream>
#include "../../Common/Window.h"
#include "../CSC8503Common/Debug.h"

#include "GooseGame.h"
#include "TestPacketReceiver.h"
#include "../CSC8503Common/GameServer.h"
#include "../CSC8503Common/GameClient.h"

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
			if (singlePlayerGame)
			{
				if (gameResult == 0)
				{
					singlePlayerGame->UpdateGame(dt);
					RenderMenu();
					gameResult = singlePlayerGame->GameStatusUpdate(dt);
					if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
					{
						return -2;
					}
				}

				if (gameResult != 0)
				{
					if (singlePlayerGame->VictoryScreenUpdate(dt, gameResult) <= 0)
					{
						// update leaderboards data here
						return -2;
					}
				}
				return -1;
			}
			else
				return -2;
		}

		void SinglePlayerState::OnAwake()
		{
			gameResult = 0;
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
				if (selectedChoice == 2)
					return -2;
				return (selectedChoice + 4);
			}
			return -1;
		}

		void MultiPlayerState::RenderMenu()
		{
			static const Vector4 green(0, 1, 0, 1);
			static const Vector4 black(0, 0, 0, 1);

			static const Vector2 pos4(960, 1000);
			static const Vector2 serverPosition(200, 500);
			static const Vector2 clientPosition(200, 475);
			static const Vector2 pos1(960, 120);

			Debug::Print("Multi-Player Game", pos4, black);

			if (selectedChoice == 0)
			{
				Debug::Print("Start Server Game", serverPosition, green);
				Debug::Print("Start Client Game", clientPosition, black);
				Debug::Print("BACK TO MAIN MENU", pos1, black);
			}
			else if (selectedChoice == 1)
			{
				Debug::Print("Start Server Game", serverPosition, black);
				Debug::Print("Start Client Game", clientPosition, green);
				Debug::Print("BACK TO MAIN MENU", pos1, black);
			}
			else if (selectedChoice == 2)
			{
				Debug::Print("Start Server Game", serverPosition, black);
				Debug::Print("Start Client Game", clientPosition, black);
				Debug::Print("BACK TO MAIN MENU", pos1, green);
			}

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
			static const Vector4 blue(0, 0, 1, 1);

			static const Vector2 pos4(960, 1000);
			Debug::Print("Leaderboards", pos4, black);

			static const Vector2 leaderboard1(600, 800);
			static const Vector2 leaderboard2(600, 750);
			static const Vector2 leaderboard3(600, 700);
			static const Vector2 leaderboard4(600, 650);
			static const Vector2 leaderboard5(600, 600);
			static const Vector2 leaderboard6(600, 550);

			Debug::Print("1 | Rich   | >9000", leaderboard1, blue);
			Debug::Print("2 | Daniel | 12", leaderboard2, black);
			Debug::Print("3 | Goose  | 5", leaderboard3, black);
			Debug::Print("4 | Aaron  | 3", leaderboard4, black);
			Debug::Print("5 | John   | 2", leaderboard5, black);
			Debug::Print("6 | Marvel | -3", leaderboard6, black);

			
			static const Vector2 pos1(960, 120);
			Debug::Print("BACK TO MAIN MENU", pos1, green);
		}

		////////////////////////////////
		//// Server Game
		////////////////////////////////

		int ServerState::Update(float dt)
		{
			if (serverGame)
			{
				server->SendGlobalPacket(StringPacket(" Server says hello ! " + std::to_string(dt)));
				server->UpdateServer();
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				if (gameResult == 0)
				{
					serverGame->UpdateGame(dt);
					RenderMenu();
					gameResult = serverGame->GameStatusUpdate(dt);
					if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
					{
						return -2;
					}
				}

				if (gameResult != 0)
				{
					if (serverGame->VictoryScreenUpdate(dt, gameResult) <= 0)
					{
						// update leaderboards data here
						return -2;
					}
				}
				return -1;
			}
			else
				return -2;
		}

		void ServerState::OnAwake()
		{
			NetworkBase::Initialise();
			//serverReceiver = TestPacketReceiver(" Server ");
			int port = NetworkBase::GetDefaultPort();
			server = new GameServer(port, 1);
			server->RegisterPacketHandler(String_Message, &serverReceiver);

			gameResult = 0;
			serverGame = new GooseGame();
		}

		void ServerState::OnSleep()
		{
			NetworkBase::Destroy();
			delete serverGame;
			serverGame = nullptr;
		}

		void ServerState::RenderMenu()
		{
			static const Vector4 green(0, 1, 0, 1);
			static const Vector4 black(0, 0, 0, 1);

			static const Vector2 pos4(960, 1000);
			Debug::Print("Server Game", pos4, green);
		}
		
		////////////////////////////////
		//// Client Game
		////////////////////////////////

		int ClientState::Update(float dt)
		{
			if (clientGame)
			{
				
				client->SendPacket(StringPacket(" Client says hello ! " + std::to_string(dt)));
				client->UpdateClient();

				if (gameResult == 0)
				{
					clientGame->UpdateGame(dt);
					RenderMenu();
					gameResult = clientGame->GameStatusUpdate(dt);
					if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
					{
						return -2;
					}
				}

				if (gameResult != 0)
				{
					if (clientGame->VictoryScreenUpdate(dt, gameResult) <= 0)
					{
						// update leaderboards data here
						return -2;
					}
				}
				return -1;
			}
			else
				return -2;
		}

		void ClientState::OnAwake()
		{
			//clientReceiver = TestPacketReceiver("Client");
			NetworkBase::Initialise();
			client = new GameClient();
			client->RegisterPacketHandler(String_Message, &clientReceiver);
			int port = NetworkBase::GetDefaultPort();

			bool canConnect = client->Connect(127, 0, 0, 1, port);
			if(canConnect)
			{
				gameResult = 0;
				clientGame = new GooseGame();
			}
		}

		void ClientState::OnSleep()
		{
			NetworkBase::Destroy();
			delete clientGame;
			clientGame = nullptr;
		}

		void ClientState::RenderMenu()
		{
			static const Vector4 green(0, 1, 0, 1);
			static const Vector4 black(0, 0, 0, 1);

			static const Vector2 pos4(960, 1000);
			Debug::Print("Client Game", pos4, green);
		}
	}
}
