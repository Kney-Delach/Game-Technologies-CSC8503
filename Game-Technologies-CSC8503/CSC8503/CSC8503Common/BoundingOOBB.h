#pragma once
#include "BoundingVolume.h"

namespace NCL {
	class BoundingOOBB : BoundingVolume
	{
	public:
		BoundingOOBB();
		~BoundingOOBB();

		Vector3 origin;
		Vector3 halfSizes;
	};
}

