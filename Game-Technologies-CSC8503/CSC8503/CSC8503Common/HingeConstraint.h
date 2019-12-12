#pragma once
#include "Constraint.h"

namespace NCL
{
	namespace CSC8503
	{
		class GameObject;

		class HingeConstraint : public Constraint
		{
		public:
			HingeConstraint(GameObject* a, GameObject* b);
			~HingeConstraint();

			void UpdateConstraint(float dt) override;

		protected:
			GameObject* objectA;
			GameObject* objectB;
		};
	}
}