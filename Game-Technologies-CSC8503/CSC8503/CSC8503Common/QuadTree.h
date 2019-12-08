/***************************************************************************
* Filename		: QuadTree.h
* Name			: Ori Lazar
* Date			: 31/10/2019
* Description	: QuadTree        -> Represents a quad tree spatial acceleration structure.
*                 QuadTreeNode    -> Represents an individual part of the quadtree.
*                 QuadTreeEntries -> Multiples of these can be added to a tree node (contains position and size).
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
#include "../../Common/Vector2.h"
#include "CollisionDetection.h"

#include "Debug.h"
#include <list>
#include <functional>

namespace NCL
{
	using namespace NCL::Maths;
	
	namespace CSC8503
	{
		template<class T>
		class QuadTree;

		// Entries 
		template<class T>
		struct QuadTreeEntry
		{
			Vector3 position;
			Vector3 size;
			T object;
			
			QuadTreeEntry(T obj, Vector3 pos, Vector3 s) : position(pos), size(s), object(obj) {}
		};

		// Nodes
		template<class T>
		class QuadTreeNode
		{
		public:
			typedef std::function<void(std::list<QuadTreeEntry<T>>&)> QuadTreeFunction;
		protected:
			friend class QuadTree<T>;

			QuadTreeNode() = default;

			QuadTreeNode(Vector2 pos, Vector2 size)
			{
				children		= nullptr;
				this->position	= pos;
				this->size		= size;
			}

			~QuadTreeNode()
			{
				delete[] children;
			}

			// 8.12.2019
			void Insert(T& object, const Vector3& objectPos, const Vector3& objectSize, int depthLeft, int maxSize)
			{
				// 1. intersection test
				if (!CollisionDetection::AABBTest(objectPos, Vector3(position.x, 0, position.y), objectSize, Vector3(size.x, 1000.0f, size.y)))
				{
					return; // if AABBs don't overlap, then won't overlap with children either
				}

				// !object does overlap

				// 2. not a leaf node, recursively try to add node to children
				if (children)
				{ 
					for (int i = 0; i < 4; ++i) 
					{
						children[i].Insert(object, objectPos, objectSize, depthLeft - 1, maxSize);
					}
				}
				else // 3. add object ot its own list of stored objects, as currently in a leaf node 
				{ 
					contents.push_back(QuadTreeEntry<T>(object, objectPos, objectSize));
					if (static_cast<int>(contents.size()) > maxSize && depthLeft > 0) // reached maximum node contents size, split up the node (if not reached maximum tree depth)
					{
						if (!children) 
						{
							Split();
							// reinsert the contents
							for (const auto& i : contents) 
							{
								for (int j = 0; j < 4; ++j) 
								{
									auto entry = i;
									children[j].Insert(entry.object, entry.position, entry.size, depthLeft - 1, maxSize);
								}
							}
							contents.clear(); // contents now distributed
						}
					}				}
			}

			// 8.12.2019
			// tesselating area into 4 nodes, assuming either 4 or 0 children instantiated simultaneously
			void Split()
			{
				const Vector2 halfSize = size / 2.0f;
				children = new QuadTreeNode <T>[4];
				children[0] = QuadTreeNode<T>(position + Vector2(-halfSize.x, halfSize.y), halfSize);
				children[1] = QuadTreeNode<T>(position + Vector2(halfSize.x, halfSize.y), halfSize);
				children[2] = QuadTreeNode<T>(position + Vector2(-halfSize.x, -halfSize.y), halfSize);
				children[3] = QuadTreeNode<T>(position + Vector2(halfSize.x, -halfSize.y), halfSize);
			}

			void DebugDraw()
			{
			}

			// 8.12.2019
			// assuming that valid contents will always be located in a leaf node
			void OperateOnContents(QuadTreeFunction& function)
			{
				if (children)
				{
					for (int i = 0; i < 4; ++i)
					{
						children[i].OperateOnContents(function);
					}
				}
				else
				{
					if(!contents.empty())
					{
						function(contents);
					}
				}
			}

		protected:
			std::list<QuadTreeEntry<T>> contents; 
			Vector2 position;
			Vector2 size;
			QuadTreeNode<T>* children;
		};
	}
}


namespace NCL
{
	using namespace NCL::Maths;
	
	namespace CSC8503
	{
		// Tree
		template<class T>
		class QuadTree
		{
		public:
			QuadTree(Vector2 size, int maxDepth = 6, int maxSize = 5)
			{
				root = QuadTreeNode<T>(Vector2(), size);
				this->maxDepth	= maxDepth;
				this->maxSize	= maxSize;
			}
			~QuadTree() = default;
			
			void Insert(T object, const Vector3& pos, const Vector3& size)
			{
				root.Insert(object, pos, size, maxDepth, maxSize);
			}

			void DebugDraw()
			{
				root.DebugDraw();
			}

			void OperateOnContents(typename QuadTreeNode<T>::QuadTreeFunction function)
			{
				root.OperateOnContents(function);
			}
		protected:
			QuadTreeNode<T> root; // root node of the tree
			int maxDepth; // maximum depth tree can reach 
			int maxSize; // maximum size limit of how many objects should be contained within each node.
		};
	}
}