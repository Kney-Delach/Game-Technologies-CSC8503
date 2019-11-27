#pragma once
#include "BoundingVolume.h"

namespace NCL {
	class BoundingSphere : BoundingVolume
	{
	public:
		BoundingSphere(float radius = 1.0f);
		~BoundingSphere();

		float GetRadius() const {
			return radius;
		}

		Vector3 origin;
		float	radius;
	};
}

