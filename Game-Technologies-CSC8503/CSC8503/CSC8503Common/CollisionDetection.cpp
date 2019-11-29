/***************************************************************************
* Filename		: CollisionDetection.cpp
* Name			: Ori Lazar
* Date			: 31/10/2019
* Description	: This class performs the calculations to check whether collision
*                 has occured between entities.
    .---.
  .'_:___".
  |__ --==|
  [  ]  :[|
  |__| I=[|
  / / ____|
 |-/.____.'
/___\ /___\
***************************************************************************/
#include "CollisionDetection.h"
#include "CollisionVolume.h"
#include "AABBVolume.h"
#include "OBBVolume.h"
#include "SphereVolume.h"
#include "../../Common/Vector2.h"
#include "../../Common/Window.h"
#include "../../Common/Maths.h"

#include <list>

#include "../CSC8503Common/Simplex.h"

#include "Debug.h"

using namespace NCL;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 27/11/19 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// returns the correct sub-type ray intersection function call
bool CollisionDetection::RayIntersection(const Ray& r, GameObject& object, RayCollision& collision)
{	
	const Transform& transform = object.GetConstTransform();
	const CollisionVolume* volume = object.GetBoundingVolume();

	if (!volume) // object must have a volume to be intersectable
		return false;

	switch (volume->type)
	{
		case VolumeType::AABB:		return RayAABBIntersection(r, transform, (const AABBVolume&)* volume, collision);
		case VolumeType::OBB:		return RayOBBIntersection(r, transform, (const OBBVolume&)* volume, collision);
		case VolumeType::Sphere:	return RaySphereIntersection(r, transform, (const SphereVolume&)* volume, collision);
		default:					return false;
	}}

// Uses reduced plane case that only checks the 3 closest planes rather than all 6 planes that make up the box.
// check direction of ray, if going left we check only right side of box, if up check bottom, if going forward, check back side of box.
// note: box is axis-aligned.
bool CollisionDetection::RayBoxIntersection(const Ray& r, const Vector3& boxPos, const Vector3& boxSize, RayCollision& collision)
{
	Vector3 boxMin = boxPos - boxSize;
	Vector3 boxMax = boxPos + boxSize;
	Vector3 rayPos = r.GetPosition();
	Vector3 rayDir = r.GetDirection();

	Vector3 tVals(-1, -1, -1);

	for (int i = 0; i < 3; ++i)  // get best 3 intersections (axis aligned os checks each axis per loop)
	{
		if (rayDir[i] > 0)
			tVals[i] = (boxMin[i] - rayPos[i]) / rayDir[i];
		else if (rayDir[i] < 0)
			tVals[i] = (boxMax[i] - rayPos[i]) / rayDir[i];
	}
	
	 float bestT = tVals.GetMaxElement();
	
	 if (bestT < 0.0f)  
		return false; // tVals vector iniitalized with negative values, if reached here then intersection is behind the ray and should be ignored. (NO BACKAWRDS RAYS)

	Vector3 intersection = rayPos + (rayDir * bestT);

	const float epsilon = 0.0001f; // accomodates for variations in floating point accuracy

	for (int i = 0; i < 3; ++i) // (axis aligned os checks each axis per loop)
	{
		if (intersection[i] + epsilon < boxMin[i] || intersection[i] - epsilon > boxMax[i])  // calculates whether or not intersection point is too far to one side of box
			return false; 
	}
	collision.collidedAt = intersection;
	collision.rayDistance = bestT;
	return true;
}

// pass through function to RayBoxIntersection function above, as is axis aligned
bool CollisionDetection::RayAABBIntersection(const Ray&r, const Transform& worldTransform, const AABBVolume& volume, RayCollision& collision)
{
	Vector3 boxPos = worldTransform.GetWorldPosition();
	Vector3 boxSize = volume.GetHalfDimensions();
	return RayBoxIntersection(r, boxPos, boxSize, collision);
}

// 1. transofrm ray os that it is in local space of box
//      - to bring ray into local space of box, subtract box's position and transform newly formed relative position by the inverse of the box's orientation, formed
//        using conjugate of its orientation quaternion, along with the ray's direction, this returns the temporary ray defined within the frame of reference of the OBB (AABB at its own origin, i.e, the box sits at its own origin position).
// 2. if colliding, transform collision point back into world space.
//      - performing opposite operations on the collision point (add position back on, and then rotate by world transform)
bool CollisionDetection::RayOBBIntersection(const Ray&r, const Transform& worldTransform, const OBBVolume& volume, RayCollision& collision)
{
	Quaternion orientation = worldTransform.GetWorldOrientation();
	Vector3 position = worldTransform.GetWorldPosition();
	
	Matrix3 transform = Matrix3(orientation);
	Matrix3 invTransform = Matrix3(orientation.Conjugate());
	
	Vector3 localRayPos = r.GetPosition() - position;
	
	Ray tempRay(invTransform * localRayPos, invTransform * r.GetDirection());
	
	bool collided = RayBoxIntersection(tempRay, Vector3(),volume.GetHalfDimensions(), collision); // raycasting using axis aligned function above. (box sits at its own origin position)
	if (collided) 
		collision.collidedAt = transform * collision.collidedAt + position;
	return collided;
}

