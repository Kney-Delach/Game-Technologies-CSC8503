/***************************************************************************
* Filename		: NavigationTable.h
* Name			: Ori Lazar
* Date			: 9/12/2019
* Description	: Used to store pre-calculate A* pathfinding data.
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
#include "../../Common/Vector3.h"

namespace NCL
{
	namespace CSC8503
	{
		class NavigationGrid;

		struct NavTableNode
		{
			int nodeID;
			int nearestNodeID;
			Maths::Vector3 position;
		};
		
		class NavigationTable
		{
		public:
			NavigationTable(int numNodes, NavigationGrid* grid);
			~NavigationTable();
		private:
			void CalculateTable(NavigationGrid* grid);
		private:
			NavTableNode** navigationTable;
			int numberOfNodes; 
		};
	}
}
