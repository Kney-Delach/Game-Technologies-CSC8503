#pragma once
#include "BoundingVolume.h"

namespace NCL {
	class BoundingAABB : BoundingVolume
	{
	public:
		BoundingAABB(const Vector3& halfDims);
		~BoundingAABB();

		Vector3 GetHalfDimensions() const {
			return halfSizes;
		}

	protected:
		Vector3 halfSizes;
	};
}
