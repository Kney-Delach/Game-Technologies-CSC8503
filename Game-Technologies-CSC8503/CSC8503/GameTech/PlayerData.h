/***************************************************************************
* Filename		: PlayerData.h
* Name			: Ori Lazar
* Date			: 06/12/2019
* Description	: Contains the data accumulated by each player, score, inventory etc...
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

namespace NCL
{
	namespace CSC8503
	{
		class PlayerData
		{
		public:
			PlayerData();
			~PlayerData();
		private:
			//todo: add inventory (i.e, items collectable)
			//todo: add UI_Draw funciton that draws the current collected count of each collectable of the player to the UI
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
		};
	}
}