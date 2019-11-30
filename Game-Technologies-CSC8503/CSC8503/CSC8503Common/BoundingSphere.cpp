#include "BoundingSphere.h"

#include "Debug.h"

using namespace NCL;

BoundingSphere::BoundingSphere(float newRadius)
{
	type	= BoundingType::Sphere;
	radius	= newRadius;
}

BoundingSphere::~BoundingSphere()
{
}

void BoundingSphere::DrawDebug(const Vector3& position, const Vector4& colour)
{
	Debug::DrawCircle(position, radius, colour);
}
