/***************************************************************************
* Filename		: NavigationGrid.cpp
* Name			: Ori Lazar
* Date			: 07/12/2019
* Description	: Implementation of a Grid-Based navigable area.
    .---.
  .'_:___".
  |__ --==|
  [  ]  :[|
  |__| I=[|
  / / ____|
 |-/.____.'
/___\ /___\
***************************************************************************/
#include "NavigationGrid.h"
#include "../../Common/Assets.h"
#include <fstream>

using namespace NCL;
using namespace CSC8503;

const int LEFT_NODE		= 0;
const int RIGHT_NODE	= 1;
const int TOP_NODE		= 2;
const int BOTTOM_NODE	= 3;

const char WALL_NODE	= 'x';
const char FLOOR_NODE	= '.';

NavigationGrid::NavigationGrid()
{
	nodeSize	= 0;
	gridWidth	= 0;
	gridHeight	= 0;
	allNodes	= nullptr;
}

NavigationGrid::NavigationGrid(const std::string&filename) : NavigationGrid()
{
	std::ifstream infile(Assets::DATADIR + filename);

	infile >> nodeSize;
	infile >> gridWidth;
	infile >> gridHeight;

	allNodes = new GridNode[gridWidth * gridHeight];

	int counter = 0;
	for (int y = 0; y < gridHeight; ++y) 
	{
		for (int x = 0; x < gridWidth; ++x) 
		{
			GridNode&n = allNodes[(gridWidth * y) + x];
			n.nodeID = counter++;
			char type = 0;
			infile >> type;
			n.type = type;
			n.position = Vector3((float)(x * nodeSize), 0, (float)(y * nodeSize));
		}
	}
	
	//now to build the connectivity between the nodes
	for (int y = 0; y < gridHeight; ++y) 
	{
		for (int x = 0; x < gridWidth; ++x) 
		{
			GridNode&n = allNodes[(gridWidth * y) + x];

			if (y > 0) 
			{ //get the above node
				n.connected[0] = &allNodes[(gridWidth * (y - 1)) + x];
			}
			if (y < gridHeight - 1) 
			{ //get the below node
				n.connected[1] = &allNodes[(gridWidth * (y + 1)) + x];
			}
			if (x > 0) 
			{ //get left node
				n.connected[2] = &allNodes[(gridWidth * (y)) + (x - 1)];
			}
			if (x < gridWidth - 1) 
			{ //get right node
				n.connected[3] = &allNodes[(gridWidth * (y)) + (x + 1)];
			}
			for (int i = 0; i < 4; ++i) 
			{
				if (n.connected[i]) 
				{
					if (n.connected[i]->type == '.') 
					{
						n.costs[i]		= 1;
					}
					if (n.connected[i]->type == 'x') 
					{
						n.connected[i] = nullptr; //actually a wall, disconnect!
					}
				}
			}
		}	
	}
}

NavigationGrid::~NavigationGrid()
{
	delete[] allNodes;
}

// 7.12.2019
// A* algorithm to find the shortest path between 2 world positions using a navigation path  
bool NavigationGrid::FindPath(const Vector3& from, const Vector3& to, NavigationPath& outPath)
{
	// need to work out which navigation nodes the start and end positions are situated in
	//todo: fix the float to int precision errors
	const int fromX = from.x / nodeSize;
	const int fromZ = from.z / nodeSize;
	const int toX = to.x / nodeSize;
	const int toZ = to.z / nodeSize;

	if (fromX < 0 || fromX > gridWidth - 1 || fromZ < 0 || fromZ > gridHeight - 1) 
	{
		return false; // outside of map region !
	}

	 if (toX < 0 || toX > gridWidth - 1 || toZ < 0 || toZ > gridHeight - 1)
	 {
		 return false;
	 }
	GridNode* startNode = &allNodes[(fromZ * gridWidth) + fromX];
	GridNode* endNode = &allNodes[(toZ * gridWidth) + toX];
	
	std::vector<GridNode*> openList;
	std::vector<GridNode*> closedList;
	
	openList.emplace_back(startNode);
	
	startNode->f = 0;
	startNode->g = 0;
	startNode->parent = nullptr;
	
	GridNode* currentBestNode = nullptr;
	
	while (!openList.empty()) 
	{
		currentBestNode = RemoveBestNode(openList);

		if (currentBestNode == endNode) // we ’ve found the path !
		{
			GridNode* node = endNode;
			while (node != nullptr) 
			{
				outPath.AddWaypoint(node->position);
				outPath.AddWaypointID(node->nodeID);
				node = node->parent; // Build up the waypoints
			}
			return true;
		}
		else
		{
			for (int i = 0; i < 4; ++i) 
			{
				GridNode * neighbour = currentBestNode->connected[i];
				if (!neighbour) // might not be connected 
				{
					continue;
				}
				
				const bool inClosed = NodeInList(neighbour, closedList);
				if (inClosed) 
				{
					continue; // already discarded this neighbour ...
				}

				const float h = Heuristic(neighbour, endNode);
				const float g = currentBestNode->g + currentBestNode->costs[i];
				const float f = h + g;

				const bool inOpen = NodeInList(neighbour, openList);

				if (!inOpen) // first time we ’ve seen this neighbour
				{
					openList.emplace_back(neighbour);
				}
				
				// might be a better route to this node !
				if (!inOpen || f < neighbour-> f) 
				{
					neighbour->parent = currentBestNode;
					neighbour->f = f;
					neighbour->g = g;
				}
			}
			closedList.emplace_back(currentBestNode);
		}
	}
	return false; // open list emptied out with no path !
}

// 7.12.2019
// returns whether or not a node is in a list of nodes
bool NavigationGrid::NodeInList(GridNode* node, std::vector<GridNode*>& nodeList) const
{
	std::vector<GridNode*>::iterator i = std::find(nodeList.begin(), nodeList.end(), node);
	return i == nodeList.end() ? false : true;
}

// 7.12.2019
// returns the grid node with the lowest "f = g + h" cost in the node list
GridNode* NavigationGrid::RemoveBestNode(std::vector<GridNode*>& nodeList) const
{
	std::vector<GridNode*>::iterator bestNodeIndex = nodeList.begin();
	GridNode* bestNode = *nodeList.begin();
	for(auto i = nodeList.begin(); i != nodeList.end(); ++i)
	{
		if((*i)->f < bestNode->f)
		{
			bestNode = (*i);
			bestNodeIndex = i;
		}
	}
	nodeList.erase(bestNodeIndex);
	return bestNode;
}

// 7.12.2019
// currently returns the manhattan distance heuristic
float NavigationGrid::Heuristic(GridNode* startNode, GridNode* endNode) const
{
	return (startNode->position - endNode->position).Length();
}