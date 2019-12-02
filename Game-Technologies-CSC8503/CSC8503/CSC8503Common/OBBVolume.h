#pragma once
#include "CollisionVolume.h"
#include "../../Common/Vector3.h"
namespace NCL
{
	namespace Maths { class Quaternion; }

	class OBBVolume : CollisionVolume
	{
	public:
		OBBVolume(const Maths::Vector3& halfDims)
		{
			type		= VolumeType::OBB;
			halfSizes	= halfDims;
		}
		~OBBVolume() {}

		Maths::Vector3 GetHalfDimensions() const
		{
			return halfSizes;
		}

		void DrawDebugOBB(const NCL::Maths::Vector3& position, const NCL::Maths::Quaternion& orientation ,const NCL::Maths::Vector4& colour = NCL::Maths::Vector4(0, 0, 1, 1));

	protected:
		Maths::Vector3 halfSizes;
	};
}