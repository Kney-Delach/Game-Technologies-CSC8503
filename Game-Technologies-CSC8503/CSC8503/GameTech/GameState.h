#pragma once

#include <string>
#include <vector>
#include "Leaderboard.h"
#include "TestPacketReceiver.h"

namespace NCL
{
	namespace CSC8503
	{
		class GooseGame;
		class GameServer;
		class GameClient;
		
		class GameState
		{
		public:
			GameState(int id = 0, const std::string& stateName = "State") : stateID(id), m_Name(stateName) {}
			virtual ~GameState() = default;
		public:
			std::string& GetName() { return m_Name; }
			virtual void OnAwake() {};
			virtual void OnSleep() {};
			virtual int Update(float dt);
			int GetID() const { return stateID; }
		protected:
			std::string m_Name;
			int stateID;
		};

		class MainMenuState : public GameState
		{
		public:
			MainMenuState(int id = 0,int maximumChoices = 3) : GameState(id,"Main Menu"), selectedChoice(0), maxChoices(maximumChoices) {}
			virtual ~MainMenuState() = default;
			virtual int Update(float dt) override;
		private:
			void RenderMenu();
		private:
			int selectedChoice;
			int maxChoices;
		};

		class SinglePlayerState : public GameState
		{
		public:
			SinglePlayerState(int id = 1, int maximumChoices = 2) : GameState(id, "Single Player Goose Game"), selectedChoice(0), maxChoices(maximumChoices), gameResult(0), gameOverTimer(20.f) { singlePlayerGame = nullptr; }
			virtual ~SinglePlayerState() = default;
			virtual int Update(float dt) override;
			virtual void OnAwake();
			virtual void OnSleep();
		private:
			void RenderMenu();
		private:
			int selectedChoice;
			int maxChoices;
			GooseGame* singlePlayerGame;
			int gameResult;
			float gameOverTimer;
		};


		class MultiPlayerState : public GameState
		{
		public:
			MultiPlayerState(int id = 2, int maximumChoices = 2) : GameState(id, "Multi-Player Goose Game"), selectedChoice(0), maxChoices(maximumChoices) {}
			virtual ~MultiPlayerState() = default;
			virtual int Update(float dt) override;
		private:
			void RenderMenu();
		private:
			int selectedChoice;
			int maxChoices;
		};

		//todo: add state for game complete
		class LeaderboardsState : public GameState
		{
		public:
			LeaderboardsState(int id = 3, int maximumChoices = 1) : GameState(id, "Leaderboard"), selectedChoice(0), maxChoices(maximumChoices) {}
			virtual ~LeaderboardsState() = default;
			virtual int Update(float dt) override;
		public:
			static Leaderboard s_Leaderboard;
		private:
			void RenderMenu();
		private:
			int selectedChoice;
			int maxChoices;
		};

		class ServerState : public GameState
		{
		public:
			ServerState(int id = 1) : GameState(id, "Server Game"), gameResult(0), gameOverTimer(20.f), serverReceiver("Server"), server(nullptr), serverGame(nullptr) {}
			virtual ~ServerState() = default;
			virtual int Update(float dt) override;
			virtual void OnAwake();
			virtual void OnSleep();
		private:
			void RenderMenu();
		private:
			GooseGame* serverGame;
			GameServer* server;
			TestPacketReceiver serverReceiver;
			int gameResult;
			float gameOverTimer;
		};

		class ClientState : public GameState
		{
		public:
			ClientState(int id = 1) : GameState(id, "Client Game"), gameResult(0), gameOverTimer(20.f), clientReceiver("Client"), client(nullptr), clientGame(nullptr) { }
			virtual ~ClientState() = default;
			virtual int Update(float dt) override;
			virtual void OnAwake();
			virtual void OnSleep();
		private:
			void RenderMenu();
		private:
			GooseGame* clientGame;
			GameClient* client;
			TestPacketReceiver clientReceiver;
			int gameResult;
			float gameOverTimer;
		};
		
	}
}
