#pragma once
#include "Constraint.h"

namespace NCL {
	namespace CSC8503 {
		class GameObject;

		class PositionConstraint : public Constraint	{
		public:
			PositionConstraint(GameObject* a, GameObject* b, float d);
			~PositionConstraint();

			void UpdateConstraint(float dt) override;

		protected:
			GameObject* objectA;
			GameObject* objectB;

			float distance;
		};
	}
}