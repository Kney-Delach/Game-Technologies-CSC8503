/***************************************************************************
* Filename		: SphereVolume.h
* Name			: Ori Lazar
* Date			: 31/10/2019
* Description	: This class represents a sphere volume.
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
#include "CollisionVolume.h"
//#include "Debug.h"

namespace NCL
{
	class SphereVolume : CollisionVolume
	{
	public:
		SphereVolume(float sphereRadius = 1.0f)
		{
			type	= VolumeType::Sphere;
			radius	= sphereRadius;
		}
		~SphereVolume() {}

		float GetRadius() const
		{
			return radius;
		}
		virtual void DrawDebug(const NCL::Maths::Vector3& position, const NCL::Maths::Vector4& colour = NCL::Maths::Vector4(0, 1, 0, 1)) override;

	protected:
		float	radius; // radius of the sphere
	};
}