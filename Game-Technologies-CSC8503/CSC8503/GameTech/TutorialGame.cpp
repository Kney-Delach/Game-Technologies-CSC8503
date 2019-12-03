/***************************************************************************
* Filename		: TutorialGame.h
* Name			: Ori Lazar
* Date			: 28/11/2019
* Description	: Central point for running the tutorial game.
    .---.
  .'_:___".
  |__ --==|
  [  ]  :[|
  |__| I=[|
  / / ____|
 |-/.____.'
/___\ /___\
***************************************************************************/
#include "TutorialGame.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"

#include "../CSC8503Common/PositionConstraint.h"

using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame()
{
	world		= new GameWorld();
	renderer	= new GameTechRenderer(*world);
	physics		= new PhysicsSystem(*world);

	forceMagnitude	= 10.0f;
	useGravity		= false;
	inSelectionMode = false;

	// debug toggles
	displayBoundingVolumes = false;

	Debug::SetRenderer(renderer);

	InitialiseAssets();
}

// initialises all the assets used in this project.
void TutorialGame::InitialiseAssets()
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

TutorialGame::~TutorialGame()
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

void TutorialGame::UpdateGame(float dt)
{
	if (!inSelectionMode) 	
		world->GetMainCamera()->UpdateCamera(dt);	
	else
		MoveSelectedObject();
	

	if (lockedObject != nullptr)
		LockedCameraMovement();
	
	UpdateKeys();

	if (useGravity)	
		Debug::Print("(G)ravity on", Vector2(10, 40));	
	else 	
		Debug::Print("(G)ravity off", Vector2(10, 40));

	SelectObject();

	world->UpdateWorld(dt);
	renderer->Update(dt);
	physics->Update(dt);

	Debug::FlushRenderables();

	
	// debug draw
	if(displayBoundingVolumes)
	{
		world->DrawBoundingVolumes();
		//renderer->Render();

		renderer->DebugRender();
	}
	else
	{
		renderer->Render();
	}	
}

void TutorialGame::UpdateKeys()
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
	
	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.

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

	//todo: implement lock state specific object movement.
	// pass through lock state specific object movement 
//	if (lockedObject) 
//		LockedObjectMovement();
	DebugObjectMovement(); // move selected object 
}

void TutorialGame::LockedObjectMovement()
{
	Matrix4 view		= world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld	= view.Inverse();

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) 
	{
		selectionObject->GetPhysicsObject()->AddForce(-rightAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) 
	{
		selectionObject->GetPhysicsObject()->AddForce(rightAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) 
	{
		selectionObject->GetPhysicsObject()->AddForce(fwdAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) 
	{
		selectionObject->GetPhysicsObject()->AddForce(-fwdAxis);
	}
}

void  TutorialGame::LockedCameraMovement()
{
	if (lockedObject != nullptr) 
	{
		Vector3 objPos = lockedObject->GetTransform().GetWorldPosition();
		Vector3 camPos = objPos + lockedOffset;

		Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0, 1, 0));

		Matrix4 modelMat = temp.Inverse();

		Quaternion q(modelMat);
		Vector3 angles = q.ToEuler(); //nearly there now!

		world->GetMainCamera()->SetPosition(camPos);
		world->GetMainCamera()->SetPitch(angles.x);
		world->GetMainCamera()->SetYaw(angles.y);
	}
}

// move selected gameobjects with keyboard presses
void TutorialGame::DebugObjectMovement()
{
	if (inSelectionMode && selectionObject) 
	{
		GameObjectMovement();
	}
}

