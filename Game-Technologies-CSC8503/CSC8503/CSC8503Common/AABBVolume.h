#pragma once
#include "CollisionVolume.h"
#include "../../Common/Vector3.h"
namespace NCL {
	class AABBVolume : CollisionVolume
	{
	public:
		AABBVolume(const Maths::Vector3& halfDims)
		{
			type		= VolumeType::AABB;
			halfSizes	= halfDims;
		}
		~AABBVolume() = default;

		Maths::Vector3 GetHalfDimensions() const { return halfSizes; }		
		virtual void DrawDebug(const NCL::Maths::Vector3& position, const NCL::Maths::Vector4& colour = NCL::Maths::Vector4(1, 0, 0, 1)) override;
	protected:
		Maths::Vector3 halfSizes;
	};
}