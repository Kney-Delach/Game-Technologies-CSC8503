/***************************************************************************
* Filename		: ComplexAIObject.cpp
* Name			: Ori Lazar
* Date			: 9/12/2019
* Description	: Stored the representation of an AI object that uses A* pathfinding
*                 to track down a player.
    .---.
  .'_:___".
  |__ --==|
  [  ]  :[|
  |__| I=[|
  / / ____|
 |-/.____.'
/___\ /___\
***************************************************************************/
#include "ComplexAIObject.h"

#include "CollisionDetection.h"
#include "Debug.h"
#include "NavigationGrid.h"
#include "NavigationTable.h"
#include "PlayerIsland.h"
#include "StateMachine.h"
#include "State.h"
#include "StateTransition.h"
#include "GameWorld.h"
#include <random>
#include "FloatToString.h"

namespace NCL
{
	namespace CSC8503
	{
		ComplexAIObject::ComplexAIObject(const Vector3& spawnPos, const int type, const std::string name)
			: GameObject(name), aiType(type), objectID(0), spawnPosition(spawnPos)
		{
			world = nullptr;
			moveTowardsTarget = true;
			InitStateMachine();
			debugStartNodeIndex = -1;
			debugEndNodeIndex = -1;
		}

		ComplexAIObject::~ComplexAIObject()
		{
			delete navigationGrid;
			delete navigationTable;
			delete stateMachine;
		}

		void ComplexAIObject::OnCollisionBegin(GameObject* other)
		{
		}

		void ComplexAIObject::InitStateMachine()
		{
			stateMachine = new StateMachine();

			// setup state specific update functions
			void* data = this;

			StateFunction idleState = [](void* data)
			{
				ComplexAIObject* obj = (ComplexAIObject*)(data);
				obj->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));
			};
			StateFunction moveState = [](void* data)
			{
				ComplexAIObject* obj = (ComplexAIObject*)(data);
				obj->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
				obj->Move();
			};

			GenericState* stateIdle = new GenericState(idleState, static_cast<void*>(this), "Smart AI Idle", "No movement!");
			GenericState* stateMoving = new GenericState(moveState, static_cast<void*>(this), "Smart AI Moving", "Moving towards goose or Homing.");

			stateMachine->AddState(stateIdle);
			stateMachine->AddState(stateMoving);

			GenericTransition<bool&, bool>* transitionA = new GenericTransition<bool&, bool>(GenericTransition<bool&, bool>::EqualsTransition, moveTowardsTarget, true, stateIdle, stateMoving);
			GenericTransition<bool&, bool>* transitionB = new GenericTransition<bool&, bool>(GenericTransition<bool&, bool>::NotEqualsTransition, moveTowardsTarget, true, stateMoving, stateIdle);

