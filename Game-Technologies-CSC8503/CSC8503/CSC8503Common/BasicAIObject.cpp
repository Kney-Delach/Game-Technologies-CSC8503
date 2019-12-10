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

namespace NCL
{
	namespace CSC8503
	{
		BasicAIObject::BasicAIObject(const Vector3& spawnPos, const int type, const std::string name)
			: GameObject(name), aiType(type), objectID(0), spawnPosition(spawnPos)
		{
			InitStateMachine();
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
				std::cout << "Collided with wall, add force here!\n";
			}
			if(other->GetName() == "Goose")
			{
				((PlayerObject*)other)->DropItems();
				if(other == target) // this way if the farmer hits a different player's goose, it still makes the goose drop its items, but keeps chasing its taget
					target = nullptr;
				//todo: change state to homing 
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

			//todo: implement the following state
			//StateFunction homingState = [](void* data)
			//{
			//	BasicAIObject* obj = (BasicAIObject*)(data);
			//	obj->GetRenderObject()->SetColour(Vector4(0, 0, 1, 1));
			//	obj->Move();
			//};
			//
			//GenericState* stateA = new GenericState(stateFunctionA, static_cast<void*>(&data));
			//
			GenericState* stateIdle = new GenericState(idleState, static_cast<void*>(data));
			GenericState* stateMoving = new GenericState(moveState, static_cast<void*>(data));

			stateMachine->AddState(stateIdle);
			stateMachine->AddState(stateMoving);

			GenericTransition<GameObject*, GameObject*>* transitionA = new GenericTransition<GameObject*, GameObject*>(GenericTransition<GameObject*, GameObject*>::NotEqualsTransition, target, nullptr, stateIdle, stateMoving);
			GenericTransition<GameObject*, GameObject*>* transitionB = new GenericTransition<GameObject*, GameObject*>(GenericTransition<GameObject*, GameObject*>::EqualsTransition, target, nullptr, stateMoving, stateIdle);

			stateMachine->AddTransition(transitionA);
			stateMachine->AddTransition(transitionB);
		}

		void BasicAIObject::Update()
		{
			stateMachine->Update();
		}

		void BasicAIObject::ReturnHome()
		{
		}

		void BasicAIObject::DebugDraw()
		{
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
				
				if(nextNodeIndex != -1)
				{
					GridNode* nextNode = &allNodes[nextNodeIndex];

					Debug::DrawCircle(startNode->position, 1.f, Vector4(1.f, 0.f, 0.f, 1.f));
					Debug::DrawCircle(nextNode->position, 1.f, Vector4(1.f, 1.f, 1.f, 1.f));
					Debug::DrawLine(startPos, nextNode->position, Vector4(1, 1, 0, 1));
				}
			}
		}

		void BasicAIObject::Move() //todo: change this with state machine 
		{
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
			}
			else
			{
				if (GetConstTransform().GetWorldPosition() == spawnPosition)
				{
					return;
				}
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
					}
				}
			}
		}
	}
}
