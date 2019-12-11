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
		{
			numberOfNodes = numNodes;
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
			const int navWidth = grid->GetWidth();
			const int navHeight = grid->GetHeight();

			GridNode* allNavigationNodes = grid->GetNodes();
			
			for (int start = 0; start < navWidth * navHeight; ++start) // for each node in the list
			{
				for (int end = 0; end < navWidth * navHeight; ++end) // for each node in the list 
				{
					if (&allNavigationNodes[start] == &allNavigationNodes[end])
					{
						navigationTable[start][end].position = allNavigationNodes[end].position;
						navigationTable[start][end].nearestNodeID = end;
					}
					else
					{
						NavigationPath path;
						const bool found = grid->FindPath(allNavigationNodes[start].position, allNavigationNodes[end].position, path);
						if(found)
						{
							Vector3 position; 
							if(path.RemoveWaypoint(position))
							{
								navigationTable[start][end].position = position;
							}
							int id = 0;
							if(path.RemoveWaypointID(id))
							{
								navigationTable[start][end].nearestNodeID = id;
							}
						}
						else
						{
							// not reachable
							navigationTable[start][end].nearestNodeID = -1;
							navigationTable[start][end].position = Vector3(-1.f,-1.f,-1.f);
						}
					}
				}
			}

			//todo: remove this section
			// output the navigation table to cmd (testing)
			for (int i = 0; i < navWidth * navHeight; ++i)
			{
				std::cout << i << ",";
			}
			std::cout << "\n";
			std::cout << "------------------------------------------------------\n";
			for (int i = 0; i < navWidth * navHeight; ++i)
			{
				std::cout << i << "|";
				for (int k = 0; k < navWidth * navHeight; ++k)
				{
					if (navigationTable[i][k].nearestNodeID == -1)
						std::cout << ".,";
					else
						std::cout << navigationTable[i][k].nearestNodeID << ",";
				}
				std::cout << "\n";
			}
		}
	}
}