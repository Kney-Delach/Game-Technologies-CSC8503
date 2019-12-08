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

			void Insert(T& object, const Vector3& objectPos, const Vector3& objectSize, int depthLeft, int maxSize)
			{
			}

			void Split()
			{
			}

			void DebugDraw()
			{
			}

			void OperateOnContents(QuadTreeFunction& func)
			{
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

			void OperateOnContents(typename QuadTreeNode<T>::QuadTreeFunction  func)
			{
				root.OperateOnContents(func);
			}
		protected:
			QuadTreeNode<T> root;
			int maxDepth;
			int maxSize;
		};
	}
}