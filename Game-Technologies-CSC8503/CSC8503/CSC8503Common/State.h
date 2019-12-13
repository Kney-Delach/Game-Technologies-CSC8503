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

#include "../CSC8503Common/Debug.h"

namespace NCL
{
	namespace CSC8503
	{
		typedef void(*StateFunction)(void*);

		class State
		{
		public:
			State(const std::string& n = "State") : name(n) {}
			virtual ~State() = default;
			virtual void Update() {}
			virtual void DebugDraw() {}
			virtual std::string GetName() { return name; }
		protected:
			std::string name;
		};

		// The following class simply runs a single function 
		class GenericState : public State
		{
		public:
			GenericState(StateFunction someFunc, void* someData, const std::string& n = "Generic State", const std::string& desc = "")
				: State(n), description(desc)
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
			virtual void DebugDraw() override
			{
				const std::string msg = "Current FSM State: " + name;
				Debug::Print(msg, Vector2(5, 600), Vector4(0, 0, 0, 1));

				const std::string desc = "State Description: " + description;
				Debug::Print(desc, Vector2(5, 550), Vector4(0, 0, 0, 1));
			}
		protected:
			StateFunction func;
			void* funcData;
			const std::string description;
		};
	}
}