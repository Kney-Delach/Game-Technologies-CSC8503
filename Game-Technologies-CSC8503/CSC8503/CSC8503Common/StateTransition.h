#pragma once

namespace NCL {
	namespace CSC8503 {

		class State;

		class StateTransition
		{
		public:
			virtual bool CanTransition() const = 0;

			State* GetDestinationState()  const {
				return destinationState;
			}

			State* GetSourceState() const {
				return sourceState;
			}

		protected:
			State * sourceState;
			State * destinationState;
		};

		template <class T, class U>
		class GenericTransition : public StateTransition
		{
		public:
			typedef bool(*GenericTransitionFunc)(T, U);
			GenericTransition(GenericTransitionFunc f, T testData, U otherData, State* srcState, State* destState) :
				dataA(testData), dataB(otherData)
			{
				func				= f;
				sourceState			= srcState;		//
				destinationState	= destState;
			}
			~GenericTransition() {}

			virtual bool CanTransition() const override{
				if (func) {
					return func(dataA, dataB);
				}
				return false;
			}
			static bool GreaterThanTransition(T dataA, U dataB) {
				return dataA > dataB;
			}

			static bool LessThanTransition(T dataA, U dataB) {
				return dataA < dataB;
			}

			static bool EqualsTransition(T dataA, U dataB) {
				return dataA == dataB;
			}

			static bool NotEqualsTransition(T dataA, U dataB) {
				return dataA != dataB;
			}

		protected:
			GenericTransitionFunc  func;
			T dataA;
			U dataB;
		};
	}
}

