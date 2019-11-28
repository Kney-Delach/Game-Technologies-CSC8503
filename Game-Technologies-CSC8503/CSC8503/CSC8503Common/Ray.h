/***************************************************************************
* Filename		: Ray.h
* Name			: Ori Lazar
* Date			: 31/10/2019
* Description	: This class represents a ray
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
#include "../../Common/Vector3.h"
#include "../../Common/Plane.h"

namespace NCL
{
	namespace Maths
	{
		//todo: turn this into a template
		//template<typename T>
		struct RayCollision
		{
			void*		node;			//Node that was hit
			Vector3		collidedAt;		//WORLD SPACE position of the collision!
			float		rayDistance;

			RayCollision(void* node, Vector3 collidedAt)

			{
				this->node			= node;
				this->collidedAt	= collidedAt;
				this->rayDistance	= 0.0f;
			}

			RayCollision()
			{
				node			= nullptr;
				rayDistance		= FLT_MAX;
			}
		};

		class Ray
		{
		public:
			Ray(Vector3 position, Vector3 direction)
			{
				this->position  = position;
				this->direction = direction;
			}
			~Ray() {}

			Vector3 GetPosition() const {return position;}

			Vector3 GetDirection() const {return direction;}

		protected:
			Vector3 position;	// World space position
			Vector3 direction;	// Normalised world space direction
		};
	}
}