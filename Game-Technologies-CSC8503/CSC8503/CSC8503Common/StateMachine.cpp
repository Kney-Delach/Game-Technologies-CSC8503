/***************************************************************************
* Filename		: StateMachine.cpp
* Name			: Ori Lazar
* Date			: 07/12/2019
* Description	: 
    .---.
  .'_:___".
  |__ --==|
  [  ]  :[|
  |__| I=[|
  / / ____|
 |-/.____.'
/___\ /___\
***************************************************************************/
#include "StateMachine.h"
#include "State.h"
#include "StateTransition.h"
#include <iostream>
#include "Debug.h"

namespace NCL
{
	namespace CSC8503
	{
		StateMachine::StateMachine()
		{
			activeState = nullptr;
		}

		StateMachine::~StateMachine()
		{

		}

		// 7.12.19
		// the first state added to the state machine is defaulted to the starting state of the system 
		void StateMachine::AddState(State* state)
		{
			allStates.emplace_back(state);
			if (activeState == nullptr)
				activeState = state;
		}

		// 7.12.19
		void StateMachine::AddTransition(StateTransition* transition)
		{
			allTransitions.insert(std::make_pair(transition->GetSourceState(), transition));
		}

		// 7.12.19
		// 1. updates the currently activate state
		// 2. get the subset of transitions coming off that state,
		// 3. if one of them can transition to a new state, get it and set it to the new active state
		void StateMachine::Update()
		{
			if(activeState)
			{
				activeState->Update();
				auto transitionSubset = allTransitions.equal_range(activeState);
				for (auto& i = transitionSubset.first; i != transitionSubset.second; ++i)
				{
					if (i->second->CanTransition())
					{
						State* newState = i->second->GetDestinationState();
						activeState = newState;
					}
				}
			}
		}

		void StateMachine::DebugDraw(int index)
		{
			if(activeState)
			{
				activeState->DebugDraw(index);
			}
		}
	}
}