/*

Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around.

*/
bool TutorialGame::SelectObject()
{
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::R)) 
	{
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode) 
		{
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else 
		{
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}
	if (inSelectionMode) 
	{
		renderer->DrawString("Press R to change to camera mode!", Vector2(10, 0));

		if(selectionObject)
		{
			if (selectionObjectFront)
			{
				selectionObjectFront->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				selectionObjectFront = nullptr;
			}
			if (SelectionObjectBack)
			{
				SelectionObjectBack->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				SelectionObjectBack = nullptr;
			}
			
			// getting object IN-FRONT of selected object
			Ray objectForwardRay = selectionObject->BuildRayFromDirection(Vector3(0, 0, 1)); //(selectionObject->GetConstTransform().GetWorldPosition(), selectionObject->GetConstTransform().GetWorldOrientation() * Vector3(0,0,1));
			RayCollision closestObjectCollision;
			if (world->Raycast(objectForwardRay, closestObjectCollision, true))
			{
				selectionObjectFront = (GameObject*)closestObjectCollision.node;
				selectionObjectFront->DrawDebug(Vector4(0, 0, 1, 1));
				GameObject::DrawLineBetweenObjects(selectionObject, selectionObjectFront);
			}

			// getting object BEHIND selected object
			Ray objectDownRay(selectionObject->GetConstTransform().GetWorldPosition(), selectionObject->GetConstTransform().GetWorldOrientation() * Vector3(0, 0, -1));
			RayCollision closestBehindCollision;
			if (world->Raycast(objectDownRay, closestBehindCollision, true))
			{
				SelectionObjectBack = (GameObject*)closestBehindCollision.node;
				SelectionObjectBack->DrawDebug(Vector4(1, 0, 0, 1));
				GameObject::DrawLineBetweenObjects(selectionObject, SelectionObjectBack);
			}
		}
		
		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT)) 
		{
			if (selectionObject) 
			{	//set colour to deselected;
				selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				selectionObject = nullptr;
				if(selectionObjectFront)
				{
					selectionObjectFront->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
					selectionObjectFront = nullptr;
				}
				if (SelectionObjectBack) 
				{
					SelectionObjectBack->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
					SelectionObjectBack = nullptr;
				}
			}

			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());
			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true))  // object has been selected 
			{
				selectionObject = (GameObject*)closestCollision.node;
				selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
				return true;
			}
			return false;
		}
		if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::L)) 
		{
			if (selectionObject) 
			{
				if (lockedObject == selectionObject) 
					lockedObject = nullptr;
				else 
					lockedObject = selectionObject;
			}
		}
	}
	else 	
		renderer->DrawString("Press R to change to select mode!", Vector2(10, 0));	
	return false;
}

//todo: Add keys to modify position of selected object using forces
// 28.11.2019 - linear motion
// If an object has been clicked, it can be pushed with the right mouse button, by an amount determined by the scroll wheel.
void TutorialGame::MoveSelectedObject()
{
	renderer->DrawString(" Click Force :" + std::to_string(forceMagnitude), Vector2(10, 20)); // Draw debug text at 10 ,20
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;
	
	if (!selectionObject) // No object has been selected!
		return;

	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::RIGHT)) 
	{
		Ray ray = CollisionDetection::BuildRayFromMouse(* world->GetMainCamera());	
		RayCollision closestCollision;
		if(world->Raycast(ray, closestCollision, true)) 
		{
			if (closestCollision.node == selectionObject)
				selectionObject->GetPhysicsObject()->AddForceAtPosition(ray.GetDirection() * forceMagnitude, closestCollision.collidedAt); // angular calculations included
		}
	}
}

void TutorialGame::GameObjectMovement()
{
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SPACE)) // up
	{
		selectionObject->GetPhysicsObject()->AddForce(Vector3(0, forceMagnitude, 0));
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::E)) // down
	{
		selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -forceMagnitude, 0));
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A)) // x left
	{
		selectionObject->GetPhysicsObject()->AddForce(Vector3(-forceMagnitude, 0, 0));
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) // x right
	{
		selectionObject->GetPhysicsObject()->AddForce(Vector3(forceMagnitude, 0, 0));
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) // z left
	{
		selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -forceMagnitude));
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) // z right
	{
		selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, forceMagnitude));
	}
}

