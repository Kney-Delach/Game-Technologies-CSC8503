#include "AABBVolume.h"
#include "Debug.h"

namespace NCL
{
	void AABBVolume::DrawDebug(const NCL::Maths::Vector3& position, const NCL::Maths::Vector4& colour)
	{
		// draw the center of the square
		Debug::DrawCircle(position, 0.1f, colour);

		// draw the cube bounding lines
		// z
		Debug::DrawLine
		(
			Vector3(position.x + halfSizes.x, position.y + halfSizes.y, position.z - halfSizes.z),
			Vector3(position.x + halfSizes.x, position.y + halfSizes.y, position.z + halfSizes.z),
			colour
		);

		Debug::DrawLine
		(
			Vector3(position.x + halfSizes.x, position.y - halfSizes.y, position.z - halfSizes.z),
			Vector3(position.x + halfSizes.x, position.y - halfSizes.y, position.z + halfSizes.z),
			colour
		);

		Debug::DrawLine
		(
			Vector3(position.x - halfSizes.x, position.y - halfSizes.y, position.z - halfSizes.z),
			Vector3(position.x - halfSizes.x, position.y - halfSizes.y, position.z + halfSizes.z),
			colour
		);

		Debug::DrawLine
		(
			Vector3(position.x - halfSizes.x, position.y + halfSizes.y, position.z - halfSizes.z),
			Vector3(position.x - halfSizes.x, position.y + halfSizes.y, position.z + halfSizes.z),
			colour
		);

		// y
		Debug::DrawLine
		(
			Vector3(position.x + halfSizes.x, position.y - halfSizes.y, position.z + halfSizes.z),
			Vector3(position.x + halfSizes.x, position.y + halfSizes.y, position.z + halfSizes.z),
			colour
		);
		
		Debug::DrawLine
		(
			Vector3(position.x + halfSizes.x, position.y - halfSizes.y, position.z - halfSizes.z),
			Vector3(position.x + halfSizes.x, position.y + halfSizes.y, position.z - halfSizes.z),
			colour
		);

		Debug::DrawLine
		(
			Vector3(position.x - halfSizes.x, position.y - halfSizes.y, position.z - halfSizes.z),
			Vector3(position.x - halfSizes.x, position.y + halfSizes.y, position.z - halfSizes.z),
			colour
		);
		Debug::DrawLine
		(
			Vector3(position.x - halfSizes.x, position.y - halfSizes.y, position.z + halfSizes.z),
			Vector3(position.x - halfSizes.x, position.y + halfSizes.y, position.z + halfSizes.z),
			colour
		);

		// x
		Debug::DrawLine(
			Vector3(position.x - halfSizes.x, position.y + halfSizes.y, position.z + halfSizes.z),
			Vector3(position.x + halfSizes.x, position.y + halfSizes.y, position.z + halfSizes.z),
			colour
		);

		Debug::DrawLine
		(
			Vector3(position.x - halfSizes.x, position.y - halfSizes.y, position.z + halfSizes.z),
			Vector3(position.x + halfSizes.x, position.y - halfSizes.y, position.z + halfSizes.z),
			colour
		);

		Debug::DrawLine(
			Vector3(position.x - halfSizes.x, position.y - halfSizes.y, position.z - halfSizes.z),
			Vector3(position.x + halfSizes.x, position.y - halfSizes.y, position.z - halfSizes.z),
			colour
		);

		Debug::DrawLine
		(
			Vector3(position.x - halfSizes.x, position.y + halfSizes.y, position.z - halfSizes.z),
			Vector3(position.x + halfSizes.x, position.y + halfSizes.y, position.z - halfSizes.z),
			colour
		);
	}
}