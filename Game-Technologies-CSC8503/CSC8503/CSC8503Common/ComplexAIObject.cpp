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
#include "PlayerObject.h"
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
		static float COOLDOWN = 10.f;
		ComplexAIObject::ComplexAIObject(const Vector3& spawnPos, const int type, const std::string name, float awkTime)
			: GameObject(name), aiType(type), objectID(0), spawnPosition(spawnPos)
		{
			world = nullptr;
			attackTarget = false;
			moveTowardsTarget = true;
			useStun = false;
			useThrow = false;
			useBurp = false;
			useYell = false;
			InitStateMachine();
			debugStartNodeIndex = -1;
			debugEndNodeIndex = -1;
			recentAttackChoice = -1;
			currentIslandIndex = 0;
			cooldownTimer = COOLDOWN;
			awakeTime = awkTime;
			srand(time(NULL)); // Randomize seed initialization
		}

		ComplexAIObject::~ComplexAIObject()
		{
			delete navigationGrid;
			delete navigationTable;
			delete combatStateMachine;
			delete stateMachine;
		}

		void ComplexAIObject::OnCollisionBegin(GameObject* other)
		{
		}

		void ComplexAIObject::InitCombatStateMachine()
		{
			combatStateMachine = new StateMachine();
			
			void* data = this;

			StateFunction reloadAttack = [](void* data)
			{
				ComplexAIObject* obj = (ComplexAIObject*)(data);
				if (obj->GetCooldownTimer() <= 0)
				{
					const int randNum = rand() % 4; // Generate a random number between 0 and 1
					if (randNum == 0)
					{
						obj->SetStunUse(true);
						obj->SetRecentAttackChoice(0);
					}
					if (randNum == 1)
					{
						obj->SetThrowUse(true);
						obj->SetRecentAttackChoice(1);
					}
					if (randNum == 2)
					{
						obj->SetBurpUse(true);
						obj->SetRecentAttackChoice(2);
					}
					if (randNum == 3)
					{
						obj->SetYellUse(true);
						obj->SetRecentAttackChoice(3);
					}
				}
				if (obj->GetRecentAttackChoice() == 0)
				{
					const std::string staticMsg = "DROP THOSE ITEMS THIEF!!!";
					Debug::Print(staticMsg, Vector2(5, 600), Vector4(1, 0, 0, 1));
				}
				if (obj->GetRecentAttackChoice() == 1)
				{
					const std::string staticMsg = "LETS SEE YOU CATCH THESE!";
					Debug::Print(staticMsg, Vector2(250, 550), Vector4(1, 0, 0, 1));
				}
				if (obj->GetRecentAttackChoice() == 2)
				{
					const std::string staticMsg = "BUUUUUUUUUUUURRRRRRPPPPPP!";
					Debug::Print(staticMsg, Vector2(250, 500), Vector4(1, 0, 1, 1));
				}
				if (obj->GetRecentAttackChoice() == 3)
				{
					const std::string staticMsg = "WHAT ARE YOU DOING IN MY PARK?!";
					Debug::Print(staticMsg, Vector2(250, 450), Vector4(1, 0, 1, 1));
				}				
			};
			
			StateFunction stunAttack = [](void* data)
			{
				ComplexAIObject* obj = (ComplexAIObject*)(data);
				obj->StunTarget(); //todo: This should slow the player's speed
				obj->SetStunUse(false);
				obj->SetCooldownTimer(COOLDOWN);
			};
			StateFunction throwAttack = [](void* data)
			{
				ComplexAIObject* obj = (ComplexAIObject*)(data);
				obj->ThrowTowardsTarget(); //todo: add force onto some sphere object in direction of goose, new gameobject (DamagingGameObject)
				obj->SetThrowUse(false);
				obj->SetCooldownTimer(COOLDOWN);
			};

			StateFunction burpAttack = [](void* data)
			{
				ComplexAIObject* obj = (ComplexAIObject*)(data);
				obj->BurpAtTarget(); //todo: raycast a force onto the target
				obj->SetBurpUse(false);
				obj->SetCooldownTimer(COOLDOWN);
			};

			StateFunction yellAttack = [](void* data)
			{
				ComplexAIObject* obj = (ComplexAIObject*)(data);
				obj->YellAtTarget(); //todo: invert the target's controls for 2 seconds
				obj->SetYellUse(false);
				obj->SetCooldownTimer(COOLDOWN);
			};

			GenericState* reloadState = new GenericState(reloadAttack, static_cast<void*>(this), "Smart AI Reloading", "Reloading Ammo!");
			GenericState* stunState = new GenericState(stunAttack, static_cast<void*>(this), "Smart AI Stun", "Stun the goose!");
			GenericState* throwState = new GenericState(throwAttack, static_cast<void*>(this), "Smart AI Throw", "Throw ball at goose!");
			GenericState* burpState = new GenericState(burpAttack, static_cast<void*>(this), "Smart AI Burp", "Burps towards the target!");
			GenericState* yellState = new GenericState(yellAttack, static_cast<void*>(this), "Smart AI Yell", "Yells at the target!");

			combatStateMachine->AddState(reloadState);
			combatStateMachine->AddState(stunState);
			combatStateMachine->AddState(throwState);
			combatStateMachine->AddState(burpState);
			combatStateMachine->AddState(yellState);

			GenericTransition<bool&, bool>* transitionReloadStun = new GenericTransition<bool&, bool>(GenericTransition<bool&, bool>::EqualsTransition, useStun, true, reloadState, stunState);
			GenericTransition<bool&, bool>* transitionStunReload = new GenericTransition<bool&, bool>(GenericTransition<bool&, bool>::EqualsTransition, useStun, false, stunState, reloadState);
			
			GenericTransition<bool&, bool>* transitionReloadThrow = new GenericTransition<bool&, bool>(GenericTransition<bool&, bool>::EqualsTransition, useThrow, true, reloadState, throwState);
			GenericTransition<bool&, bool>* transitionThrowReload = new GenericTransition<bool&, bool>(GenericTransition<bool&, bool>::EqualsTransition, useThrow, false, throwState, reloadState);

			GenericTransition<bool&, bool>* transitionReloadBurp = new GenericTransition<bool&, bool>(GenericTransition<bool&, bool>::EqualsTransition, useBurp, true, reloadState, burpState);
			GenericTransition<bool&, bool>* transitionBurpReload= new GenericTransition<bool&, bool>(GenericTransition<bool&, bool>::EqualsTransition, useBurp, false, burpState, reloadState);

			GenericTransition<bool&, bool>* transitionReloadYell = new GenericTransition<bool&, bool>(GenericTransition<bool&, bool>::EqualsTransition, useYell, true, reloadState, yellState);
			GenericTransition<bool&, bool>* transitionYellReload = new GenericTransition<bool&, bool>(GenericTransition<bool&, bool>::EqualsTransition, useYell, false, yellState, reloadState);

			combatStateMachine->AddTransition(transitionReloadStun);
			combatStateMachine->AddTransition(transitionStunReload);
			combatStateMachine->AddTransition(transitionReloadThrow);
			combatStateMachine->AddTransition(transitionThrowReload);
			combatStateMachine->AddTransition(transitionReloadBurp);
			combatStateMachine->AddTransition(transitionBurpReload);
			combatStateMachine->AddTransition(transitionReloadYell);
			combatStateMachine->AddTransition(transitionYellReload);
		}

		void ComplexAIObject::StunTarget()
		{
			std::cout << "Stunning target!\n";
			//todo: daze the target and make it drop its items
			((PlayerObject*)target)->DropItems();
			(*playerIslandCollection)[currentIslandIndex]->UpdateScore(-1);
		}

		void ComplexAIObject::ThrowTowardsTarget()
		{
			std::cout << "Spitting at target!\n";
			Vector3 direction = target->GetConstTransform().GetWorldPosition() - GetConstTransform().GetWorldPosition();
			direction.Normalise();
			Ray ray(target->GetConstTransform().GetWorldPosition(), direction);
			RayCollision collision;
			if (world->Raycast(ray, collision, true))
			{
				target->GetPhysicsObject()->AddForceAtPosition(-direction * 3000, collision.collidedAt);
			}
		}

		void ComplexAIObject::BurpAtTarget()
		{
			std::cout << "Burping towards the target!\n";
			Vector3 direction = target->GetConstTransform().GetWorldPosition() - GetConstTransform().GetWorldPosition();
			direction.Normalise();
			Ray ray(target->GetConstTransform().GetWorldPosition(), direction);
			RayCollision collision;
			if (world->Raycast(ray, collision, true))
			{
				target->GetPhysicsObject()->AddForceAtPosition(-direction * 1500.f, collision.collidedAt);
			}
		}

		void ComplexAIObject::YellAtTarget()
		{
			std::cout << "Yelling at the target!\n";
			Vector3 direction = target->GetConstTransform().GetWorldPosition() - GetConstTransform().GetWorldPosition();
			direction.Normalise();
			Ray ray(target->GetConstTransform().GetWorldPosition(), direction);
			RayCollision collision;
			if (world->Raycast(ray, collision, true))
			{
				target->GetPhysicsObject()->AddForceAtPosition(-direction * 1500.f, collision.collidedAt);
			}
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
			
			InitCombatStateMachine();
			StateFunction combatState = [](void* data)
			{
				ComplexAIObject* obj = (ComplexAIObject*)(data);
				obj->GetRenderObject()->SetColour(Vector4(0, 0, 1, 1));
				obj->combatStateMachine->Update();
			};

			GenericState* stateIdle = new GenericState(idleState, static_cast<void*>(this), "Smart AI Idle", "No movement!");
			GenericState* stateMoving = new GenericState(moveState, static_cast<void*>(this), "Smart AI Moving", "Moving towards goose or Homing.");
			GenericState* stateCombat = new GenericState(combatState, static_cast<void*>(this), "Smart AI Combat", "Using Combat FSM");

			stateMachine->AddState(stateIdle);
			stateMachine->AddState(stateMoving);
			stateMachine->AddState(stateCombat);

			GenericTransition<bool&, bool>* transitionA = new GenericTransition<bool&, bool>(GenericTransition<bool&, bool>::EqualsTransition, moveTowardsTarget, true, stateIdle, stateMoving);
			GenericTransition<bool&, bool>* transitionB = new GenericTransition<bool&, bool>(GenericTransition<bool&, bool>::NotEqualsTransition, moveTowardsTarget, true, stateMoving, stateIdle);
			
			GenericTransition<bool&, bool>* transitionMovingToAttacking = new GenericTransition<bool&, bool>(GenericTransition<bool&, bool>::EqualsTransition, attackTarget, true, stateMoving, stateCombat);
			GenericTransition<bool&, bool>* transitionAttackingToMoving = new GenericTransition<bool&, bool>(GenericTransition<bool&, bool>::EqualsTransition, attackTarget, false, stateCombat, stateMoving);

			stateMachine->AddTransition(transitionA);
			stateMachine->AddTransition(transitionB);
			stateMachine->AddTransition(transitionMovingToAttacking);
			stateMachine->AddTransition(transitionAttackingToMoving);
		}

		void ComplexAIObject::Update(float dt)
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
			currentIslandIndex = highScoreIndex;
			target = (*playerIslandCollection)[highScoreIndex]->GetParent();

			const Vector3 startPos = GetConstTransform().GetWorldPosition();
			const Vector3 targetPos = target->GetConstTransform().GetWorldPosition();
			const Vector3 distance = startPos - targetPos;
			if (distance.x < 15.f && distance.x > -15.f && distance.z < 15.f && distance.z > -15.f)
			{
				attackTarget = true;
			}
			else
			{
				attackTarget = false;
			}
			
			cooldownTimer -= dt;
			stateMachine->Update();
		}



		void ComplexAIObject::DebugDraw()
		{
			stateMachine->DebugDraw();
			combatStateMachine->DebugDraw(1);
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
			}
		}
	}
}
