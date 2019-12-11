/***************************************************************************
* Filename		: NavigationPath.h
* Name			: Ori Lazar
* Date			: 07/12/2019
* Description	: Represents a series of Vector3s representing waypoints that an
*                 AI can move through to get from one position to another in the
*                 game world.
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
#include <vector>

namespace NCL
{
	using namespace NCL::Maths;
	
	namespace CSC8503
	{		
		class NavigationPath
		{
		public:
			NavigationPath() = default;
			~NavigationPath() = default;

			void Clear()
			{
				waypoints.clear();
				ids.clear();
			}
			void AddWaypoint(const Vector3& wayPoint) { waypoints.emplace_back(wayPoint); }
			void AddWaypointID(const int nodeID) { ids.emplace_back(nodeID); }
			bool RemoveWaypointID(int& id)
			{
				if (ids.empty())
				{
					return false;
				}
				ids.pop_back();
				if (ids.empty())
				{
					return false;
				}
				id = ids.back();
				return true;
			}
			bool RemoveWaypoint(Vector3& wayPoint)
			{
				if (waypoints.empty())
				{
					return false;
				}
				waypoints.pop_back();
				if (waypoints.empty())
				{
					return false;
				}
				wayPoint = waypoints.back();
				return true;
			}
		protected:
			std::vector<Vector3> waypoints;
			std::vector<int> ids;
		};
	}
}