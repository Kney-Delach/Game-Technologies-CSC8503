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
#include "../CSC8503Common/NetworkBase.h"

namespace NCL 
{
	namespace CSC8503 
	{
		class NavigationTable;
		class NavigationGrid;
		class BasicAIObject;
		class ComplexAIObject;
		class PlayerIsland;
		
		class GooseGame
		{
		public:
			GooseGame();
			~GooseGame();

			virtual StringPacket* UpdateGame(float dt);

		protected:
			void InitialiseAssets();
			void InitCamera();
			void InitWorld();

			// 8.12.2019
			// used to load a world from a file 
			void LoadWorldFromFile(const std::string& filePath = "SinglePlayerWorld.goose");

			//todo: remove the following function
			void AddJumpPadToWorld(const Vector3& position, const Vector3& dimensions);

			void BridgeConstraintTest();
			void SimpleGJKTest();

			// third person camera stuff
			void TPPlayerUpdate(float dt);
			void TPCameraUpdate();
			
			GameObject* AddFloorToWorld(const Vector3& position, const int collisionType, const Vector3& dimensions = Vector3(100, 2, 100), const Vector4& colour = Vector4(1, 1, 1, 1), float stiffness = 0.8f, const std::string& name = "Ground");
			PlayerIsland* AddPlayerIslandToWorld(const Vector3& position, const int collisionType, const Vector3& dimensions = Vector3(100,2,100), const Vector4& colour = Vector4(1,1,1,1), float stiffness = 0.8f, int playerIndex = 0.f);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, bool isHollow, float inverseMass = 10.f);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, bool isAABB = true, float inverseMass = 10.f, const Vector4& color = Vector4(1,1,1,1));
			GameObject* AddStaticCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.f, const std::string& name = "Cube", float elasticity = 0.01f, float stiffness = 8.f);
			GameObject* AddStaticOBBCubeToWorld(const Vector3& position, const Vector3& scale, const Vector3& rotation, const Vector4& color);
			void AddMultiDirectionalGate(const Vector3& startPosition, const Vector3& dimensions, const Vector4& color, int numberOfLinks, int nodeSize);
			
			// 6.12.2019 - adds a player to the world 
			PlayerObject* AddGooseToWorld(const Vector3& position);
			// 6.12.2019 - adds a collectable objects to the world
			GameObject* AddAppleToWorld(const Vector3& position);
			GameObject* AddCornToWorld(const Vector3& position);
			GameObject* AddHatToWorld(const Vector3& position);

			GameObject* AddParkKeeperToWorld(const Vector3& position, NavigationGrid* navGrid, NavigationTable* navTable);
			ComplexAIObject* AddComplexKeeperToWorld(const Vector3& position, NavigationGrid* navGrid, NavigationTable* navTable, float awakeTime);
			
			GameTechRenderer* renderer;
			PhysicsSystem* physics;
			GameWorld* world;

			bool useGravity;
			bool inSelectionMode;

			float forceMagnitude;

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

			//Coursework Additional functionality	PlayerMovement
			GameObject* lockedObject	= nullptr;
			Vector3 lockedOffset		= Vector3(0, 5, -15.f);
			void LockCameraToObject(GameObject* o) { lockedObject = o; }
		protected:
			// 12.12.2019
			// game completion related data
			float gameTimer;
			int appleCollectableCount;
			int cornCollectableCount;
			int hatCollectableCount;

			// player movement usage
			bool playerControlMode;

			////////////////////////////////////////
			//// DEBUG MODE DATA 
			////////////////////////////////////////
			bool displayBoundingVolumes;
			Vector4 previousSelectedColor;
			void UpdateDebugKeys();
			void SelectObject();
		public:
			int GameStatusUpdate(float dt);
			float VictoryScreenUpdate(float dt, int gameResult);
			int GetScore();
		protected:
			// 8.12.2019
			// load from file specific world data
			int nodeSize;
			int gridWidth;
			int gridHeight;

			// muiltiplayer stuff
			int thisPlayerIndex;

			// networking stuff
			StringPacket playerNetworkPacket;
		public:
			void SetThisPlayerIndex(int index = 0) { thisPlayerIndex = index; }
		protected:
			std::vector<BasicAIObject*> farmerCollection;
			std::vector<ComplexAIObject*> keeperCollection;
			std::vector<PlayerObject*> playerCollection;
			std::vector<PlayerIsland*> islandCollection;
		};
	}
}