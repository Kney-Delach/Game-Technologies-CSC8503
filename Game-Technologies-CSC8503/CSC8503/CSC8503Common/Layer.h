/***************************************************************************
* Filename		: Layer.h
* Name			: Ori Lazar
* Date			: 28/11/2019
* Description	: Used to determine the layer a GameObject is on,
*                 used in collision and raycasting algorithms.
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

namespace NCL
{
	namespace CSC8503
	{
		class Layer
		{
		public:
			Layer() : layerID(0) { }
			~Layer() = default;

			void SetLayerID(unsigned newID)
			{
				layerID = newID;
			}
			
			unsigned& GetLayerID()
			{
				return layerID;
			}

			const unsigned& GetConstLayerID() const
			{
				return layerID;
			}
			
			bool operator==(const Layer& other) const
			{
				if(this->GetConstLayerID() == other.GetConstLayerID())
					return true;
				return false;
			}
		private:
			unsigned layerID;
		};
	}
}