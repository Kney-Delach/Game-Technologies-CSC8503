#include "BoundingSphere.h"

using namespace NCL;

BoundingSphere::BoundingSphere(float newRadius)
{
	type	= BoundingType::Sphere;
	radius	= newRadius;
}

BoundingSphere::~BoundingSphere()
{
}
