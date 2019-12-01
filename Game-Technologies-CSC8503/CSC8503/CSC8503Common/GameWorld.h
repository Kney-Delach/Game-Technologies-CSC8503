#pragma once
#include <vector>
#include "Ray.h"
#include "CollisionDetection.h"
#include "QuadTree.h"
#include "Layer.h"

namespace NCL
{
		class Camera;
		using Maths::Ray;
	namespace CSC8503 {
		class GameObject;
		class Constraint;

		typedef std::function<void(GameObject*)> GameObjectFunc;
		typedef std::vector<GameObject*>::const_iterator GameObjectIterator;

		class GameWorld	{
		public:
			GameWorld();
			~GameWorld();

			void Clear();
			void ClearAndErase();

			void AddGameObject(GameObject* o);
			void RemoveGameObject(GameObject* o);

			void AddConstraint(Constraint* c);
			void RemoveConstraint(Constraint* c);

			Camera* GetMainCamera() const {
				return mainCamera;
			}

			void ShuffleConstraints(bool state) {
				shuffleConstraints = state;
			}

			void ShuffleObjects(bool state) {
				shuffleObjects = state;
			}

			bool Raycast(Ray& r, RayCollision& closestCollision, bool closestObject = false) const;

			virtual void UpdateWorld(float dt);

			void OperateOnContents(GameObjectFunc f);

			void GetObjectIterators(
				GameObjectIterator& first,
				GameObjectIterator& last) const;

			void GetConstraintIterators(
				std::vector<Constraint*>::const_iterator& first,
				std::vector<Constraint*>::const_iterator& last) const;

			unsigned UpdateWorldLayers(unsigned layerIndex, const std::vector<Layer>& newConfig); // parameter is preffered layer index, if unable to set as that, returns the actual layer index set
			std::vector<std::vector<Layer>>& GetWorldLayers() { return worldLayers; }
			const std::vector<std::vector<Layer>>& GetConstWorldLayers() const { return worldLayers; }
			const std::vector<Layer>& GetConstLayer(unsigned layerIndex) const { return worldLayers[layerIndex]; } //todo: add error handling mechanism
		protected:
			void UpdateTransforms();
			void UpdateQuadTree();
			
			std::vector<GameObject*> gameObjects;

			std::vector<Constraint*> constraints;

			QuadTree<GameObject*>* quadTree;

			Camera* mainCamera;

			bool shuffleConstraints;
			bool shuffleObjects;

			// 28.11.19 - world collision layer references
			// Acts as a dynamically sized matrix of layers (vector ID positions, i.e layer 0 is index 0 and layer 1 is at index 1,
			// the second degree vector holds the list of layers that each specific layer can interact with
			std::vector<std::vector<Layer>> worldLayers; //todo: create a function to append layers to a layer configuration , check it isn't repeated

			// 1.12.19
			////////////////////////////////////////////////////////////////////
			//// Debug functions ///////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////
		public:
			void DrawBoundingVolumes() const;
		};
	}
}

