#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"


namespace NCL {
	namespace CSC8503 {
		class TutorialGame		{
		public:
			TutorialGame();
			~TutorialGame();

			virtual void UpdateGame(float dt);

		protected:
			void InitialiseAssets();
			void InitCamera();
			void UpdateKeys();
			void InitWorld();

			// 3.12.19
			// goose level world initialization
			void InitGooseGameWorld();
			void InitGroundLevelTerrain();
			void InitBoundaries();
			
			/*
			These are some of the world/object creation functions I created when testing the functionality
			in the module. Feel free to mess around with them to see different objects being created in different
			test scenarios (constraints, collision types, and so on). 
			*/
			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);
			void BridgeConstraintTest();
			void SimpleGJKTest();

			bool SelectObject();
			void MoveSelectedObject();
			void DebugObjectMovement();
			void LockedObjectMovement();
			void LockedCameraMovement();

			void GameObjectMovement();
			
			GameObject* AddFloorToWorld(const Vector3& position, const Vector3& dimensions = Vector3(100,2,100), const Vector4& colour = Vector4(1,1,1,1), bool resolveAsSprings = false);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, bool isHollow, float inverseMass = 10.f);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, bool isAABB = true, float inverseMass = 10.f);
			
			//IT'S HAPPENING
			GameObject* AddGooseToWorld(const Vector3& position);
			GameObject* AddParkKeeperToWorld(const Vector3& position);
			GameObject* AddCharacterToWorld(const Vector3& position);
			GameObject* AddAppleToWorld(const Vector3& position);

			GameTechRenderer*	renderer;
			PhysicsSystem*		physics;
			GameWorld*			world;

			bool useGravity;
			bool inSelectionMode;

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;
			GameObject* selectionObjectFront = nullptr;
			GameObject* SelectionObjectBack = nullptr;

			OGLMesh*	cubeMesh	= nullptr;
			OGLMesh*	sphereMesh	= nullptr;
			OGLTexture* basicTex	= nullptr;
			OGLShader*	basicShader = nullptr;

			//Coursework Meshes
			OGLMesh*	gooseMesh	= nullptr;
			OGLMesh*	keeperMesh	= nullptr;
			OGLMesh*	appleMesh	= nullptr;
			OGLMesh*	charA		= nullptr;
			OGLMesh*	charB		= nullptr;

			//Coursework Additional functionality	
			GameObject* lockedObject	= nullptr;
			Vector3 lockedOffset		= Vector3(0, 14, 20);
			void LockCameraToObject(GameObject* o) { lockedObject = o; }

			// 1.12.19
			////////////////////////////////////////////////////////////////////
			//// Debug functionality a ///////////////////////////////////
			////////////////////////////////////////////////////////////////////
			bool displayBoundingVolumes;
		};
	}
}