void TutorialGame::InitCamera()
{
	world->GetMainCamera()->SetNearPlane(0.5f);
	world->GetMainCamera()->SetFarPlane(2000.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
	lockedObject = nullptr;
}

void TutorialGame::InitWorld() //todo:
{
	world->ClearAndErase();
	physics->Clear();

	InitGooseGameWorld();
}

void TutorialGame::InitGooseGameWorld()
{
	//todo: initialize ground level terrain
	InitGroundLevelTerrain();
	AddGooseToWorld(Vector3(0.f,1.f,0.f));
	//InitMixedGridWorld(10, 10, 3.5f, 3.5f);
	//todo: create initialize player character functionality
	//todo: create collectable objects init
	//todo: create DumbAI init function (atleast 1 for each collectable zone 
	//todo: create SmartAI init function (maybe 1 that follows player throughout entire level? can swim? 
}

void TutorialGame::InitGroundLevelTerrain()
{
	// 1. add water terrain (curently a blue square)
	AddFloorToWorld(Vector3(0, -5, 0), Vector3(100, 5, 100), Vector4(0, 0, 1, 1), true);

	// 2. add spawn land terrain
	AddFloorToWorld(Vector3(0, -5, 0),Vector3(25,5.5,25), Vector4(1,1,1,1));

	// 3. add playable terrains (areas with collectables and AI)
	// red world
	AddFloorToWorld(Vector3(0, -5, 200), Vector3(50, 5.5, 100), Vector4(1, 0, 0, 1));

	// green world 
	AddFloorToWorld(Vector3(0, -5, -200), Vector3(50, 5.5, 100), Vector4(0, 1, 0, 1));

	// yellow world
	AddFloorToWorld(Vector3(200, -5, 0), Vector3(100, 5.5, 50), Vector4(1, 1, 0, 1));

	// light blue world
	AddFloorToWorld(Vector3(-200, -5, 0), Vector3(100, 5.5, 50), Vector4(0, 1, 1, 1));
}


/*

A single function to add a large immoveable cube to the bottom of our world

*/
GameObject* TutorialGame::AddFloorToWorld(const Vector3& position, const Vector3& dimensions, const Vector4& colour, bool resolveAsSprings)
{
	GameObject* floor = new GameObject("Ground");

	AABBVolume* volume = new AABBVolume(dimensions);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform().SetWorldScale(dimensions);
	floor->GetTransform().SetWorldPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->SetElasticity(0.8);
	floor->GetPhysicsObject()->InitCubeInertia();

	if(resolveAsSprings)
	{
		floor->GetPhysicsObject()->SetResolveAsSpring(true);
		floor->GetPhysicsObject()->SetStiffness(200.f);
		floor->GetPhysicsObject()->SetResolveAsImpulse(false);
	}

	floor->GetRenderObject()->SetColour(colour);
	
	floor->GetLayer().SetLayerID(1); // set layer ID to 1 (not raycastable)
	
	world->AddGameObject(floor);

	return floor;
}

/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple' 
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, bool isHollow, float inverseMass)
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
		sphere->GetPhysicsObject()->InitSphereInertia(true);
		sphere->GetPhysicsObject()->SetStiffness(100.f);
	}
	else
	{
		sphere->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
		sphere->GetPhysicsObject()->InitSphereInertia(false);
		sphere->GetPhysicsObject()->SetStiffness(100.f);
	}

	sphere->GetPhysicsObject()->SetElasticity(1.f); // highly elastic material (like a rubber ball) 

	sphere->GetLayer().SetLayerID(0); // set layer ID to 1 (not raycastable)

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, bool isAABB, float inverseMass)
{
	GameObject* cube = new GameObject("Cube");

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

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->SetElasticity(0.01); // low elasticity material (like steel)
	cube->GetPhysicsObject()->SetStiffness(8.f);
	cube->GetPhysicsObject()->InitCubeInertia();

	cube->GetLayer().SetLayerID(0); // set layer ID to 1 (not raycastable)
	
	world->AddGameObject(cube);

	return cube;
}

GameObject* TutorialGame::AddGooseToWorld(const Vector3& position)
{
	float size			= 1.0f;
	float inverseMass = 1.f / 4.f;

	GameObject* goose = new GameObject("Goose");

	SphereVolume* volume = new SphereVolume(size);
	goose->SetBoundingVolume((CollisionVolume*)volume);

	goose->GetTransform().SetWorldScale(Vector3(size,size,size) );
	goose->GetTransform().SetWorldPosition(position);

	goose->SetRenderObject(new RenderObject(&goose->GetTransform(), gooseMesh, nullptr, basicShader));
	goose->SetPhysicsObject(new PhysicsObject(&goose->GetTransform(), goose->GetBoundingVolume()));

	goose->GetPhysicsObject()->SetInverseMass(inverseMass);
	goose->GetPhysicsObject()->InitSphereInertia();
	goose->GetPhysicsObject()->SetStiffness(300.f);
	goose->GetPhysicsObject()->SetResolveAsImpulse(true);
	goose->GetPhysicsObject()->SetResolveAsImpulse(true);
	
	world->AddGameObject(goose);

	return goose;
}

GameObject* TutorialGame::AddParkKeeperToWorld(const Vector3& position)
{
	float meshSize = 4.0f;
	float inverseMass = 0.5f;

	GameObject* keeper = new GameObject("Park Keeper");

	AABBVolume* volume = new AABBVolume(Vector3(0.3, 0.9f, 0.3) * meshSize);
	keeper->SetBoundingVolume((CollisionVolume*)volume);

	keeper->GetTransform().SetWorldScale(Vector3(meshSize, meshSize, meshSize));
	keeper->GetTransform().SetWorldPosition(position);

	keeper->SetRenderObject(new RenderObject(&keeper->GetTransform(), keeperMesh, nullptr, basicShader));
	keeper->SetPhysicsObject(new PhysicsObject(&keeper->GetTransform(), keeper->GetBoundingVolume()));

	keeper->GetPhysicsObject()->SetInverseMass(inverseMass);
	keeper->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(keeper);

	return keeper;
}

