/***************************************************************************
* Filename		: CollisionVolume.h
* Name			: Ori Lazar
* Date			: 27/11/2019
* Description	: Used to determine what type of collision a derived collision class is.
    .---.
  .'_:___".
  |__ --==|
  [  ]  :[|
  |__| I=[|
  / / ____|
 |-/.____.'
/___\ /___\
***************************************************************************/
#pragma once
#include "../../Common/Vector4.h"

namespace NCL
{
	// volume type used for bitwise operations in collision detection (as power of two)
	enum class VolumeType
	{
		AABB	= 1,
		OBB		= 2,
		Sphere	= 4, 
		Mesh	= 8,
		Compound= 16,
		Invalid = 256
	};

	class CollisionVolume
	{
	public:
		CollisionVolume() : type(VolumeType::Invalid) {}
		virtual ~CollisionVolume() = default;
		VolumeType type;
		virtual void DrawDebug(const NCL::Maths::Vector3& position, const NCL::Maths::Vector4& colour = NCL::Maths::Vector4(0, 1, 0, 1)) {}

	};
}