#pragma once
#include "Constraint.h"

namespace NCL
{
	namespace CSC8503
	{
		class GameObject;

		class HeightConstraint : public Constraint
		{
		public:
			HeightConstraint(GameObject* a, float h);
			~HeightConstraint();

			void UpdateConstraint(float dt) override;

		protected:
			GameObject* objectA;
			float height;
		};
	}
}