// ray intersection with spheres
bool CollisionDetection::RaySphereIntersection(const Ray&r, const Transform& worldTransform, const SphereVolume& volume, RayCollision& collision)
{
	Vector3 spherePos = worldTransform.GetWorldPosition();				// world position of sphere
	float sphereRadius = volume.GetRadius();							// radius of sphere
	
	Vector3 dir = (spherePos - r.GetPosition());						// Get the direction between the ray origin and the sphere origin
	float sphereProj = Vector3::Dot(dir, r.GetDirection());			// Project the sphere ’s origin onto the ray direction vector
	Vector3 point = r.GetPosition() + (r.GetDirection() * sphereProj);	// Get closest point on ray line to sphere 
	
	float sphereDist = (point - spherePos).Length();					// Distance from the point to the sphere
	
	if (sphereDist > sphereRadius) // out of sphere radius (NOT COLLIDING)
		return false;

	// determine collision point (COLLIDING)
	// move collision point back along the direction vector so that it touches the surface of the sphere, rather than being inside of it.
	float sNorm = sphereDist / sphereRadius;
	sNorm = cos(DegreesToRadians(sNorm * 90.0f));
	
	collision.rayDistance = sphereProj - sphereRadius * sNorm;
	collision.collidedAt = r.GetPosition() + r.GetDirection() * collision.rayDistance;
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 29/11/19 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// determines which collision detection function to use between two gameobjects.
//todo: implement this in an optimized manner -> virtual functions, dispatch functions, function pointers....
bool CollisionDetection::ObjectIntersection(GameObject* a, GameObject* b, CollisionInfo& collisionInfo)
{
	const CollisionVolume * volA = a->GetBoundingVolume();
	const CollisionVolume * volB = b->GetBoundingVolume();
	
	if (!volA || !volB) // collidable shapes must have a bounding volume
		return false;	
	
	collisionInfo.a = a;
	collisionInfo.b = b;
	
	const Transform & transformA = a->GetConstTransform();
	const Transform & transformB = b->GetConstTransform();

	// perform bitwise OR operator to determine ordering
	const VolumeType pairType = (VolumeType)((int)volA->type | (int)volB->type);

	// objects of the same type
	if (pairType == VolumeType::AABB) 
		return AABBIntersection((AABBVolume&)*volA, transformA,(AABBVolume&)*volB, transformB, collisionInfo);
		
	if (pairType == VolumeType::Sphere) 
		return SphereIntersection((SphereVolume&)*volA, transformA,(SphereVolume&)*volB, transformB, collisionInfo);
	// objects not the same type (can perform bit shifting above to differentiate between these cases)
	if (volA->type == VolumeType::AABB && volB->type == VolumeType::Sphere)
		return AABBSphereIntersection((AABBVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
	
	if (volA->type == VolumeType::Sphere && volB->type == VolumeType::AABB) 
	{
		// flip collision info ordering as AABBSphereIntersection expects AABB volume object first.
		collisionInfo.a = b;
		collisionInfo.b = a;
		return AABBSphereIntersection((AABBVolume&)*volB, transformB, (SphereVolume&)*volA, transformA, collisionInfo);
	}
	return false;
}

// Sphere - Sphere collision
bool CollisionDetection::SphereIntersection(const SphereVolume& volumeA, const Transform& worldTransformA, const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo)
{
	const float radii = volumeA.GetRadius() + volumeB.GetRadius(); // total radius
	const Vector3 delta = worldTransformB.GetWorldPosition() - worldTransformA.GetWorldPosition(); // position A - position B
	const float deltaLength = delta.Length(); // length of distance
	
	if (deltaLength < radii)  // if length of distance between spheres is smaller than total radius, then spheres are colliding
	{
		const float penetration = radii - deltaLength;
		const Vector3 normal = delta.Normalised(); // normalized vector between the two spheres
		const Vector3 localA = normal * volumeA.GetRadius();
		const Vector3 localB = -normal * volumeB.GetRadius();
		
		collisionInfo.AddContactPoint(localA, localB, normal, penetration);
		return true;
	}	
	return false;
}

// AABB - Sphere Collision
bool CollisionDetection::AABBSphereIntersection(const AABBVolume& volumeA, const Transform& worldTransformA, const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo)
{
	// find closest point on the box to the sphere's location
	Vector3 boxSize = volumeA.GetHalfDimensions();
	Vector3 delta = worldTransformB.GetWorldPosition() - worldTransformA.GetWorldPosition();
	Vector3 closestPointOnBox = Maths::Clamp(delta, -boxSize, boxSize); // clamp relative position of the sphere, to limit it to the box's size.

	// determine how far away the sphere is from the closest point on the box (subtract this point from the sphere's relative position)

	// if point lies at a distance less than the sphere's radius, then objects are colliding.

	// Collision point for sphere -> will lie radius units backwards along the collision normal,
	
	return false;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TBD... ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CollisionDetection::AABBTest(const Vector3& posA, const Vector3& posB, const Vector3& halfSizeA, const Vector3& halfSizeB)
{
	return false;
}

//AABB/AABB Collisions
bool CollisionDetection::AABBIntersection(const AABBVolume& volumeA, const Transform& worldTransformA,
	const AABBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo)
{
	return false;
}

bool CollisionDetection::OBBIntersection(
	const OBBVolume& volumeA, const Transform& worldTransformA,
	const OBBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo)
{
	return false;
}

//It's helper functions for generating rays from here on out:

Matrix4 GenerateInverseView(const Camera &c)
{
	float pitch = c.GetPitch();
	float yaw	= c.GetYaw();
	Vector3 position = c.GetPosition();

	Matrix4 iview =
		Matrix4::Translation(position) *
		Matrix4::Rotation(-yaw, Vector3(0, -1, 0)) *
		Matrix4::Rotation(-pitch, Vector3(-1, 0, 0));

	return iview;
}

Matrix4 GenerateInverseProjection(float aspect, float nearPlane, float farPlane, float fov)
{
	float negDepth = nearPlane - farPlane;

	float invNegDepth = negDepth / (2 * (farPlane * nearPlane));

	Matrix4 m;

	float h = 1.0f / tan(fov*PI_OVER_360);

	m.array[0] = aspect / h;
	m.array[5] = tan(fov*PI_OVER_360);
	m.array[10] = 0.0f;

	m.array[11] = invNegDepth;//// +PI_OVER_360;
	m.array[14] = -1.0f;
	m.array[15] = (0.5f / nearPlane) + (0.5f / farPlane);

	//Matrix4 temp = projection.Inverse();
	//return temp;
	return m;
}

Vector3 CollisionDetection::Unproject(const Vector3& screenPos, const Camera& cam)
{
	Vector2 screenSize = Window::GetWindow()->GetScreenSize();

	float aspect	= screenSize.x / screenSize.y;
	float fov		= cam.GetFieldOfVision();
	float nearPlane = cam.GetNearPlane();
	float farPlane  = cam.GetFarPlane();

	//Create our inverted matrix! Note how that to get a correct inverse matrix,
	//the order of matrices used to form it are inverted, too.
	Matrix4 invVP = GenerateInverseView(cam) * GenerateInverseProjection(aspect, fov, nearPlane, farPlane);

	Matrix4 test1 = GenerateInverseView(cam);
	Matrix4 test2 = cam.BuildViewMatrix().Inverse();

	Matrix4 proj  = cam.BuildProjectionMatrix(aspect);
	Matrix4 test4 = cam.BuildProjectionMatrix(aspect).Inverse();
	Matrix4 test3 = GenerateInverseProjection(aspect, fov, nearPlane, farPlane);

	//Our mouse position x and y values are in 0 to screen dimensions range,
	//so we need to turn them into the -1 to 1 axis range of clip space.
	//We can do that by dividing the mouse values by the width and height of the
	//screen (giving us a range of 0.0 to 1.0), multiplying by 2 (0.0 to 2.0)
	//and then subtracting 1 (-1.0 to 1.0).
	Vector4 clipSpace = Vector4(
		(screenPos.x / (float)screenSize.x) * 2.0f - 1.0f,
		(screenPos.y / (float)screenSize.y) * 2.0f - 1.0f,
		(screenPos.z),
		1.0f
	);

	//Then, we multiply our clipspace coordinate by our inverted matrix
	Vector4 transformed = invVP * clipSpace;

	//our transformed w coordinate is now the 'inverse' perspective divide, so
	//we can reconstruct the final world space by dividing x,y,and z by w.
	return Vector3(transformed.x / transformed.w, transformed.y / transformed.w, transformed.z / transformed.w);
}

Ray CollisionDetection::BuildRayFromMouse(const Camera& cam)
{
	Vector2 screenMouse = Window::GetMouse()->GetAbsolutePosition();
	Vector2 screenSize	= Window::GetWindow()->GetScreenSize();

	//We remove the y axis mouse position from height as OpenGL is 'upside down',
	//and thinks the bottom left is the origin, instead of the top left!
	Vector3 nearPos = Vector3(screenMouse.x,
		screenSize.y - screenMouse.y,
		-0.99999f
	);

	//We also don't use exactly 1.0 (the normalised 'end' of the far plane) as this
	//causes the unproject function to go a bit weird. 
	Vector3 farPos = Vector3(screenMouse.x,
		screenSize.y - screenMouse.y,
		0.99999f
	);

	Vector3 a = Unproject(nearPos, cam);
	Vector3 b = Unproject(farPos, cam);
	Vector3 c = b - a;

	c.Normalise();

	//std::cout << "Ray Direction:" << c << std::endl;

	return Ray(cam.GetPosition(), c);
}

//http://bookofhook.com/mousepick.pdf
Matrix4 CollisionDetection::GenerateInverseProjection(float aspect, float fov, float nearPlane, float farPlane)
{
	Matrix4 m;

	float t = tan(fov*PI_OVER_360);

	float neg_depth = nearPlane - farPlane;

	const float h = 1.0f / t;

	float c = (farPlane + nearPlane) / neg_depth;
	float e = -1.0f;
	float d = 2.0f*(nearPlane*farPlane) / neg_depth;

	m.array[0]  = aspect / h;
	m.array[5]  = tan(fov*PI_OVER_360);

	m.array[10] = 0.0f;
	m.array[11] = 1.0f / d;

	m.array[14] = 1.0f / e;

	m.array[15] = -c / (d*e);

	return m;
}

/*
And here's how we generate an inverse view matrix. It's pretty much
an exact inversion of the BuildViewMatrix function of the Camera class!
*/
Matrix4 CollisionDetection::GenerateInverseView(const Camera &c)
{
	float pitch = c.GetPitch();
	float yaw	= c.GetYaw();
	Vector3 position = c.GetPosition();

	Matrix4 iview =
	Matrix4::Translation(position) *
	Matrix4::Rotation(yaw, Vector3(0, 1, 0)) *
	Matrix4::Rotation(pitch, Vector3(1, 0, 0));

	return iview;
}


/*
If you've read through the Deferred Rendering tutorial you should have a pretty
good idea what this function does. It takes a 2D position, such as the mouse
position, and 'unprojects' it, to generate a 3D world space position for it.

Just as we turn a world space position into a clip space position by multiplying
it by the model, view, and projection matrices, we can turn a clip space
position back to a 3D position by multiply it by the INVERSE of the
view projection matrix (the model matrix has already been assumed to have
'transformed' the 2D point). As has been mentioned a few times, inverting a
matrix is not a nice operation, either to understand or code. But! We can cheat
the inversion process again, just like we do when we create a view matrix using
the camera.

So, to form the inverted matrix, we need the aspect and fov used to create the
projection matrix of our scene, and the camera used to form the view matrix.

*/
Vector3	CollisionDetection::UnprojectScreenPosition(Vector3 position, float aspect, float fov, const Camera &c)
{
	//Create our inverted matrix! Note how that to get a correct inverse matrix,
	//the order of matrices used to form it are inverted, too.
	Matrix4 invVP = GenerateInverseView(c) * GenerateInverseProjection(aspect, fov, c.GetNearPlane(), c.GetFarPlane());

	Vector2 screenSize = Window::GetWindow()->GetScreenSize();

	//Our mouse position x and y values are in 0 to screen dimensions range,
	//so we need to turn them into the -1 to 1 axis range of clip space.
	//We can do that by dividing the mouse values by the width and height of the
	//screen (giving us a range of 0.0 to 1.0), multiplying by 2 (0.0 to 2.0)
	//and then subtracting 1 (-1.0 to 1.0).
	Vector4 clipSpace = Vector4(
		(position.x / (float)screenSize.x) * 2.0f - 1.0f,
		(position.y / (float)screenSize.y) * 2.0f - 1.0f,
		(position.z) - 1.0f,
		1.0f
	);

	//Then, we multiply our clipspace coordinate by our inverted matrix
	Vector4 transformed = invVP * clipSpace;

	//our transformed w coordinate is now the 'inverse' perspective divide, so
	//we can reconstruct the final world space by dividing x,y,and z by w.
	return Vector3(transformed.x / transformed.w, transformed.y / transformed.w, transformed.z / transformed.w);
}

bool CollisionDetection::RayPlaneIntersection(const Ray& r, const Plane& p, RayCollision& collisions)
{
	return false;
}