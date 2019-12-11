#pragma once

#include <stack>
#include <vector>
#include <iostream>
#include "../../Plugins/OpenGLRendering/OGLRenderer.h"

namespace NCL
{
	namespace CSC8503
	{
		class GameState;
		class StateMachine;
		
		class StateManager
		{
		public:
			StateManager(OGLRenderer* render, int nStates);
			~StateManager();
			void Update(float dt);
			int& GetNextStateSelected() { return nextStateSelected; }
			void SetNextStateSelected(int value) { nextStateSelected = value; }
			std::stack<GameState*>& GetStateStack() { return stateStack; }
			std::vector<GameState*>& GetStateStorage() { return stateStorage; }
			void PushSelectedToStack();
			void PopMenuStack();
		public:
			int currentState;
		private:
			void InitStateFSM();
			void InitStateStorage();
		private:
			int numberOfStates;
			int nextStateSelected;
			GameState* activeState = nullptr; 
			std::stack<GameState*> stateStack;
			std::vector<GameState*> stateStorage;
			StateMachine* stateFSM;
			OGLRenderer* renderer;
		};
	}
}
