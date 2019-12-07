/***************************************************************************
* Filename		: PlayerObject.h
* Name			: Ori Lazar
* Date			: 06/12/2019
* Description	: Player controlled gameobject
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
#include "GameObject.h"

namespace NCL
{
	namespace CSC8503
	{
		class PlayerObject : public GameObject
		{
		public:
			PlayerObject(const std::string name = "Player Object");
			virtual ~PlayerObject();
			
			unsigned int AddObjectToInventory(GameObject* object);
			
			int GetInventorySize() const { return inventory.size(); }
			std::vector<GameObject*>& GetInventory() { return inventory; }
			
			void UpdateInventoryTransformations(const float dt) const;

			void DropItems();
			void DrawInventoryToUI() const; 
		protected:
			std::vector<GameObject*> inventory; 
		};
	}
}


//todo: add inventory (i.e, items collectable)
//todo: add UI_Draw function that draws the current collected count of each collectable of the player to the UI
//todo: add collectObject function that handles colliding with collectable objects

//todo: DUMB AI:
//todo: add a way to handle being caught by the AI (i.e resets the collected objects that are still in the current inventory of the player back to their original location) 

//todo: SMART AI: (not here, but spawn a new one every 30 seconds)
//todo: handle dropping items, losing points, and random wander for 2 seconds.

//todo: add collectedHandler function to handle when the player returns to their home island with collected objects (increase the score + call victory check)
//todo: create a winning check funciton to handle custom message if player has won or lost the game.
//todo: 
//todo: add debugdraw function that keeps track of and draws most recently collided with objects, and displays information about them aswell as this.
//todo: add debugdrawcollected function that displays the location from which an object was collected, its (make this static btw) gameobject ID...
