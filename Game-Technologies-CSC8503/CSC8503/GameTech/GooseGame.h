/***************************************************************************
* Filename		: GooseGame.h
* Name			: Ori Lazar
* Date			: 28/11/2019
* Description	: Central point for running the goose game.
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
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include "../CSC8503Common/PlayerObject.h"


namespace NCL 
{
	namespace CSC8503 
	{
		class NavigationTable;
		class NavigationGrid;
		class BasicAIObject;
		class PlayerIsland;
		
		class GooseGame
		{
		public:
			GooseGame();
			~GooseGame();

			virtual void UpdateGame(float dt);

		protected:
			void InitialiseAssets();
			void InitCamera();
			void UpdateKeys();
			void InitWorld();

			// 8.12.2019
			// used to load a world from a file 
			void LoadWorldFromFile(const std::string& filePath = "SinglePlayerWorld.goose");

			//todo: remove the following function
			void AddJumpPadToWorld(const Vector3& position, const Vector3& dimensions);

			/*
			These are some of the world/object creation functions I created when testing the functionality
			in the module. Feel free to mess around with them to see different objects being created in different
			test scenarios (constraints, collision types, and so on). 
			*/
			//void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			//void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void BridgeConstraintTest();
			void SimpleGJKTest();

			bool SelectObject();
			void MoveSelectedObject();
			void DebugObjectMovement();

			// 8.12.2019 
			// player movement
			void PlayerMovement(); // used to move the player goose around
			void PlayerCameraMovement();

			void GameObjectMovement();
			
			GameObject* AddFloorToWorld(const Vector3& position, const int collisionType, const Vector3& dimensions = Vector3(100, 2, 100), const Vector4& colour = Vector4(1, 1, 1, 1), float stiffness = 0.8f);
			PlayerIsland* AddPlayerIslandToWorld(const Vector3& position, const int collisionType, const Vector3& dimensions = Vector3(100,2,100), const Vector4& colour = Vector4(1,1,1,1), float stiffness = 0.8f, int playerIndex = 0.f);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, bool isHollow, float inverseMass = 10.f);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, bool isAABB = true, float inverseMass = 10.f);
			GameObject* AddStaticCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.f, bool isWall = false, float elasticity = 0.01f, float stiffness = 8.f);
			GameObject* AddStaticOBBCubeToWorld(const Vector3& position, const Vector3& scale, const Vector3& rotation, const Vector4& color);
			
			// 6.12.2019 - adds a player to the world 
			PlayerObject* AddGooseToWorld(const Vector3& position);
			// 6.12.2019 - adds a collectable objects to the world
			GameObject* AddAppleToWorld(const Vector3& position);
			GameObject* AddCornToWorld(const Vector3& position);
			GameObject* AddHatToWorld(const Vector3& position);

			GameObject* AddParkKeeperToWorld(const Vector3& position, NavigationGrid* navGrid, NavigationTable* navTable);
			GameObject* AddCharacterToWorld(const Vector3& position);
			
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
			Vector3 lockedOffset		= Vector3(0, 5, 20);
			void LockCameraToObject(GameObject* o) { lockedObject = o; }

			// 1.12.19
			////////////////////////////////////////////////////////////////////
			//// Debug functionality a ///////////////////////////////////
			////////////////////////////////////////////////////////////////////
			bool displayBoundingVolumes;

			// 6.12.2019
			// player controllers (currently only 1 but make this 2 in the multiplayer version)
			//PlayerObject* playerGameObject = nullptr;

		protected:
			// 8.12.2019
			// load from file specific world data
			int nodeSize;
			int gridWidth;
			int gridHeight;

			// 9.12.2019
			// pathfinding related variables
			//BasicAIObject* farmerAIObject = nullptr;
			std::vector<BasicAIObject*> farmerCollection;
			std::vector<PlayerObject*> playerCollection;
			std::vector<PlayerIsland*> islandCollection;
		};
	}
}
