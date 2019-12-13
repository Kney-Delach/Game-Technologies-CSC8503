/***************************************************************************
* Filename		: GooseGame.cpp
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
#include "GooseGame.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"

#include "../CSC8503Common/PositionConstraint.h"
#include "../CSC8503Common/CollectableObject.h"
#include "../CSC8503Common/PlayerIsland.h"
#include "../../Common/Assets.h"
#include "../CSC8503Common/NavigationGrid.h"

#include <fstream>
#include "../CSC8503Common/NavigationTable.h"
#include "../CSC8503Common/BasicAIObject.h"
#include "../CSC8503Common/ComplexAIObject.h"

#include "../CSC8503Common/FileManager.h"
#include "../CSC8503Common/HeightConstraint.h"
#include "../CSC8503Common/HingeConstraint.h"

#include <algorithm>
#include "../CSC8503Common/FloatToString.h"

using namespace NCL;
using namespace CSC8503;

void GooseGame::LoadWorldFromFile(const std::string& filePath)
{
	SetThisPlayerIndex(0); //todo: move this to multiplayer stuff
	playerControlMode = true;
	gameTimer = 180.f;
	appleCollectableCount = 0;
	cornCollectableCount = 0;
	hatCollectableCount = 0;
	
	if (filePath.empty())
	{
		std::cout << "Cannot load game world with empty path\n";
		return;
	}
	
	// load file into an input stream
	std::ifstream infile(Assets::DATADIR + filePath);

	// load in level data
	infile >> nodeSize;
	infile >> gridWidth;
	infile >> gridHeight;

	// add human players
	int numberOfHumanPlayers = 0;
	infile >> numberOfHumanPlayers;
	std::cout << "Number of human players: " << numberOfHumanPlayers << "\n";
	for (int i = 0; i < numberOfHumanPlayers; ++i)
	{
		float posX, posY, posZ;

		infile >> posX;
		infile >> posY;
		infile >> posZ;
		std::cout << posX << posY << posZ << "\n";
		playerCollection.push_back(AddGooseToWorld(Vector3(posX * (float) nodeSize, posY, posZ * (float)nodeSize)));
	}
	
	const Vector3 cubeDims = Vector3(nodeSize / 2, nodeSize / 2, nodeSize / 2);
	Vector3 position;

	// add the world data from the file to a vector
	vector<int> terrainMap;
	char entity;
	for (int z = 0; z < gridHeight; ++z) 
	{
		for (int x = 0; x < gridWidth; ++x) 
		{
			infile >> entity;
			terrainMap.push_back(entity);
		}
	}
	vector<int> objectMap;
	for (int z = 0; z < gridHeight; ++z)
	{
		for (int x = 0; x < gridWidth; ++x)
		{
			infile >> entity;
			objectMap.push_back(entity);
		}
	}
	
	int playerIslandCount = 0;
	for (int z = 0; z < gridHeight; ++z) //todo: add trampoline addition
	{
		for (int x = 0; x < gridWidth; ++x)
		{
			if (terrainMap[x + z * gridWidth] == '.')
			{
				continue;
			}
			if (terrainMap[x + z * gridWidth] == 'x')
			{
				position = Vector3(x * 2 * cubeDims.x, 3.f, z * 2 * cubeDims.z);
				AddStaticCubeToWorld(position, Vector3(((float)nodeSize) / 2.f, 5, ((float)nodeSize) / 2.f), 0.f, "Wall", 1.f, 10000.f);
			}
			if (terrainMap[x + z * gridWidth] == 'm')
			{
				position = Vector3(x * 2 * cubeDims.x, 6.f, z * 2 * cubeDims.z);
				AddStaticCubeToWorld(position, Vector3(((float)nodeSize) / 2.f, 10, ((float)nodeSize) / 2.f), 0.f, "Wall", 1.f, 10000.f);
			}
			if (terrainMap[x + z * gridWidth] == 'l') //todo: move this to encompass entire world (MAKE SURE ONLY ONE OF THESE CAN BE ADDED)
			{
				GameObject* cube = AddStaticCubeToWorld(Vector3(static_cast<float>(gridWidth) * nodeSize , -4.f, static_cast<float>(gridHeight) * nodeSize - nodeSize) / 2.f, Vector3(static_cast<float>(gridWidth) * nodeSize, 1.f, static_cast<float>(gridHeight) * nodeSize) / 2.f, 0.f, "Ground");
				cube->GetRenderObject()->SetColour(Vector4(0.f, 0.5f, 0.5f, 0.f));
			}
			if(terrainMap[x + z * gridWidth] == 'w')
			{
				position = Vector3(x * 2 * cubeDims.x, 0.f, z * 2 * cubeDims.z);
				AddFloorToWorld(position, ObjectCollisionType::SPRING, Vector3(((float)nodeSize) / 2.f, 2.f, ((float)nodeSize) / 2.f), Vector4(0.f, 0.f, 1.f, 0.f), 300.f, "Water");
			}
			if (terrainMap[x + z * gridWidth] == 'q') // water ramp
			{
				position = Vector3(x * 2 * cubeDims.x, -0.25f, z * 2 * cubeDims.z-0.1f);
				AddStaticOBBCubeToWorld(position,Vector3(((float)nodeSize) / 2.f, 0.25f, ((float)nodeSize) / 2.f), Vector3(20, 0, 0),Vector4(0.f, 0.f, 1.f, 0.f));
			}
			if (terrainMap[x + z * gridWidth] == 'p') // water ramp
			{
				position = Vector3(x * 2 * cubeDims.x, -0.25f, z * 2 * cubeDims.z + 0.1f);
				AddStaticOBBCubeToWorld(position, Vector3(((float)nodeSize) / 2.f, 0.25f, ((float)nodeSize) / 2.f), Vector3(-20, 0, 0), Vector4(0.f, 0.f, 1.f, 0.f));
			}
			if (terrainMap[x + z * gridWidth] == 'g') // constrained multi directional gate
			{
				position = Vector3(x * 2 * cubeDims.x, 3.f, z * 2 * cubeDims.z);
				AddMultiDirectionalGate(position, Vector3(cubeDims.x, 5.f, cubeDims.z), Vector4(0.f, 0.f, 1.f, 0.f), 1, nodeSize);
			}
			if (terrainMap[x + z * gridWidth] == 's')
			{
				if (playerIslandCount < (int)playerCollection.size())
				{
					position = Vector3(-cubeDims.x + (x * 2 * cubeDims.x), 2.f, -cubeDims.z + (z * 2 * cubeDims.z));
					islandCollection.push_back(AddPlayerIslandToWorld(position, ObjectCollisionType::IMPULSE, Vector3(((float)nodeSize), 1.f, ((float)nodeSize)), Vector4(0, 1, 1, 1), 0.8f, playerIslandCount++));
				}
			}
		}
	}
	int aiCount = 0;
	int smartAICount = 0;
	bool newPathfindingData = false;
	bool newPathfindingDataComplex = false;
	for (int j = 0; j < gridHeight; ++j)
	{
		for (int k = 0; k < gridWidth; ++k)
		{
			if (objectMap[k + j * gridWidth] == '.' || objectMap[k + j * gridWidth] == 'x')
			{
				continue;
			}
			if (objectMap[k + j * gridWidth] == 'a')
			{
				position = Vector3(k * 2 * cubeDims.x, 0.25f, j * 2 * cubeDims.z);
				AddAppleToWorld(position);
				appleCollectableCount++;
				continue;
			}

			if (objectMap[k + j * gridWidth] == 'c')
			{
				position = Vector3(k * 2 * cubeDims.x, 0.25f, j * 2 * cubeDims.z);
				AddCornToWorld(position);
				cornCollectableCount++;
				continue;
			}

			if (objectMap[k + j * gridWidth] == 'h')
			{
				position = Vector3(k * 2 * cubeDims.x, 0.25f, j * 2 * cubeDims.z);
				AddHatToWorld(position);
				hatCollectableCount++;
				continue;
			}

			if (objectMap[k + j * gridWidth] == 'r')
			{
				position = Vector3(k * 2 * cubeDims.x, 2.f, j * 2 * cubeDims.z);
				const std::string gridFile = Assets::DATADIR + "NavigationGridSP.Pathfinding";
				NavigationGrid* dumbAiNavGrid = new NavigationGrid(gridFile);
				NavigationTable* navTable;
				const std::string navTableFile = Assets::DATADIR + "NavigationTableSP.Pathfinding"; 
				if(newPathfindingData)
				{
					navTable = new NavigationTable((int)(dumbAiNavGrid->GetWidth() * dumbAiNavGrid->GetHeight()), dumbAiNavGrid, false);
					FileManager<NavigationTable>::Writer(navTableFile, *navTable);
				}
				else
				{
					navTable = new NavigationTable((int)(dumbAiNavGrid->GetWidth() * dumbAiNavGrid->GetHeight()), dumbAiNavGrid, true);
					FileManager<NavigationTable>::Loader(navTableFile, *navTable);
				}
				aiCount++;
				newPathfindingData = false;
				BasicAIObject* farmer = (BasicAIObject*)AddParkKeeperToWorld(position, dumbAiNavGrid, navTable);
				farmer->SetPlayerObjectCollection(&playerCollection);
				farmerCollection.push_back(farmer);
			}
			if (objectMap[k + j * gridWidth] == 'k')
			{
				position = Vector3(k * 2 * cubeDims.x, 30.f, j * 2 * cubeDims.z);
				const std::string gridFile = Assets::DATADIR + "NavigationGridSPKeeper.Pathfinding";
				NavigationGrid* smartAINavGrid = new NavigationGrid(gridFile);
				NavigationTable* navTable;
				const std::string navTableFile = Assets::DATADIR + "NavigationTableSPKeeper.Pathfinding";
				if (newPathfindingDataComplex)
				{
					navTable = new NavigationTable((int)(smartAINavGrid->GetWidth() * smartAINavGrid->GetHeight()), smartAINavGrid, false);
					FileManager<NavigationTable>::Writer(navTableFile, *navTable);
				}
				else
				{
					navTable = new NavigationTable((int)(smartAINavGrid->GetWidth() * smartAINavGrid->GetHeight()), smartAINavGrid, true);
					FileManager<NavigationTable>::Loader(navTableFile, *navTable);
				}
				newPathfindingData = false;
				ComplexAIObject* parkKeeper = (ComplexAIObject*)AddComplexKeeperToWorld(position, smartAINavGrid, navTable, 180.f - (30.f * (float)smartAICount));
				parkKeeper->SetPlayerIslandCollection(&islandCollection);
				smartAICount++;

				keeperCollection.push_back(parkKeeper);
			}
		}
	}

	for (PlayerIsland* p : islandCollection)
	{
		p->SetMaxCollectables(appleCollectableCount, cornCollectableCount, hatCollectableCount);
	}
}

#pragma region INITIALIZATION

GooseGame::GooseGame()
{
	world		= new GameWorld();
	renderer	= new GameTechRenderer(*world);
	renderer->SetAsActiveContext();

	physics		= new PhysicsSystem(*world);

	gameTimer = 180.f;
	appleCollectableCount = 0;
	cornCollectableCount = 0;
	hatCollectableCount = 0;
	forceMagnitude	= 10.0f;
	useGravity		= false;
	inSelectionMode = false;

	// debug toggles
	displayBoundingVolumes = false;
	
	Debug::SetRenderer(renderer);

	InitialiseAssets();
}

// initialises all the assets used in this project.
void GooseGame::InitialiseAssets()
{
	auto loadFunc = [](const string& name, OGLMesh** into)
	{
		*into = new OGLMesh(name);
		(*into)->SetPrimitiveType(GeometryPrimitive::Triangles);
		(*into)->UploadToGPU();
	};

	loadFunc("cube.msh"		 , &cubeMesh);
	loadFunc("sphere.msh"	 , &sphereMesh);
	loadFunc("goose.msh"	 , &gooseMesh);
	loadFunc("CharacterA.msh", &keeperMesh);
	loadFunc("CharacterM.msh", &charA);
	loadFunc("CharacterF.msh", &charB);
	loadFunc("Apple.msh"	 , &appleMesh);

	basicTex	= (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");
	basicShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");

	InitCamera();
	InitWorld();
}

void GooseGame::InitCamera()
{
	world->GetMainCamera()->SetNearPlane(0.5f);
	world->GetMainCamera()->SetFarPlane(2000.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
	lockedObject = nullptr;
}

void GooseGame::InitWorld()
{
	selectionObject = nullptr;
	selectionObjectFront = nullptr;
	SelectionObjectBack = nullptr;
	lockedObject = nullptr;
	world->ClearAndErase();
	physics->Clear();
	farmerCollection.clear();
	playerCollection.clear();
	islandCollection.clear();
	keeperCollection.clear();
	LoadWorldFromFile();
}

GooseGame::~GooseGame()
{
	delete cubeMesh;
	delete sphereMesh;
	delete gooseMesh;
	delete basicTex;
	delete basicShader;
	delete physics;
	delete renderer;
	delete world;
}

#pragma endregion INITIALIZATION

#pragma region UPDATE

void GooseGame::UpdateGame(float dt)
{	
	if (playerControlMode)
	{
		TPCameraUpdate();
		TPPlayerUpdate(dt);
		Debug::Print("[R] -> Free Camera!", Vector2(5, 875), Vector4(0,0,0,1));
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::R))
		{
			playerControlMode = false;
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}
	else
	{
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::R))
		{
			playerControlMode = true;
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
			inSelectionMode = false;
			displayBoundingVolumes = false;
			if (selectionObject)
			{	
				selectionObject->GetRenderObject()->SetColour(previousSelectedColor);
				selectionObject = nullptr;
			}
		}
		
		UpdateDebugKeys();
		
		Debug::Print("[R] -> Player Control!", Vector2(5, 875), Vector4(0, 1, 1, 1));
		Debug::Print("[P] -> display physics volumes!", Vector2(5, 850), Vector4(0, 1, 0, 1));
		if(inSelectionMode)
		{
			Debug::Print("[T] -> Free Camera!", Vector2(5, 800), Vector4(1, 0, 0, 1));
			Debug::Print("Click on an object for info!", Vector2(5, 750), Vector4(1, 0, 0, 1));

			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::T))
			{
				inSelectionMode = false;
				Window::GetWindow()->ShowOSPointer(false);
				Window::GetWindow()->LockMouseToWindow(true);
				if (selectionObject)
				{
					selectionObject->GetRenderObject()->SetColour(previousSelectedColor);
					selectionObject = nullptr;
				}
			}
			SelectObject();
		}
		else
		{
			Debug::Print("[T] -> Object selection mode!", Vector2(5, 800), Vector4(1, 0, 0, 1));
			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::T))
			{
				inSelectionMode = true;
				Window::GetWindow()->ShowOSPointer(true);
				Window::GetWindow()->LockMouseToWindow(false);
			}
			world->GetMainCamera()->UpdateCamera(dt);
		}
	}

	for (int i = 0; i < islandCollection.size(); ++i)
	{
		islandCollection[i]->DrawPlayerScore();
	}
	for (unsigned i = 0; i < playerCollection.size(); i++)
	{
		playerCollection[i]->UpdateCooldown(dt);
		playerCollection[i]->UpdateInventoryTransformations(dt);
	}

	for (int i = 0; i < farmerCollection.size(); ++i)
	{
		farmerCollection[i]->Update();
	}

	for (int i = 0; i < keeperCollection.size(); ++i)
	{
		if(keeperCollection[i]->GetAwakeTime() >= gameTimer)
		{
			keeperCollection[i]->SetActive(true);
			keeperCollection[i]->Update(dt);
		}
		else
		{
			keeperCollection[i]->SetActive(false);
		}
	}
	
	world->UpdateWorld(dt);
	renderer->Update(dt);
	physics->Update(dt);

	Debug::FlushRenderables();
	
	// debug draw
	if(displayBoundingVolumes)
	{
		world->DrawBoundingVolumes();
		renderer->DebugRender();
	}
	else
	{
		renderer->Render();
	}	
}

void GooseGame::UpdateDebugKeys()
{
	///////////////////////////////////////////////////////////////////////////////////////
	//// Debug Toggle Keys ////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P)) // toggle physics volumes debug display
	{
		displayBoundingVolumes = !displayBoundingVolumes;
	}
	
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)) // resets the simulation
	{
		InitWorld(); 
		selectionObject = nullptr;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2)) // resets camera to specific location
	{
		InitCamera();
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::G)) // gravity toggle
	{
		useGravity = !useGravity;
		physics->UseGravity(useGravity);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F7)) // enable object shuffles
	{
		world->ShuffleObjects(true);
	}
	
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F8))  // disable object shuffles
	{
		world->ShuffleObjects(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9)) // enable constraint shuffle
	{
		world->ShuffleConstraints(true);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10))  // disable constraint shuffle 
	{
		world->ShuffleConstraints(false);
	}
}

//todo: do something with the scores.
int GooseGame::GameStatusUpdate(float dt) //todo: clean this up
{
	int totalApples = 0;
	for (PlayerIsland* island : islandCollection)
	{
		totalApples += island->GetApplesCount();
	}

	if(totalApples == appleCollectableCount)
	{
		gameTimer = 10.f; // set timer for victory screen

		// somebody won
		if(islandCollection.size() > 1)
		{
			islandCollection[thisPlayerIndex]->SetWinnerStatus(true);
			// check who won (compare scores)
			return 2; // 2 is winner
			return 1; // 1 is lost
		}
		else
		{
			islandCollection[thisPlayerIndex]->SetWinnerStatus(true);

			// single player victory royale!
			return 2;
		}
	}
	
	if(gameTimer <= 0)
	{
		gameTimer = 10.f; // set timer for victory screen
		// lost
		if (islandCollection.size() > 1)
		{
			// see who has the highest score
			//todo: implement these
			islandCollection[thisPlayerIndex]->SetWinnerStatus(false);
			return 1; // 1 is lost
			return 2; // 2 is winner
		}
		else
		{
			// single player WASTED
			islandCollection[thisPlayerIndex]->SetWinnerStatus(false);
			return 1; // 1 is lost
		}
	}
	gameTimer -= dt;
	static const Vector4 green(1, 0, 0, 1);
	static const Vector2 pos(5, 175);
	const std::string sc = "Time: " + FloatToString<float>(gameTimer, 2);
	Debug::Print(sc, pos, green);
	return 0; // 0 is GAME NOT OVER
}

float GooseGame::VictoryScreenUpdate(float dt, int gameResult)
{
	gameTimer -= dt;
	static const Vector4 green(1, 0, 1, 1);
	static const Vector2 pos(5, 1000);
	const std::string sc = "Back To Lobby In: " + FloatToString<float>(gameTimer, 2);
	Debug::Print(sc, pos, green);

	// game has ended, 1 is lost, 2 is won, create a timer to go to the menu in X seconds and update leaderboard files
	if (gameResult == 1)
	{
		// WASTED
		static const Vector4 green(1, 0, 0, 1);
		static const Vector2 pos(960, 540);
		Debug::Print("WASTED", pos, green);
	}
	if (gameResult == 2)
	{
		// VICTORY ROYALE
		static const Vector4 green(0, 1, 0, 1);
		static const Vector2 pos(960, 540);
		Debug::Print("VICTORY ROYALE!", pos, green);
	}

	int index = 0;
	for (PlayerIsland* island : islandCollection)
	{
		island->DrawFinalScore(index++);
	}
	
	renderer->Update(dt);
	Debug::FlushRenderables();
	renderer->Render();

	return gameTimer;
}

#pragma endregion UPDATE


#pragma region ADD_OBJECTS


//todo: fix the way collisions are resolved here
void GooseGame::AddJumpPadToWorld(const Vector3& position, const Vector3& dimensions)
{
	// the center of the pad (actually makes you jump)	
	AddFloorToWorld(position, ObjectCollisionType::JUMP_PAD, dimensions, Vector4(0.5,0.5,0.5,1), 1000.f); //todo: make this resolve collisions differently (as a jump pad)

	// left padding
	AddFloorToWorld(position - Vector3(0.f, 0.f, dimensions.z + 1.f), ObjectCollisionType::IMPULSE, Vector3(dimensions.x, dimensions.y * 2.f, 1.f), Vector4(1, 1, 1, 1), 0.8f); // green

	// right padding
	AddFloorToWorld(position + Vector3(0.f, 0.f, dimensions.z + 1.f), ObjectCollisionType::IMPULSE, Vector3(dimensions.x, dimensions.y * 2.f, 1.f), Vector4(1, 1, 1, 1), 0.8f); // green

	// front padding
	AddFloorToWorld(position - Vector3(dimensions.x + 1.f, 0.f, 0.f), ObjectCollisionType::IMPULSE, Vector3(1.f, dimensions.y * 2.f, dimensions.z + 2.f), Vector4(1, 1, 1, 1), 0.8f); // green

	// back padding
	AddFloorToWorld(position + Vector3(dimensions.x + 1.f, 0.f, 0.f), ObjectCollisionType::IMPULSE, Vector3(1.f, dimensions.y * 2.f, dimensions.z + 2.f), Vector4(1, 1, 1, 1), 0.8f); // green
}


// A single function to add a large immoveable cube to the bottom of our world
GameObject* GooseGame::AddFloorToWorld(const Vector3& position, const int collisionType, const Vector3& dimensions, const Vector4& colour, float stiffness, const std::string& name)
{
	GameObject* floor = new GameObject(name);

	AABBVolume* volume = new AABBVolume(dimensions);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform().SetWorldScale(dimensions);
	floor->GetTransform().SetWorldPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->SetElasticity(stiffness);
	floor->GetPhysicsObject()->SetStiffness(stiffness);
	floor->GetPhysicsObject()->InitCubeInertia();
	floor->GetPhysicsObject()->SetStatic(true);

	floor->GetPhysicsObject()->SetCollisionType(collisionType);
	
	floor->GetRenderObject()->SetColour(colour);
	
	floor->GetLayer().SetLayerID(0); // set layer ID to 1 (not raycastable)
	
	world->AddGameObject(floor);

	return floor;
}

PlayerIsland* GooseGame::AddPlayerIslandToWorld(const Vector3& position, const int collisionType, const Vector3& dimensions, const Vector4& colour, float stiffness, int playerIndex)
{
	PlayerIsland* island = new PlayerIsland("Player Island");

	AABBVolume* volume = new AABBVolume(dimensions);
	island->SetBoundingVolume((CollisionVolume*)volume);
	island->GetTransform().SetWorldScale(dimensions);
	island->GetTransform().SetWorldPosition(position);

	island->SetRenderObject(new RenderObject(&island->GetTransform(), cubeMesh, basicTex, basicShader));
	island->SetPhysicsObject(new PhysicsObject(&island->GetTransform(), island->GetBoundingVolume()));

	island->GetPhysicsObject()->SetInverseMass(0);
	island->GetPhysicsObject()->SetElasticity(stiffness);
	island->GetPhysicsObject()->SetStiffness(stiffness);
	island->GetPhysicsObject()->InitCubeInertia();

	island->GetPhysicsObject()->SetCollisionType(collisionType);

	island->GetRenderObject()->SetColour(colour);

	island->GetLayer().SetLayerID(1); // set layer ID to 1 (not raycastable)
	island->GetPhysicsObject()->SetStatic(true);

	island->SetParent(playerCollection[playerIndex]);
	world->AddGameObject(island);

	return island;
}

GameObject* GooseGame::AddSphereToWorld(const Vector3& position, float radius, bool isHollow, float inverseMass)
{
	GameObject* sphere = new GameObject("Sphere");

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);
	sphere->GetTransform().SetWorldScale(sphereSize);
	sphere->GetTransform().SetWorldPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	
	if(isHollow)
	{
		sphere->GetRenderObject()->SetColour(Vector4(1, 0, 1, 1));
		sphere->GetPhysicsObject()->InitSphereInertia(true); // creates a hollow sphere
		sphere->GetPhysicsObject()->SetStiffness(100.f);
	}
	else
	{
		sphere->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
		sphere->GetPhysicsObject()->InitSphereInertia(false);
		sphere->GetPhysicsObject()->SetStiffness(100.f);
	}

	sphere->GetPhysicsObject()->SetElasticity(1.f); // highly elastic material (like a rubber ball) 
	sphere->GetPhysicsObject()->SetCollisionType(ObjectCollisionType::IMPULSE | ObjectCollisionType::SPRING | ObjectCollisionType::JUMP_PAD);
	sphere->GetLayer().SetLayerID(0); // set layer ID to 1 (not raycastable)
	sphere->GetPhysicsObject()->SetStatic(true);

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* GooseGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, bool isAABB, float inverseMass, const Vector4& color)
{
	GameObject* cube = new GameObject("Constrained OBB Gate");

	if(isAABB)
	{
		AABBVolume* volume = new AABBVolume(dimensions);
		cube->SetBoundingVolume((CollisionVolume*)volume);
	}
	else
	{
		OBBVolume* volume = new OBBVolume(dimensions);
		cube->SetBoundingVolume((CollisionVolume*)volume);
	}

	cube->GetTransform().SetWorldPosition(position);
	cube->GetTransform().SetWorldScale(dimensions);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));
	cube->GetRenderObject()->SetColour(color);

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->SetElasticity(0.01f); // low elasticity material (like steel)
	cube->GetPhysicsObject()->SetStiffness(8.f);
	cube->GetPhysicsObject()->SetCollisionType(ObjectCollisionType::IMPULSE);// | ObjectCollisionType::SPRING | ObjectCollisionType::JUMP_PAD); //todo: change this 
	cube->GetPhysicsObject()->InitCubeInertia();

	cube->GetLayer().SetLayerID(0); // set layer ID to 1 (not raycastable)
	cube->GetPhysicsObject()->SetStatic(true);

	world->AddGameObject(cube);

	return cube;
}

GameObject* GooseGame::AddStaticCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass, const std::string& name, float elasticity, float stiffness)
{
	GameObject* cube = new GameObject(name);

	AABBVolume* volume = new AABBVolume(dimensions);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform().SetWorldPosition(position);
	cube->GetTransform().SetWorldScale(dimensions);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->SetElasticity(elasticity);
	cube->GetPhysicsObject()->SetStiffness(stiffness);
	if(name == "Wall")
	{
		cube->GetPhysicsObject()->SetCollisionType(ObjectCollisionType::SPRING);
	}
	else
	{
		cube->GetPhysicsObject()->SetCollisionType(ObjectCollisionType::IMPULSE);
	}
	cube->GetPhysicsObject()->InitCubeInertia();
	cube->GetPhysicsObject()->SetGravityUsage(false);
	cube->GetLayer().SetLayerID(0); // set layer ID to 1 (not raycastable)
	
	cube->GetPhysicsObject()->SetStatic(true);

	world->AddGameObject(cube);

	return cube;
}

GameObject* GooseGame::AddStaticOBBCubeToWorld(const Vector3& position, const Vector3& scale, const Vector3& rotation, const Vector4& color)
{
	GameObject* cube = new GameObject("Ramp");
	OBBVolume* volume = new OBBVolume(scale);
	cube->SetBoundingVolume((CollisionVolume*)volume);
	cube->GetTransform().SetWorldScale(scale);
	cube->GetTransform().SetWorldPosition(position);
	cube->GetTransform().SetLocalOrientation(Quaternion::EulerAnglesToQuaternion(rotation.x, rotation.y, rotation.z));
	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));
	cube->GetPhysicsObject()->SetElasticity(0.0f);
	cube->GetPhysicsObject()->SetStiffness(0.0f);
	cube->GetPhysicsObject()->SetGravityUsage(false);
	cube->GetPhysicsObject()->SetCollisionType(ObjectCollisionType::IMPULSE);
	cube->GetPhysicsObject()->SetInverseMass(0);
	cube->GetPhysicsObject()->InitCubeInertia();
	cube->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));
	cube->GetPhysicsObject()->SetStatic(true);

	world->AddGameObject(cube);
	return cube;
}

PlayerObject* GooseGame::AddGooseToWorld(const Vector3& position)
{
	float size			= 1.0f;
	float inverseMass = 1.f / 4.f;

	PlayerObject* goose = new PlayerObject("Goose");

	SphereVolume* volume = new SphereVolume(size);
	goose->SetBoundingVolume((CollisionVolume*)volume);

	goose->GetTransform().SetWorldScale(Vector3(size,size,size));
	goose->GetTransform().SetWorldPosition(position);
	goose->GetTransform().SetLocalPosition(position);

	goose->SetRenderObject(new RenderObject(&goose->GetTransform(), gooseMesh, nullptr, basicShader));
	goose->SetPhysicsObject(new PhysicsObject(&goose->GetTransform(), goose->GetBoundingVolume()));

	goose->GetPhysicsObject()->SetInverseMass(inverseMass);
	goose->GetPhysicsObject()->InitSphereInertia();
	goose->GetPhysicsObject()->SetElasticity(0.7f); // low elasticity material (like steel)
	goose->GetPhysicsObject()->SetStiffness(200.f);
	
	goose->GetPhysicsObject()->SetCollisionType(ObjectCollisionType::IMPULSE | ObjectCollisionType::SPRING | ObjectCollisionType::JUMP_PAD | ObjectCollisionType::COLLECTABLE);
	
	world->AddGameObject(goose);

	return goose;
}

GameObject* GooseGame::AddParkKeeperToWorld(const Vector3& position, NavigationGrid* navGrid, NavigationTable* navTable)
{
	float meshSize = 4.0f;
	float inverseMass = 1.f / 4.f;

	BasicAIObject* keeper = new BasicAIObject(position, 1, "Basic AI");

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.9f, 0.3f) * meshSize);
	keeper->SetBoundingVolume((CollisionVolume*)volume);

	keeper->GetTransform().SetWorldScale(Vector3(meshSize, meshSize, meshSize));
	keeper->GetTransform().SetWorldPosition(position);

	keeper->SetRenderObject(new RenderObject(&keeper->GetTransform(), keeperMesh, nullptr, basicShader));
	keeper->SetPhysicsObject(new PhysicsObject(&keeper->GetTransform(), keeper->GetBoundingVolume()));

	keeper->GetPhysicsObject()->SetInverseMass(inverseMass);
	keeper->GetPhysicsObject()->InitCubeInertia();
	
	keeper->GetPhysicsObject()->SetElasticity(0.7f); // low elasticity material (like steel)
	keeper->GetPhysicsObject()->SetStiffness(200.f);
	
	//todo: change this to attacker type
	keeper->GetPhysicsObject()->SetCollisionType(ObjectCollisionType::IMPULSE | ObjectCollisionType::SPRING);// | ObjectCollisionType::JUMP_PAD);

	world->AddGameObject(keeper);
	keeper->SetWorld(world);
	keeper->SetNavigationGrid(navGrid);
	keeper->SetNavigationTable(navTable);

	//farmerAIObject = keeper; //todo: change this to an array of farmers?
	
	return keeper;
}

ComplexAIObject* GooseGame::AddComplexKeeperToWorld(const Vector3& position, NavigationGrid* navGrid, NavigationTable* navTable, float awakeTime)
{
	float meshSize = 4.0f;
	float inverseMass = 1.f / 4.f;

	ComplexAIObject* complexKeeper = new ComplexAIObject(position, 1, "Complex AI", awakeTime);

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.9f, 0.3f) * meshSize);
	complexKeeper->SetBoundingVolume((CollisionVolume*)volume);
	complexKeeper->GetTransform().SetWorldScale(Vector3(meshSize, meshSize, meshSize));
	complexKeeper->GetTransform().SetWorldPosition(position);
	complexKeeper->SetRenderObject(new RenderObject(&complexKeeper->GetTransform(), charA, nullptr, basicShader));
	complexKeeper->SetPhysicsObject(new PhysicsObject(&complexKeeper->GetTransform(), complexKeeper->GetBoundingVolume()));
	complexKeeper->GetPhysicsObject()->SetInverseMass(inverseMass);
	complexKeeper->GetPhysicsObject()->InitCubeInertia();
	complexKeeper->GetPhysicsObject()->SetElasticity(0.7f);
	complexKeeper->GetPhysicsObject()->SetStiffness(200.f);
	complexKeeper->GetPhysicsObject()->SetCollisionType(ObjectCollisionType::IMPULSE | ObjectCollisionType::SPRING);
	complexKeeper->GetRenderObject()->SetColour(Vector4(0, 0, 1, 1));
	world->AddGameObject(complexKeeper);
	complexKeeper->SetWorld(world);
	complexKeeper->SetNavigationGrid(navGrid);
	complexKeeper->SetNavigationTable(navTable);
	complexKeeper->GetPhysicsObject()->SetGravityUsage(false);
	return complexKeeper;
}

GameObject* GooseGame::AddAppleToWorld(const Vector3& position)
{
	CollectableObject* apple = new CollectableObject(CollectableType::APPLE, "Apple"); //todo: insert static id (use the final value of this to dictate victory checks, also reset when world respawns

	float size = 4.f;

	SphereVolume* volume = new SphereVolume(0.7f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform().SetWorldScale(Vector3(size, size, size));
	apple->GetTransform().SetWorldPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), appleMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetRenderObject()->SetColour(Vector4(1, 0, 1, 1)); //todo: make this red
	
	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();
	apple->GetPhysicsObject()->SetElasticity(0.8f); 
	apple->GetPhysicsObject()->SetStiffness(300.f); //todo: change this with collectable collision 

	 // apples aren't affected by gravity and should only collide with collectable collisions types (i.e player controlled characters)
	apple->GetPhysicsObject()->SetCollisionType(ObjectCollisionType::COLLECTABLE);
	apple->GetPhysicsObject()->SetGravityUsage(false);
	apple->GetPhysicsObject()->SetStatic(true);

	world->AddGameObject(apple);

	return apple;
}

GameObject* GooseGame::AddCornToWorld(const Vector3& position)
{
	CollectableObject* corn = new CollectableObject(CollectableType::CORN, "Corn"); //todo: insert static id (use the final value of this to dictate victory checks, also reset when world respawns

	float size = 0.7f;

	SphereVolume* volume = new SphereVolume(0.7f);
	corn->SetBoundingVolume((CollisionVolume*)volume);
	corn->GetTransform().SetWorldScale(Vector3(size, size, size));
	corn->GetTransform().SetWorldPosition(position);

	corn->SetRenderObject(new RenderObject(&corn->GetTransform(), sphereMesh, nullptr, basicShader));
	corn->SetPhysicsObject(new PhysicsObject(&corn->GetTransform(), corn->GetBoundingVolume()));

	corn->GetRenderObject()->SetColour(Vector4(1, 1, 0, 1)); //todo: give this a corn texture

	corn->GetPhysicsObject()->SetInverseMass(1.0f);
	corn->GetPhysicsObject()->InitSphereInertia();
	corn->GetPhysicsObject()->SetElasticity(0.8f);
	corn->GetPhysicsObject()->SetStiffness(300.f); //todo: change this with collectable collision 

	corn->GetPhysicsObject()->SetCollisionType(ObjectCollisionType::COLLECTABLE);
	corn->GetPhysicsObject()->SetGravityUsage(false);
	corn->GetPhysicsObject()->SetStatic(true);

	world->AddGameObject(corn);

	return corn;
}

GameObject* GooseGame::AddHatToWorld(const Vector3& position)
{
	CollectableObject* hat = new CollectableObject(CollectableType::HAT, "Hat");

	float size = 0.7f;

	SphereVolume* volume = new SphereVolume(0.7f);
	hat->SetBoundingVolume((CollisionVolume*)volume);
	hat->GetTransform().SetWorldScale(Vector3(size, size, size));
	hat->GetTransform().SetWorldPosition(position);

	hat->SetRenderObject(new RenderObject(&hat->GetTransform(), sphereMesh, nullptr, basicShader));
	hat->SetPhysicsObject(new PhysicsObject(&hat->GetTransform(), hat->GetBoundingVolume()));

	hat->GetRenderObject()->SetColour(Vector4(0, 1, 1, 1)); //todo: give this a hat texture and mesh

	hat->GetPhysicsObject()->SetInverseMass(1.0f);
	hat->GetPhysicsObject()->InitSphereInertia();
	hat->GetPhysicsObject()->SetElasticity(0.8f);
	hat->GetPhysicsObject()->SetStiffness(300.f);

	hat->GetPhysicsObject()->SetCollisionType(ObjectCollisionType::COLLECTABLE);
	hat->GetPhysicsObject()->SetGravityUsage(false);
	hat->GetPhysicsObject()->SetStatic(true);

	world->AddGameObject(hat);

	return hat;
}



void GooseGame::AddMultiDirectionalGate(const Vector3& startPosition, const Vector3& dimensions, const Vector4& color, int numberOfLinks, int nodeSize)
{

	float	invCubeMass = 1 / 1.f;
	float	maxDistance = (float)nodeSize;
	float	cubeDistance = (float) nodeSize;

	GameObject* start = AddCubeToWorld(startPosition + Vector3(0, 0, 0), Vector3(dimensions.x / 2.f, dimensions.y, dimensions.z/2.f), 0, 0, color);
	HeightConstraint* heightConstraintStart = new HeightConstraint(start, startPosition.y);
	world->AddConstraint(heightConstraintStart);
	
	GameObject* leftBlock = AddCubeToWorld(startPosition + Vector3((float)nodeSize * 1.4f, 0.f, 0), Vector3(dimensions.x * 2.f + 0.5f, dimensions.y, dimensions.z / 2.f), 0, invCubeMass, Vector4(0, 1, 0, 1));
	HeightConstraint* heightConstraint = new HeightConstraint(leftBlock, startPosition.y);
	HingeConstraint* hingeConstraint = new HingeConstraint(start, leftBlock);
	PositionConstraint* constraint = new PositionConstraint(start, leftBlock, nodeSize*1.4f);
	world->AddConstraint(heightConstraint);
	world->AddConstraint(hingeConstraint);
	world->AddConstraint(constraint);

	GameObject* rightBlock = AddCubeToWorld(startPosition - Vector3((float)nodeSize * 1.4f, 0.f, 0), Vector3(dimensions.x * 2.f + 0.5f, dimensions.y, dimensions.z / 2.f), 0, invCubeMass, Vector4(1, 1, 0, 1));
	HeightConstraint* heightConstraintRight = new HeightConstraint(rightBlock, startPosition.y);
	HingeConstraint* hingeConstraintRight = new HingeConstraint(start, rightBlock);
	PositionConstraint* constraintRight = new PositionConstraint(start, rightBlock, nodeSize * 1.4f);
	world->AddConstraint(heightConstraintRight);
	world->AddConstraint(hingeConstraintRight);
	world->AddConstraint(constraintRight);

	PositionConstraint* constraintRightAndLeft = new PositionConstraint(leftBlock, rightBlock, 2* nodeSize * 1.4f);
	world->AddConstraint(constraintRightAndLeft);

}

#pragma endregion ADD_OBJECTS

#pragma region UNUSED 

void GooseGame::BridgeConstraintTest()
{
	Vector3 cubeSize = Vector3(8, 8, 8);

	float	invCubeMass = 5;
	int		numLinks	= 3;
	float	maxDistance	= 25;
	float	cubeDistance = 20;

	Vector3 startPos = Vector3(0.f, 20.f, 0.f);

	GameObject* start = AddCubeToWorld(startPos + Vector3(0, 0, 0), cubeSize, 1,0);

	//GameObject* end = AddCubeToWorld(startPos + Vector3((numLinks + 2) * cubeDistance, 0, 0), cubeSize, 1, 0);

	GameObject* previous = start;

	for (int i = 0; i < numLinks; ++i) 
	{
		GameObject* block = AddCubeToWorld(startPos + Vector3((i + 1) * cubeDistance, 0, 0), cubeSize, 1, invCubeMass);
		PositionConstraint* constraint = new PositionConstraint(previous, block, maxDistance);
		world->AddConstraint(constraint);
		previous = block;
	}

	//PositionConstraint* constraint = new PositionConstraint(previous, end, maxDistance);
	//world->AddConstraint(constraint);
}

void GooseGame::SimpleGJKTest()
{
	Vector3 dimensions		= Vector3(5, 5, 5);
	Vector3 floorDimensions = Vector3(100, 2, 100);

	GameObject* fallingCube = AddCubeToWorld(Vector3(0, 20, 0), dimensions, true);//10.0f);
	GameObject* newFloor = AddCubeToWorld(Vector3(0, 0, 0), floorDimensions, false);//0.0f);

	delete fallingCube->GetBoundingVolume();
	delete newFloor->GetBoundingVolume();

	fallingCube->SetBoundingVolume((CollisionVolume*)new OBBVolume(dimensions));
	newFloor->SetBoundingVolume((CollisionVolume*)new OBBVolume(floorDimensions));

}


#pragma endregion UNUSED

#pragma region MOVEMENT
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// MOVEMENT RELATED STUFF
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void GooseGame::TPPlayerUpdate(float dt)
{
	//renderer->DrawString(" Click Force :" + std::to_string(forceMagnitude), Vector2(10, 20)); // Draw debug text at 10 ,20
	//forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;
	static const float forceMagnitude = 220.f; 
	static const float rotationSpeed = 90.f;
	Vector3 pitchYawRoll = playerCollection[thisPlayerIndex]->GetConstTransform().GetLocalOrientation().ToEuler();
	
	const Matrix4 view = world->GetMainCamera()->BuildViewMatrix();
	const Matrix4 camWorld = view.Inverse();
	const Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); 

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::Q) && playerCollection[thisPlayerIndex]->CanFart())
	{
		// fart
		const float radius = playerCollection[thisPlayerIndex]->GetFartRadius();
		//todo: add a fart sound effect 
		for (BasicAIObject* farmer : farmerCollection)
		{
			Vector3 direction = playerCollection[thisPlayerIndex]->GetConstTransform().GetWorldPosition() - farmer->GetConstTransform().GetWorldPosition();
			if (direction.Length() < radius)
			{
				direction.Normalise();

				Ray ray(playerCollection[thisPlayerIndex]->GetConstTransform().GetWorldPosition(), direction);
				RayCollision collision;
				if (world->Raycast(ray, collision, true))
				{
					farmer->GetPhysicsObject()->AddForceAtPosition(-direction * 1500.f, collision.collidedAt);
				}
			}
		}
	}
	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::SPACE) && playerCollection[thisPlayerIndex]->IsGrounded())
	{
		playerCollection[thisPlayerIndex]->GetPhysicsObject()->AddForce(Vector3(0, 3, 0) * forceMagnitude);
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W))
	{
		playerCollection[thisPlayerIndex]->GetPhysicsObject()->AddForce(playerCollection[thisPlayerIndex]->GetConstTransform().GetLocalOrientation() * Vector3(0, 0, 1) * forceMagnitude);
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S))
	{
		playerCollection[thisPlayerIndex]->GetPhysicsObject()->AddForce(playerCollection[thisPlayerIndex]->GetConstTransform().GetLocalOrientation() * Vector3(0, 0, -1) * forceMagnitude);
	}
	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::A))
	{
		pitchYawRoll.y += rotationSpeed * dt;
		pitchYawRoll.y = pitchYawRoll.y >= 0.0f ? pitchYawRoll.y <= 360.0f ? pitchYawRoll.y : pitchYawRoll.y - 360.0f : pitchYawRoll.y + 360.0f;
		playerCollection[thisPlayerIndex]->GetTransform().SetLocalOrientation(Quaternion::EulerAnglesToQuaternion(pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z));
	}	
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) 
	{
		pitchYawRoll.y -= rotationSpeed * dt;
		pitchYawRoll.y = pitchYawRoll.y >= 0.0f ? pitchYawRoll.y <= 360.0f ? pitchYawRoll.y : pitchYawRoll.y - 360.0f : pitchYawRoll.y + 360.0f;
		playerCollection[thisPlayerIndex]->GetTransform().SetLocalOrientation(Quaternion::EulerAnglesToQuaternion(pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z));
	}
}

// third person camera update
void GooseGame::TPCameraUpdate()
{
	const Vector3 objPos = playerCollection[thisPlayerIndex]->GetTransform().GetWorldPosition();
	const Vector3 camPos = objPos + playerCollection[thisPlayerIndex]->GetConstTransform().GetLocalOrientation() * lockedOffset;
	const Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos + Vector3(0, 5, 0), Vector3(0, 1, 0));

	Matrix4 modelMat = temp.Inverse();
	Quaternion q(modelMat);
	Vector3 angles = q.ToEuler();

	world->GetMainCamera()->SetPosition(camPos);
	world->GetMainCamera()->SetPitch(angles.x);
	world->GetMainCamera()->SetYaw(angles.y);

	//todo: make a nicer camera controller 
	//float newPitch = world->GetMainCamera()->GetPitch() - Window::GetMouse()->GetRelativePosition().y;
	////Bounds check the pitch, to be between straight up and straight down ;)
	//newPitch = min(newPitch, 45.0f);
	//newPitch = max(newPitch, -45.0f);

	////Update the mouse by how much
	//world->GetMainCamera()->SetPitch(newPitch);
}

void GooseGame::SelectObject()
{
	if (selectionObject)
	{
		selectionObject->DebugDraw();
	}

	if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT))
	{
		if (selectionObject)
		{	
			selectionObject->GetRenderObject()->SetColour(previousSelectedColor);
			selectionObject = nullptr;
		}
		Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());
		RayCollision closestCollision;
		if (world->Raycast(ray, closestCollision, true))
		{
			selectionObject = (GameObject*)closestCollision.node;
			previousSelectedColor = selectionObject->GetRenderObject()->GetColour();
			selectionObject->GetRenderObject()->SetColour(Vector4(0, 0, 0, 1));
		}
	}
}
#pragma endregion MOVEMENT