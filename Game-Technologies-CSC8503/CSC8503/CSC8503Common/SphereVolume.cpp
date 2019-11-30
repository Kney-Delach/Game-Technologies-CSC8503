#include "SphereVolume.h"
#include "Debug.h"

void NCL::SphereVolume::DrawDebug(const Vector3& position, const Vector4& colour)
{
	Debug::DrawCircle(position, radius, colour);
}