			stateMachine->AddTransition(transitionA);
			stateMachine->AddTransition(transitionB);
		}

		void ComplexAIObject::Update()
		{
			int highScoreIndex = 0;
			int highscore = 0;
			int currentIndex = 0;
			for(PlayerIsland* island : *playerIslandCollection)
			{
				if(highscore <= island->GetScore())
				{
					highscore = island->GetScore();
					highScoreIndex = currentIndex;
				}
				currentIndex++;
			}
			target = (*playerIslandCollection)[highScoreIndex]->GetParent();
			// moveTowardsTarget = true; // dynamically set 

			stateMachine->Update();
		}



		void ComplexAIObject::DebugDraw()
		{
			stateMachine->DebugDraw();
			DrawDebugVolume();

			const std::string msg = "Selected GameObject: " + name;
			Debug::Print(msg, Vector2(5, 700), Vector4(0, 0, 0, 1));

			const std::string val = GetPhysicsObject()->IsStatic() ? "True" : "false";
			const std::string staticMsg = "Is Static: " + val;
			Debug::Print(staticMsg, Vector2(5, 650), Vector4(0, 0, 0, 1));

			std::string physicsMSG = "Physics Volume: ";
			if (!boundingVolume)
			{
				physicsMSG += "No Volume;";
			}
			else
			{
				switch (boundingVolume->type)
				{
				case VolumeType::AABB:
				{
					physicsMSG += "AABB";
					break;
				}
				case VolumeType::OBB:
				{
					physicsMSG += "OBB";
					break;
				}
				case VolumeType::Sphere:
				{
					physicsMSG += "SPHERE";
					break;
				}
				}
			}
			Debug::Print(physicsMSG, Vector2(5, 600), Vector4(0, 0, 0, 1));

			// world position
			const Vector3 pos = GetConstTransform().GetWorldPosition();
			const std::string worldPos = "World Position: (" + FloatToString(pos.x, 2) + ", " + FloatToString(pos.y, 2) + ", " + FloatToString(pos.z, 2) + ")";
			Debug::Print(worldPos, Vector2(5, 550), Vector4(0, 0, 0, 1));

			// rotation
			const Quaternion rot = GetConstTransform().GetWorldOrientation();
			const std::string worldOrientation = "World Orientation: (" + FloatToString(rot.x, 2) + ", " + FloatToString(rot.y, 2) + ", " + FloatToString(rot.z, 2) + ")";
			Debug::Print(worldOrientation, Vector2(5, 500), Vector4(0, 0, 0, 1));

			if (debugStartNodeIndex != -1 && debugEndNodeIndex != -1)
			{
				// display navigation table positions
				GridNode* allNodes = navigationGrid->GetNodes();
				GridNode* startNode = &allNodes[debugStartNodeIndex];
				GridNode* endNode = &allNodes[debugEndNodeIndex];
				Debug::DrawCircle(startNode->position, 1.f, Vector4(0.f, 0.f, 0.f, 1.f)); // start at black
				Debug::DrawCircle(endNode->position, 1.f, Vector4(1.f, 1.f, 1.f, 1.f)); // finish at white 
				Debug::DrawLine(startNode->position, endNode->position, Vector4(1, 1, 1, 1)); // white line 
			}
		}

		void ComplexAIObject::Move()
		{
			int returnHome = -1;
			if (target && navigationTable)
			{
				NavTableNode** table = navigationTable->GetNavTable();

				const Vector3 startPos = GetConstTransform().GetWorldPosition();
				const Vector3 targetPos = target->GetConstTransform().GetWorldPosition();

				const int targetX = (targetPos.x + navigationGrid->GetNodeSize() / 2.f) / navigationGrid->GetNodeSize();
				const int targetZ = (targetPos.z + navigationGrid->GetNodeSize() / 2.f) / navigationGrid->GetNodeSize();

				const int startX = (startPos.x + navigationGrid->GetNodeSize() / 2.f) / navigationGrid->GetNodeSize();
				const int startZ = (startPos.z + navigationGrid->GetNodeSize() / 2.f) / navigationGrid->GetNodeSize();

				if (startX < 0 || startX > navigationGrid->GetWidth() - 1 || startZ < 0 || startZ > navigationGrid->GetHeight() - 1)
				{
					return;
				}

				if (targetX < 0 || targetX > navigationGrid->GetWidth() - 1 || targetZ < 0 || targetZ > navigationGrid->GetHeight() - 1)
				{
					return;
				}

				GridNode* allNodes = navigationGrid->GetNodes();
				GridNode* startNode = &allNodes[(startZ * navigationGrid->GetWidth()) + startX];
				GridNode* endNode = &allNodes[(targetZ * navigationGrid->GetWidth()) + targetX];

				const int startIndex = startNode->nodeID;
				const int endIndex = endNode->nodeID;
				const int nextNodeIndex = table[startIndex][endIndex].nearestNodeID;

				if (nextNodeIndex != -1)
				{
					GridNode* nextNode = &allNodes[nextNodeIndex];
					Vector3 direction = nextNode->position - GetConstTransform().GetWorldPosition();
					direction.Normalise();
					direction.y = 0;
					physicsObject->AddForce(direction * 250.f);
					debugStartNodeIndex = startIndex;
					debugEndNodeIndex = nextNodeIndex;
				}
				if (nextNodeIndex == startIndex)
				{
					Vector3 direction = targetPos - GetConstTransform().GetWorldPosition();
					direction.Normalise();
					direction.y = 0;
					physicsObject->AddForce(direction * 250.f);
				}
				returnHome = nextNodeIndex;
			}
		}
	}
}
