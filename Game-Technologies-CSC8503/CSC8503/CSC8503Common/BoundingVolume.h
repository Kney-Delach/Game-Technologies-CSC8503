#pragma once
#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"

using namespace NCL::Maths;

namespace NCL {
	enum class BoundingType {
		AABB,
		OOBB,
		Sphere,
		Mesh
	};

	class BoundingVolume
	{
	public:
		BoundingVolume();
		~BoundingVolume();

		virtual void DrawDebug(const Vector3& position, const Vector4& colour = Vector4(0, 1, 0, 1)) {} //todo: implement this in all children classes

		BoundingType type;
	};
}

