/***************************************************************************
* Filename		: NavigationTable.cpp
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
#include "NavigationTable.h"

#include "NavigationGrid.h"
#include "NavigationPath.h"

namespace NCL
{
	namespace CSC8503
	{
		NavigationTable::NavigationTable(int numNodes, NavigationGrid* grid)
			: numberOfNodes(numNodes)
		{
			navigationTable = new NavTableNode*[numberOfNodes];
			for (int i = 0; i < numberOfNodes; ++i)
			{
				navigationTable[i] = new NavTableNode[numberOfNodes];
			}
			CalculateTable(grid);
		}

		NavigationTable::~NavigationTable()
		{
			for (int i = 0; i < numberOfNodes; ++i)
			{
				delete[] navigationTable[i];
			}
			delete[] navigationTable;
		}

		void NavigationTable::CalculateTable(NavigationGrid* grid)
		{

			const Vector3 startPos(80, 0, 80);
			const Vector3 endPos(60, 0, 10);

			//todo: iterate over the following for all possible combinations of nodes

			NavigationPath path;
			bool found = (*grid).FindPath(startPos, endPos, path);

			Vector3 pos;

			if(path.RemoveWaypoint(pos))
			{
			}
		}
	}
}