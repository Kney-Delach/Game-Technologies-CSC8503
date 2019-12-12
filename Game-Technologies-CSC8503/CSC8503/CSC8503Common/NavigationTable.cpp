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
		NavigationTable::NavigationTable(int numNodes, NavigationGrid* grid, bool loadTableFromFile)
		{
			numberOfNodes = numNodes;
			navigationTable = new NavTableNode*[numberOfNodes];
			for (int i = 0; i < numberOfNodes; ++i)
			{
				navigationTable[i] = new NavTableNode[numberOfNodes];
			}
			if(!loadTableFromFile)
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
			tableWidth = navWidth;
			tableHeight = navHeight;

			GridNode* allNavigationNodes = grid->GetNodes();
			
			for (int start = 0; start < navWidth * navHeight; ++start) // for each node in the list
			{
				for (int end = 0; end < navWidth * navHeight; ++end) // for each node in the list 
				{
					if (&allNavigationNodes[start] == &allNavigationNodes[end])
					{
						navigationTable[start][end].nearestNodeID = end;
					}
					else
					{
						NavigationPath path;
						const bool found = grid->FindPath(allNavigationNodes[start].position, allNavigationNodes[end].position, path);
						if(found)
						{
							Vector3 position; 
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

		// writer 
		std::ostream& operator<<(std::ostream& out, const NavigationTable& navTable)
		{
			// 1. need to know number of nodes
			// 2. need to know navigation table width
			// 3. need to know navigation table height
			out << navTable.numberOfNodes << "\n";
			out << navTable.tableWidth << "\n";
			out << navTable.tableHeight << "\n";
			
			for (int i = 0; i < navTable.tableWidth * navTable.tableHeight; ++i)
			{
				for (int k = 0; k < navTable.tableWidth * navTable.tableHeight; ++k)
				{
					out << navTable.navigationTable[i][k].nearestNodeID << " ";
				}
				out << "\n";
			}
			return out;
		}

		// reader
		std::istream& operator>>(std::istream& in, NavigationTable& navTable)
		{
			int temp;
			in >> temp;
			std::cout << temp << "\n";
			if (temp != navTable.numberOfNodes)
				std::cout << "Invalid Navigation Table File Loaded: Number of nodes: " << temp << " versus assigned: " << navTable.numberOfNodes << "\n";
			in >> temp;
			navTable.tableWidth = temp;
			in >> temp;
			navTable.tableHeight = temp;
			std::cout << "Loading navigation table with width: " << navTable.tableWidth << " and height: " << navTable.tableHeight << "\n";

			for (int i = 0; i < navTable.tableWidth * navTable.tableHeight; ++i)
			{
				for (int k = 0; k < navTable.tableWidth * navTable.tableHeight; ++k)
				{
					in >> navTable.navigationTable[i][k].nearestNodeID;
				}
			}
			return in;
		}
	}
}