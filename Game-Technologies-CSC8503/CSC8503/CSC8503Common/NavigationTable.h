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
			int nearestNodeID;
		};
		
		class NavigationTable
		{
		public:
			NavigationTable(int numNodes, NavigationGrid* grid, bool loadTableFromFile = false);
			~NavigationTable();
			NavTableNode** GetNavTable() const { return navigationTable; }
			friend std::ostream& operator<<(std::ostream& out, const NavigationTable& navTable);
			friend std::istream& operator>>(std::istream& in, NavigationTable& navTable);

		private:
			void CalculateTable(NavigationGrid* grid);
		private:
			NavTableNode** navigationTable;
			int numberOfNodes;
			int tableWidth;
			int tableHeight;
		};
	}
}
