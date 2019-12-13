/***************************************************************************
* Filename		: BasicAIObject.cpp
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
#include "BasicAIObject.h"

#include "CollisionDetection.h"
#include "Debug.h"
#include "NavigationGrid.h"
#include "NavigationTable.h"
#include "PlayerObject.h"
#include "StateMachine.h"
#include "State.h"
#include "StateTransition.h"
#include "GameWorld.h"
#include <random>

namespace NCL
{
	namespace CSC8503
	{
		BasicAIObject::BasicAIObject(const Vector3& spawnPos, const int type, const std::string name)
			: GameObject(name), aiType(type), objectID(0), spawnPosition(spawnPos)
		{
			world = nullptr;
			moveTowardsTarget = false;
			InitStateMachine();
			debugStartNodeIndex = -1;
			debugEndNodeIndex = -1;
		} 

		BasicAIObject::~BasicAIObject()
		{
			delete navigationGrid;
			delete navigationTable;
			delete stateMachine;
		}

		void BasicAIObject::OnCollisionBegin(GameObject* other)
		{
			//todo: add force away from wall if interacts with one....
			if(other->GetName() == "Wall")
			{
			}
			if(other->GetName() == "Goose")
			{
				((PlayerObject*)other)->DropItems();
				if(other == target) // this way if the farmer hits a different player's goose, it still makes the goose drop its items, but keeps chasing its taget
					target = nullptr;
			}
		}

		void BasicAIObject::InitStateMachine()
		{
			stateMachine = new StateMachine();

			// setup state specific update functions
			void* data = this;

			StateFunction idleState = [](void* data)
			{
				BasicAIObject* obj = (BasicAIObject*)(data);
				obj->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));
			};
			StateFunction moveState = [](void* data)
			{
				BasicAIObject* obj = (BasicAIObject*)(data);
				obj->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
				obj->Move();
			};

			GenericState* stateIdle = new GenericState(idleState, static_cast<void*>(this), "AI Idle", "No movement!");
			GenericState* stateMoving = new GenericState(moveState, static_cast<void*>(this), "AI Moving", "Moving towards goose or Homing.");

			stateMachine->AddState(stateIdle);
			stateMachine->AddState(stateMoving);

			GenericTransition<bool&, bool>* transitionA = new GenericTransition<bool&, bool>(GenericTransition<bool&, bool>::EqualsTransition, moveTowardsTarget,true, stateIdle, stateMoving);
			GenericTransition<bool&, bool>* transitionB = new GenericTransition<bool&, bool>(GenericTransition<bool&, bool>::NotEqualsTransition, moveTowardsTarget,true, stateMoving, stateIdle);

			stateMachine->AddTransition(transitionA);
			stateMachine->AddTransition(transitionB);
		}

		void BasicAIObject::Update()
		{
			if (target == nullptr)
			{
				//todo: replace this with raycast towards goose's position
				
				const float halfSize = ((AABBVolume*)boundingVolume)->GetHalfDimensions().y/2.f;
				//// if no target currently then raycast for a target with a bonus item
				FindTarget(Vector3(0.f,-halfSize,0.f),Vector3(0, 0, 1));
				FindTarget(Vector3(0.f,-halfSize,0.f),Vector3(0, 0, -1));
				FindTarget(Vector3(0.f,-halfSize,0.f),Vector3(-1, 0, 0));
				FindTarget(Vector3(0.f,-halfSize,0.f),Vector3(1, 0, 0));
				//FindTarget(Vector3(0.f,-halfSize,0.f),Vector3(1, 0, 1));
				//FindTarget(Vector3(0.f,-halfSize,0.f),Vector3(1, 0, -1));
				//FindTarget(Vector3(0.f,-halfSize,0.f),Vector3(-1, 0, 0.99));
				//FindTarget(Vector3(0.f,-halfSize,0.f),Vector3(-1, 0, -1));
				//todo: The following function should successfully hit the sphere of the goose when in direct line of sight, but doesn't for known reasons
				//for (PlayerObject* g : *playerObjectCollection)
				//{
				//	const Vector3 gooseDirection = g->GetConstTransform().GetWorldPosition() - GetConstTransform().GetWorldPosition();
				//	FindTarget(Vector3(0.f, -halfSize, 0.f), gooseDirection);
				//}
			}
			else if(!((PlayerObject*)target)->GetBonusItemStatus())
			{
				target = nullptr;
			}
			stateMachine->Update();
		}

		bool BasicAIObject::FindTarget(const Vector3& offset, const Vector3& direction, bool drawLines)
		{
			Ray objectForwardRay = BuildRayFromDirectionOffset(offset, direction);
			RayCollision closestObjectCollision;
			if (world->Raycast(objectForwardRay, closestObjectCollision, true))
			{
				GameObject* obj = (GameObject*)closestObjectCollision.node;
				
				if(drawLines)
				{
					GameObject::DrawLineInDirection(this, direction * 5.f, offset);
					GameObject::DrawLineBetweenObjectsOffset(offset, this, obj);
				}

				if(obj->GetName() == "Goose")
				{
					PlayerObject* player = (PlayerObject*)obj;
					if(player->GetBonusItemStatus())
					{
						target = obj;
						moveTowardsTarget = true;
						return true;
					}
				}
			}
			return false;
		}

		void BasicAIObject::DebugDraw()
		{
			stateMachine->DebugDraw();
			DrawDebugVolume();
			
			const std::string msg = "Selected GameObject: " + name;
			Debug::Print(msg, Vector2(5, 700), Vector4(0, 0, 0, 1));

			const std::string val = GetPhysicsObject()->IsStatic() ? "True" : "false";
			const std::string staticMsg = "Is Static: " + val;
			Debug::Print(staticMsg, Vector2(5, 650), Vector4(0, 0, 0, 1));
			
			const float halfSize = ((AABBVolume*)boundingVolume)->GetHalfDimensions().y / 2.f;
			FindTarget(Vector3(0.f, -halfSize, 0.f), Vector3(0, 0, 1), true);
			FindTarget(Vector3(0.f, -halfSize, 0.f), Vector3(0, 0, -1), true);
			FindTarget(Vector3(0.f, -halfSize, 0.f), Vector3(-1, 0, 0), true);
			FindTarget(Vector3(0.f, -halfSize, 0.f), Vector3(1, 0, 0), true);

			if(debugStartNodeIndex != -1 && debugEndNodeIndex != -1)
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

		void BasicAIObject::Move() //todo: change this with state machine 
		{
			int returnHome = -1;
			if(target && navigationTable)
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
					physicsObject->AddForce(direction * 150.f);
				}
				if(nextNodeIndex == startIndex)
				{
					Vector3 direction = targetPos - GetConstTransform().GetWorldPosition();
					direction.Normalise();
					physicsObject->AddForce(direction * 150.f);
				}
				
				returnHome = nextNodeIndex;
			}
			
			if(target == nullptr)
			{
				const Vector3 check = GetTransform().GetLocalPosition() - spawnPosition;
				const float n1 = -(float)navigationGrid->GetNodeSize()/2.f;
				const float n2 = (float)navigationGrid->GetNodeSize()/2.f;
				if ((n1 < check.x  && check.x < n2) && (n1 < check.z) && (check.z < n2))
				{
					moveTowardsTarget = false;
					return;
				}
			}
			if(returnHome == -1)
			{
				if (target)
					target = nullptr;
				if(navigationTable)
				{
					NavTableNode** table = navigationTable->GetNavTable();

					const Vector3 startPos = GetConstTransform().GetWorldPosition();
					const Vector3 targetPos = spawnPosition;

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
						physicsObject->AddForce(direction * 150.f);
						debugStartNodeIndex = startIndex;
						debugEndNodeIndex = endIndex;
					}
				}
			}
		}
	}
}
