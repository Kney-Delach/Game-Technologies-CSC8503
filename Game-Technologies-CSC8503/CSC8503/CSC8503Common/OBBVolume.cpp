#include "OBBVolume.h"
#include "Debug.h"
#include "../../../Common/Quaternion.h"

#define OBB_VERTEX_POSITION(pos, orientation, vertex)  (orientation * (vertex - position)) + position

namespace NCL
{
	void OBBVolume::DrawDebugOBB(const NCL::Maths::Vector3& position, const NCL::Maths::Quaternion& orientation, const NCL::Maths::Vector4& colour)
	{
		// draw the center of the square
		Debug::DrawCircle(position, 0.1f, colour);

		/////////////////////////////////////
		// Draw the cube's bounding lines ///
		/////////////////////////////////////
		Vector3 vertexPos0 = OBB_VERTEX_POSITION(position, orientation, Vector3(position.x + halfSizes.x, position.y + halfSizes.y, position.z - halfSizes.z));
		Vector3 vertexPos1 = OBB_VERTEX_POSITION(position, orientation, Vector3(position.x + halfSizes.x, position.y + halfSizes.y, position.z + halfSizes.z));	
		Debug::DrawLine(vertexPos0, vertexPos1, colour);
		
		vertexPos0 = OBB_VERTEX_POSITION(position, orientation, Vector3(position.x + halfSizes.x, position.y - halfSizes.y, position.z - halfSizes.z));
		vertexPos1 = OBB_VERTEX_POSITION(position, orientation, Vector3(position.x + halfSizes.x, position.y - halfSizes.y, position.z + halfSizes.z));
		Debug::DrawLine(vertexPos0, vertexPos1, colour);

		vertexPos0 = OBB_VERTEX_POSITION(position, orientation, Vector3(position.x - halfSizes.x, position.y - halfSizes.y, position.z - halfSizes.z));
		vertexPos1 = OBB_VERTEX_POSITION(position, orientation, Vector3(position.x - halfSizes.x, position.y - halfSizes.y, position.z + halfSizes.z));
		Debug::DrawLine(vertexPos0, vertexPos1, colour);

		vertexPos0 = OBB_VERTEX_POSITION(position, orientation, Vector3(position.x - halfSizes.x, position.y + halfSizes.y, position.z - halfSizes.z));
		vertexPos1 = OBB_VERTEX_POSITION(position, orientation, Vector3(position.x - halfSizes.x, position.y + halfSizes.y, position.z + halfSizes.z));
		Debug::DrawLine(vertexPos0, vertexPos1, colour);

		vertexPos0 = OBB_VERTEX_POSITION(position, orientation, Vector3(position.x + halfSizes.x, position.y - halfSizes.y, position.z + halfSizes.z));
		vertexPos1 = OBB_VERTEX_POSITION(position, orientation, Vector3(position.x + halfSizes.x, position.y + halfSizes.y, position.z + halfSizes.z));
		Debug::DrawLine(vertexPos0, vertexPos1, colour);

		vertexPos0 = OBB_VERTEX_POSITION(position, orientation, Vector3(position.x + halfSizes.x, position.y - halfSizes.y, position.z - halfSizes.z));
		vertexPos1 = OBB_VERTEX_POSITION(position, orientation, Vector3(position.x + halfSizes.x, position.y + halfSizes.y, position.z - halfSizes.z));
		Debug::DrawLine(vertexPos0, vertexPos1, colour);

		vertexPos0 = OBB_VERTEX_POSITION(position, orientation, Vector3(position.x - halfSizes.x, position.y - halfSizes.y, position.z - halfSizes.z));
		vertexPos1 = OBB_VERTEX_POSITION(position, orientation, Vector3(position.x - halfSizes.x, position.y + halfSizes.y, position.z - halfSizes.z));
		Debug::DrawLine(vertexPos0, vertexPos1, colour);

		vertexPos0 = OBB_VERTEX_POSITION(position, orientation, Vector3(position.x - halfSizes.x, position.y - halfSizes.y, position.z + halfSizes.z));
		vertexPos1 = OBB_VERTEX_POSITION(position, orientation, Vector3(position.x - halfSizes.x, position.y + halfSizes.y, position.z + halfSizes.z));
		Debug::DrawLine(vertexPos0, vertexPos1, colour);

		vertexPos0 = OBB_VERTEX_POSITION(position, orientation, Vector3(position.x - halfSizes.x, position.y + halfSizes.y, position.z + halfSizes.z));
		vertexPos1 = OBB_VERTEX_POSITION(position, orientation, Vector3(position.x + halfSizes.x, position.y + halfSizes.y, position.z + halfSizes.z));
		Debug::DrawLine(vertexPos0, vertexPos1, colour);

		vertexPos0 = OBB_VERTEX_POSITION(position, orientation, Vector3(position.x - halfSizes.x, position.y - halfSizes.y, position.z + halfSizes.z));
		vertexPos1 = OBB_VERTEX_POSITION(position, orientation, Vector3(position.x + halfSizes.x, position.y - halfSizes.y, position.z + halfSizes.z));
		Debug::DrawLine(vertexPos0, vertexPos1, colour);

		vertexPos0 = OBB_VERTEX_POSITION(position, orientation, Vector3(position.x - halfSizes.x, position.y - halfSizes.y, position.z - halfSizes.z));
		vertexPos1 = OBB_VERTEX_POSITION(position, orientation, Vector3(position.x + halfSizes.x, position.y - halfSizes.y, position.z - halfSizes.z));
		Debug::DrawLine(vertexPos0, vertexPos1, colour);

		vertexPos0 = OBB_VERTEX_POSITION(position, orientation, Vector3(position.x - halfSizes.x, position.y + halfSizes.y, position.z - halfSizes.z));
		vertexPos1 = OBB_VERTEX_POSITION(position, orientation, Vector3(position.x + halfSizes.x, position.y + halfSizes.y, position.z - halfSizes.z));
		Debug::DrawLine(vertexPos0, vertexPos1, colour);
	}
}