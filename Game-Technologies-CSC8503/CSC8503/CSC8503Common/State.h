/***************************************************************************
* Filename		: State.h
* Name			: Ori Lazar
* Date			: 07/12/2019
* Description	: Representation of a state for state machine usages.
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

namespace NCL
{
	namespace CSC8503
	{
		typedef void(*StateFunction)(void*);

		class State
		{
		public:
			State() = default;
			virtual ~State() = default;
			virtual void Update() = 0;
		};

		// The following class simply runs a single function 
		class GenericState : public State
		{
		public:
			GenericState(StateFunction someFunc, void* someData)
			{
				func		= someFunc;
				funcData	= someData;
			}			
			virtual void Update() override
			{
				if (funcData != nullptr) 
				{
					func(funcData);
				}
			}
		protected:
			StateFunction func;
			void* funcData;
		};
	}
}