#include "BoundingAABB.h"

using namespace NCL;

BoundingAABB::BoundingAABB(const Vector3& halfDims)
{
	type = BoundingType::AABB;

	this->halfSizes = halfDims;
}


BoundingAABB::~BoundingAABB()
{
}
