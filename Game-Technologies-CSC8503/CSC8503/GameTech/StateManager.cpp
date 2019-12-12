#include "StateManager.h"
#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/State.h"
#include "../CSC8503Common/StateTransition.h"
#include "GameState.h"

#include <iostream>
#include "../CSC8503Common/Debug.h"

namespace NCL
{
	namespace CSC8503
	{
		StateManager::StateManager(OGLRenderer* render, int nStates)
			: numberOfStates(nStates), nextStateSelected(0), renderer(render)
		{
			InitStateFSM();
			InitStateStorage();
		}

		StateManager::~StateManager()
		{
			delete stateFSM;
			for(GameState* s : stateStorage)
			{
				delete s;
			}
			stateStorage.clear();
		}

		void StateManager::InitStateStorage()
		{
			stateStorage.reserve(numberOfStates);
			stateStorage.emplace_back(new MainMenuState(0));
			stateStorage.emplace_back(new SinglePlayerState(1));
			stateStorage.emplace_back(new MultiPlayerState(2));
			stateStorage.emplace_back(new LeaderboardsState(3));
		}

		void StateManager::PushSelectedToStack()
		{
			if (!(nextStateSelected < stateStorage.size()))
			{
				std::cout << "state trying to add doesn't exist!\n";
				nextStateSelected = -1; // will take state machine back to idle 
				return;
			}
			stateStack.push(stateStorage[nextStateSelected]);
			nextStateSelected = -1; // will take state machine back to idle
			if(activeState)
				activeState->OnSleep();
			activeState = stateStack.top();
			activeState->OnAwake();
			currentState = activeState->GetID();
		}

		void StateManager::PopMenuStack()
		{
			if (!stateStack.empty())
			{
				stateStack.pop();
			}
			nextStateSelected = -1; // will take state machine back to idle
			activeState->OnSleep();
			if (currentState == 1 || currentState == 2)
			{
				renderer->SetAsActiveContext();
				Debug::SetRenderer(renderer);
			}
			if (!stateStack.empty())
			{
				activeState = stateStack.top();
				activeState->OnAwake();
				currentState = activeState->GetID();
			}
			else
				activeState = nullptr;
		}

		void StateManager::InitStateFSM()
		{
			stateFSM = new StateMachine();

			// setup data to be used within state machine
			void* data = this;

			// setup state specific update functions 
			StateFunction idleState = [](void* data) // next state selected is -1
			{
			};

			StateFunction pushState = [](void* data) // next state selected is >=0
			{
				StateManager* stateManager = ((StateManager*)data);
				std::cout << "Pushing state: " << stateManager->GetNextStateSelected() << " onto the state stack!";
				stateManager->PushSelectedToStack();
			};

			StateFunction popState = [](void* data) // next state selected is -2
			{
				StateManager* stateManager = ((StateManager*)data);
				std::cout << "Poping state from the stack\n";
				stateManager->PopMenuStack();
			};

			GenericState* stateIdle = new GenericState(idleState, static_cast<void*>(this));
			GenericState* statePush = new GenericState(pushState, static_cast<void*>(this));
			GenericState* statePop = new GenericState(popState, static_cast<void*>(this));

			// add states to state machine 
			stateFSM->AddState(stateIdle);
			stateFSM->AddState(statePush);
			stateFSM->AddState(statePop);

			// setup transitions between states created above

			GenericTransition<int&, int>* transitionIdlePush = new GenericTransition<int&, int>(GenericTransition<int&, int>::GreaterThanTransition, ((StateManager*)data)->GetNextStateSelected(), -1, stateIdle, statePush);
			GenericTransition<int&, int>* transitionPushIdle = new GenericTransition<int&, int>(GenericTransition<int&, int>::LessThanTransition, ((StateManager*)data)->GetNextStateSelected(), 0, statePush, stateIdle);
			GenericTransition<int&, int>* transitionIdlePop = new GenericTransition<int&, int>(GenericTransition<int&, int>::EqualsTransition, ((StateManager*)data)->GetNextStateSelected(), -2, stateIdle, statePop);
			GenericTransition<int&, int>* transitionPopIdle = new GenericTransition<int&, int>(GenericTransition<int&, int>::GreaterThanTransition, ((StateManager*)data)->GetNextStateSelected(), -2, statePop, stateIdle);

			// add transitions to state machine
			stateFSM->AddTransition(transitionIdlePush);
			stateFSM->AddTransition(transitionPushIdle);
			stateFSM->AddTransition(transitionIdlePop);
			stateFSM->AddTransition(transitionPopIdle);
		}

		void StateManager::Update(float dt)
		{
			if (activeState)
				nextStateSelected = activeState->Update(dt);
			stateFSM->Update();
			stateFSM->Update();
		}
	}
}