GameObject* TutorialGame::AddCharacterToWorld(const Vector3& position)
{
	float meshSize = 4.0f;
	float inverseMass = 0.5f;

	auto pos = keeperMesh->GetPositionData();

	Vector3 minVal = pos[0];
	Vector3 maxVal = pos[0];

	for (auto& i : pos) {
		maxVal.y = max(maxVal.y, i.y);
		minVal.y = min(minVal.y, i.y);
	}

	GameObject* character = new GameObject("Character");

	float r = rand() / (float)RAND_MAX;

	AABBVolume* volume = new AABBVolume(Vector3(0.3, 0.9f, 0.3) * meshSize);
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform().SetWorldScale(Vector3(meshSize, meshSize, meshSize));
	character->GetTransform().SetWorldPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), r > 0.5f ? charA : charB, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitCubeInertia();

	// set temporary color (for debugging)
	character->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));
	world->AddGameObject(character);

	return character;
}

GameObject* TutorialGame::AddAppleToWorld(const Vector3& position)
{
	GameObject* apple = new GameObject("Apple");

	SphereVolume* volume = new SphereVolume(0.7f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform().SetWorldScale(Vector3(4, 4, 4));
	apple->GetTransform().SetWorldPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), appleMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(apple);

	return apple;
}

void TutorialGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius)
{
	for (int x = 0; x < numCols; ++x) 
	{
		for (int z = 0; z < numRows; ++z) 
		{
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddSphereToWorld(position, radius, false);
		}
	}
	AddFloorToWorld(Vector3(0, -8, 0));
}

void TutorialGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing)
{
	const float sphereRadius = 1.f;
	const Vector3 cubeDims = Vector3(1,1,1);
	for (int x = 0; x < numRows; ++x) 
	{
		for (int z = 0; z < numCols; ++z) 
		{
			Vector3 position = Vector3(3.f * x * colSpacing, 10.0f, 3.f * z * rowSpacing);
			if (x % 2)
			{
				if(x % 3)
					AddCubeToWorld(position, cubeDims, true);
				else
					AddCubeToWorld(position, cubeDims, false);
			}
			else
			{
				if(x % 3)
					AddSphereToWorld(position, sphereRadius, false);
				else 
					AddSphereToWorld(position, sphereRadius, true);

			}
		}
	}
}

void TutorialGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims)
{
	for (int x = 1; x < numCols+1; ++x) {
		for (int z = 1; z < numRows+1; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddCubeToWorld(position, cubeDims);
		}
	}
	AddFloorToWorld(Vector3(0, -8, 0));
}

void TutorialGame::BridgeConstraintTest()
{
	Vector3 cubeSize = Vector3(8, 8, 8);

	float	invCubeMass = 5;
	int		numLinks	= 25;
	float	maxDistance	= 30;
	float	cubeDistance = 20;

	Vector3 startPos = Vector3(500, 1000, 500);

	GameObject* start = AddCubeToWorld(startPos + Vector3(0, 0, 0), cubeSize, 0);

	GameObject* end = AddCubeToWorld(startPos + Vector3((numLinks + 2) * cubeDistance, 0, 0), cubeSize, 0);

	GameObject* previous = start;

	for (int i = 0; i < numLinks; ++i) {
		GameObject* block = AddCubeToWorld(startPos + Vector3((i + 1) * cubeDistance, 0, 0), cubeSize, invCubeMass);
		PositionConstraint* constraint = new PositionConstraint(previous, block, maxDistance);
		world->AddConstraint(constraint);
		previous = block;
	}

	PositionConstraint* constraint = new PositionConstraint(previous, end, maxDistance);
	world->AddConstraint(constraint);
}

void TutorialGame::SimpleGJKTest()
{
	Vector3 dimensions		= Vector3(5, 5, 5);
	Vector3 floorDimensions = Vector3(100, 2, 100);

	GameObject* fallingCube = AddCubeToWorld(Vector3(0, 20, 0), dimensions, 10.0f);
	GameObject* newFloor	= AddCubeToWorld(Vector3(0, 0, 0), floorDimensions, 0.0f);

	delete fallingCube->GetBoundingVolume();
	delete newFloor->GetBoundingVolume();

	fallingCube->SetBoundingVolume((CollisionVolume*)new OBBVolume(dimensions));
	newFloor->SetBoundingVolume((CollisionVolume*)new OBBVolume(floorDimensions));

}

