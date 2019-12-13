/***************************************************************************
* Filename		: StateMachine.h
* Name			: Ori Lazar
* Date			: 07/12/2019
* Description	: Representation of a state machine
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
#include <vector>
#include <map>

namespace NCL
{
	namespace CSC8503
	{
		class State;
		class StateTransition;
		
		typedef std::multimap<State*, StateTransition*> TransitionContainer; // a (directed graph representation) map of states to their transitions
		typedef TransitionContainer::iterator TransitionIterator; // std iterator for the map declared above

		class StateMachine
		{
		public:
			StateMachine();
			~StateMachine();
			void AddState(State* state);
			void AddTransition(StateTransition* transition);
			void Update();
			void DebugDraw();
		protected:
			State* activeState;
			std::vector<State*> allStates;
			TransitionContainer allTransitions;
		};
	}
}