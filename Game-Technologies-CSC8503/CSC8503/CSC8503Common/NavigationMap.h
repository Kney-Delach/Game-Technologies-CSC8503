/***************************************************************************
* Filename		: NavigationMap.h
* Name			: Ori Lazar
* Date			: 07/12/2019
* Description	: Abstract class representing anything that can provide a navigation path.
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
#include "NavigationPath.h"
namespace NCL
{
	using namespace Maths;
	
	namespace CSC8503
	{
		class NavigationMap
		{
		public:
			NavigationMap() = default;
			virtual ~NavigationMap() = default;

			virtual bool FindPath(const Vector3& from, const Vector3& to, NavigationPath& outPath) = 0;
		};
	}
}