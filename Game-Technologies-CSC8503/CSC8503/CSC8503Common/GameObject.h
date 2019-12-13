/***************************************************************************
* Filename		: GameObject.h
* Name			: Ori Lazar
* Date			: 28/11/2019
* Description	: Contains the declarations of the gameobjects.
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
#include "Transform.h"
#include "CollisionVolume.h"

#include "PhysicsObject.h"
#include "RenderObject.h"
#include "NetworkObject.h"

#include "Layer.h"
#include "Ray.h"

#include <vector>
#include "SphereVolume.h"

//todo: add a static id counter for each object

using std::vector;

namespace NCL
{
	namespace CSC8503
	{
		class NetworkObject;

		class GameObject
		{
		public:
			GameObject(string name = "");
			virtual ~GameObject();

			void SetBoundingVolume(CollisionVolume* vol)
			{
				boundingVolume = vol;
			}

			const CollisionVolume* GetBoundingVolume() const
			{
				return boundingVolume;
			}

			bool IsActive() const
			{
				return isActive;
			}

			void SetActive(bool val)
			{
				isActive = val;
			}

			const Transform& GetConstTransform() const
			{
				return transform;
			}

			Transform& GetTransform()
			{
				return transform;
			}

			// 28.11.2019
			const Layer& GetConstLayer() const
			{
				return layer;
			}

			// 28.11.19
			Layer& GetLayer()
			{
				return layer;
			}

			Ray BuildRayFromDirection(const Vector3& direction) const
			{
				return Ray(transform.GetWorldPosition(), transform.GetWorldOrientation() * direction); 
			}

			Ray BuildRayFromDirectionOffset(const Vector3& offset, const Vector3& direction) const
			{
				return Ray(transform.GetWorldPosition() + offset, transform.GetWorldOrientation() * direction);
			}

			RenderObject* GetRenderObject() const
			{
				return renderObject;
			}

			PhysicsObject* GetPhysicsObject() const
			{
				return physicsObject;
			}

			NetworkObject* GetNetworkObject() const 
			{
				return networkObject;
			}

			void SetRenderObject(RenderObject* newObject)
		{
				renderObject = newObject;
			}

			void SetPhysicsObject(PhysicsObject* newObject)
			{
				physicsObject = newObject;
			}

			const string& GetName() const
			{
				return name;
			}

			virtual void OnCollisionBegin(GameObject* otherObject)
			{
				//std::cout << "OnCollisionBegin event occured!\n";
			}

			virtual void OnCollisionEnd(GameObject* otherObject)
			{
				//std::cout << "OnCollisionEnd event occured!\n";
			}

			bool GetBroadphaseAABB(Vector3&outsize) const;

			void UpdateBroadphaseAABB();

			void UpdateObjectPosition(float dt, const Vector3& parentPosition, const unsigned int index);
			void UpdateIslandStoredObjectPosition(const Vector3& parentPosition, const unsigned row, const unsigned index);

			// draw debug information about a gameobject
			virtual void DebugDraw();

			// 30.11.2019
			void DrawDebugVolume();

		public:
			// 28.11.2019 
			static void DrawLineBetweenObjects(const GameObject* from, const GameObject* to);
			static void DrawLineInDirection(const GameObject* from, const Vector3& direction, const Vector3& offset = Vector3(0.f,0.f,0.f));
			static void DrawLineBetweenObjectsOffset(const Vector3& offset, const GameObject* from, const GameObject* to);
		protected:
			Transform transform;
			Layer layer; // 28.11.2019 - collision layer references
			
			CollisionVolume* boundingVolume;
			PhysicsObject* physicsObject;
			RenderObject* renderObject;
			NetworkObject* networkObject;

			bool isActive;
			string name;

			Vector3 broadphaseAABB; // 8.12.2019
		};
	}
}

