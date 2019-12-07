/***************************************************************************
* Filename		: StateTransition.h
* Name			: Ori Lazar
* Date			: 07/12/2019
* Description	: State transition representation declarations.
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
		class State;

		class StateTransition
		{
		public:
			virtual bool CanTransition() const = 0;

			State* GetDestinationState() const { return destinationState; }
			State* GetSourceState() const { return sourceState; }
		protected:
			State* sourceState = nullptr;
			State* destinationState = nullptr;
		};

		//////////////////////////////////////////////
		//// Generic state transition template ///////
		//////////////////////////////////////////////
		template <class T, class U>
		class GenericTransition : public StateTransition
		{
		public:
			typedef bool(*GenericTransitionFunc)(T, U);
			GenericTransition(GenericTransitionFunc function, T testData, U otherData, State* srcState, State* destState)
				: dataA(testData), dataB(otherData)
			{
				func = function;
				sourceState = srcState;
				destinationState = destState;
			}
			virtual ~GenericTransition() = default;
			virtual bool CanTransition() const override
			{
				if (func) 
				{
					return func(dataA, dataB);
				}
				return false;
			}
		public: // note, the following don't have to be used, utilize lambdas here.
			static bool GreaterThanTransition(T dataA, U dataB) { return dataA > dataB; }
			static bool LessThanTransition(T dataA, U dataB) { return dataA < dataB; }
			static bool EqualsTransition(T dataA, U dataB) { return dataA == dataB; }
			static bool NotEqualsTransition(T dataA, U dataB) { return dataA != dataB; }
		protected:
			GenericTransitionFunc func;
			T dataA;
			U dataB;
		};
	}
}