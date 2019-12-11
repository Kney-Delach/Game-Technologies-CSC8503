/***************************************************************************
* Filename		: NavigationGrid.h
* Name			: Ori Lazar
* Date			: 07/12/2019
* Description	: Declaration of a Grid-Based navigable area.
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
#include "NavigationMap.h"
#include <string>

namespace NCL
{
	namespace CSC8503
	{
		// 7.12.2019
		// represents a node in a path finding graph
		struct GridNode
		{
			GridNode* parent; 
			GridNode* connected[4]; // pointers to nodes it may connect to
			int costs[4]; // costs of navigating from this node to the connected nodes (above)
			Vector3 position; // represents center of this node in world space co-ordinates
			float f; // distance traveled to get to the current node 
			float g; // heuristic of how far this node is from the destination (manhattan)
			int type;
			int nodeID; 

			//todo: initialize position?
			GridNode() : parent(nullptr), position(Vector3(1.f, 1.f, 1.f)), f(0), g(0), type(0)
			{
				for (int i = 0; i < 4; ++i) 
				{
					connected[i] = nullptr;
					costs[i] = 0;
				}
			}
			~GridNode() = default;
		};

		// 7.12.2019
		// represents a node in a grid-based graph
		class NavigationGrid : public NavigationMap
		{
		public:
			NavigationGrid();
			NavigationGrid(const std::string&filename);
			~NavigationGrid();
			bool FindPath(const Vector3& from, const Vector3& to, NavigationPath& outPath) override;

			GridNode* GetNodes() const { return allNodes; }
			int GetNodeSize() const { return nodeSize; }
			int GetWidth() const { return gridWidth; }
			int GetHeight() const { return gridHeight; }
		protected:
			bool NodeInList(GridNode* node, std::vector<GridNode*>& nodeList) const; //todo: make static?
			GridNode* RemoveBestNode(std::vector<GridNode*>& nodeList) const; //todo: make static?
			float Heuristic(GridNode* startNode, GridNode* endNode) const; //todo: make static?
			int nodeSize;
			int gridWidth;
			int gridHeight;
			GridNode* allNodes;
		};
	}
}