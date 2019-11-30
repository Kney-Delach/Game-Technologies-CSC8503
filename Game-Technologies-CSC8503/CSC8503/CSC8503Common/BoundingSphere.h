#pragma once
#include "BoundingVolume.h"

namespace NCL
{
	class BoundingSphere : BoundingVolume
	{
	public:
		BoundingSphere(float radius = 1.0f);
		~BoundingSphere();

		float GetRadius() const { return radius; }

		virtual void DrawDebug(const Vector3& position, const Vector4& colour = Vector4(0,1,0,1)) override;

		Vector3 origin;
		float	radius;
	};
}

