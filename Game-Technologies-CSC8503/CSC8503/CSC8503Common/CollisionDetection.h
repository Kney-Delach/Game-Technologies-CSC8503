/***************************************************************************
* Filename		: CollisionDetection.h
* Name			: Ori Lazar
* Date			: 29/11/2019
* Description	: This class contains the declarations for the collision detection
*                 mechanisms in this project.
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

#include "../../Common/Camera.h"
#include "../../Common/Plane.h"

#include "Transform.h"
#include "GameObject.h"
#include "AABBVolume.h"
#include "OBBVolume.h"
#include "SphereVolume.h"

#include "Ray.h"

using NCL::Camera;
using namespace NCL::Maths;
using namespace NCL::CSC8503;

namespace NCL
{
	class CollisionDetection
	{
	public:
		// 29.11.2019
		struct ContactPoint
		{
			Vector3 localA; // location of where collision occured
			Vector3 localB; // location of collision in object frame
			Vector3 normal; // normal vector of the collision
			float penetration; // length of penetration 
		};
		// 29.11.2019
		struct CollisionInfo
		{
			GameObject* a;
			GameObject* b;
			mutable int	framesLeft;
			ContactPoint point;

			void AddContactPoint(const Vector3& localA, const Vector3& localB, const Vector3& normal, float p)
			{
				point.localA = localA;
				point.localB = localB;
				point.normal = normal;
				point.penetration = p;
			}

			// 8.12.2019
			// This gets used as a quick hashing function to identify unique pairs of colliding objects in lists.
			bool operator<(const CollisionInfo& other) const
			{
				size_t otherHash = (size_t)other.a + ((size_t)other.b << 8);
				size_t thisHash  = (size_t)a + ((size_t)b << 8);
				return (thisHash < otherHash);
			}

			bool operator ==(const CollisionInfo& other) const
			{
				if (other.a == a && other.b == b) {
					return true;
				}
				return false;
			}
		};

		// 27.11.2019
		// raycasting functions 
		static Ray BuildRayFromMouse(const Camera& c);
		static bool RayIntersection(const Ray&r, GameObject& object, RayCollision &collisions);
		static bool RayBoxIntersection(const Ray&r, const Vector3& boxPos, const Vector3& boxSize, RayCollision& collision);
		static bool RayAABBIntersection(const Ray&r, const Transform& worldTransform, const AABBVolume&	volume, RayCollision& collision);
		static bool RayOBBIntersection(const Ray&r, const Transform& worldTransform, const OBBVolume&	volume, RayCollision& collision);
		static bool RaySphereIntersection(const Ray&r, const Transform& worldTransform, const SphereVolume& volume, RayCollision& collision);

		// 29.11.2019
		// collision detection functions
		static bool ObjectIntersection(GameObject* a, GameObject* b, CollisionInfo& collisionInfo);
		static bool SphereIntersection(const SphereVolume& volumeA, const Transform& worldTransformA, const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo);
		static bool AABBSphereIntersection(const AABBVolume& volumeA, const Transform& worldTransformA, const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo);
		static bool	AABBTest(const Vector3& posA, const Vector3& posB, const Vector3& halfSizeA, const Vector3& halfSizeB);
		static bool AABBIntersection(const AABBVolume& volumeA, const Transform& worldTransformA, const AABBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo);

		// 12.12.2010
		// OBB & Sphere collisions
		static bool OBBSphereIntersection(const OBBVolume& volumeA, const Transform& worldTransformA, const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo);
		
		// tbd...
		static bool RayPlaneIntersection(const Ray&r, const Plane&p, RayCollision& collisions);
		static bool OBBIntersection(const OBBVolume& volumeA, const Transform& worldTransformA, const OBBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo);

		// Helper functions to project screen positions to world positions (used by raycasting!)
		static Vector3 Unproject(const Vector3& screenPos, const Camera& cam);

		static Vector3 UnprojectScreenPosition(Vector3 position, float aspect, float fov, const Camera &c);
		static Matrix4 GenerateInverseProjection(float aspect, float fov, float nearPlane, float farPlane);
		static Matrix4 GenerateInverseView(const Camera &c);
	private:
		CollisionDetection() = default;
		~CollisionDetection() = default;
	};
}