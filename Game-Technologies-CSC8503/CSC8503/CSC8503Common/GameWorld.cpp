#include "GameWorld.h"
#include "GameObject.h"
#include "Constraint.h"
#include "CollisionDetection.h"
#include "../../Common/Camera.h"
#include <algorithm>

#include "WorldMacros.h"

using namespace NCL;
using namespace NCL::CSC8503;

GameWorld::GameWorld()
{
	mainCamera = new Camera();

	quadTree = nullptr;

	shuffleConstraints	= false;
	shuffleObjects		= false;

	// initially, layer 0 can interact with layer 0, layer 1 can interact with layer 1
	std::vector<Layer> layerConfig0;
	const Layer layer0;
	layerConfig0.emplace_back(layer0);
	std::vector<Layer> layerConfig1;
	Layer layer1;
	layer1.SetLayerID(1);
	layerConfig0.emplace_back(layer1);
	worldLayers.emplace_back(layerConfig0);
	worldLayers.emplace_back(layerConfig1);
}

GameWorld::~GameWorld()
{
}

void GameWorld::Clear()
{
	gameObjects.clear();
	constraints.clear();
}

void GameWorld::ClearAndErase()
{
	for (auto& i : gameObjects) 
	{
		delete i;
	}
	for (auto& i : constraints) 
	{
		delete i;
	}
	Clear();
}

void GameWorld::AddGameObject(GameObject* o) {
	gameObjects.emplace_back(o);
}

void GameWorld::RemoveGameObject(GameObject* o) {
	std::remove(gameObjects.begin(), gameObjects.end(), o);
}

void GameWorld::GetObjectIterators(GameObjectIterator& first,GameObjectIterator& last) const
{
	first	= gameObjects.begin();
	last	= gameObjects.end();
}

void GameWorld::OperateOnContents(GameObjectFunc f)
{
	for (GameObject* g : gameObjects) 
	{
		f(g);
	}
}

void GameWorld::UpdateWorld(float dt)
{
	UpdateTransforms();
	if (shuffleObjects) 
	{
		std::random_shuffle(gameObjects.begin(), gameObjects.end());
	}
	if (shuffleConstraints) 
	{
		std::random_shuffle(constraints.begin(), constraints.end());
	}
}

void GameWorld::UpdateTransforms()
{
	for (auto& i : gameObjects) 
	{
		i->GetTransform().UpdateMatrices();
	}
}

void GameWorld::UpdateQuadTree()
{
	delete quadTree;

	//quadTree = new QuadTree<GameObject*>(Vector2(512, 512), 6);

	//for (auto& i : gameObjects) {
	//	quadTree->Insert(i);
	//}
}

// 1.12.19
// draws debug physics volumes 
void GameWorld::DrawBoundingVolumes() const
{
	for (auto& i : gameObjects)
	{
		i->DrawDebugVolume();
	}
}

bool GameWorld::Raycast(Ray& r, RayCollision& closestCollision, bool closestObject) const
{
	//The simplest raycast just goes through each object and sees if there's a collision
	RayCollision collision;
	for (auto& i : gameObjects) 
	{
		if (!i->GetBoundingVolume())  // objects must have a bounding volume to be collidable
			continue;
		
		const unsigned layerID = i->GetLayer().GetConstLayerID();
		bool found = false;
		for(unsigned i = 0; i < worldLayers[layerID].size(); i++)  // 28.11.19 - if object is on a layer that can't be raycasted against, then continue... (raycast layer is 0)
		{
			if (worldLayers[layerID][i].GetConstLayerID() == RAYCAST_LAYER_ID)
			{
				found = true;
				break;
			}
		}
		if (!found)
			continue;
		
		RayCollision thisCollision;
		if (CollisionDetection::RayIntersection(r, *i, thisCollision)) 
		{
			if (!closestObject) 
			{
				closestCollision		= collision;
				closestCollision.node = i;
				return true;
			}
			if (thisCollision.rayDistance < collision.rayDistance) 
			{
				thisCollision.node = i;
				collision = thisCollision;
			}
		}
	}
	if (collision.node) 
	{
		closestCollision		= collision;
		closestCollision.node	= collision.node;
		return true;
	}
	return false;
}


/*
Constraint Tutorial Stuff
*/
void GameWorld::AddConstraint(Constraint* c)
{
	constraints.emplace_back(c);
}

void GameWorld::RemoveConstraint(Constraint* c)
{
	std::remove(constraints.begin(), constraints.end(), c);
}

void GameWorld::GetConstraintIterators(std::vector<Constraint*>::const_iterator& first,	std::vector<Constraint*>::const_iterator& last) const
{
	first	= constraints.begin();
	last	= constraints.end();
}

unsigned GameWorld::UpdateWorldLayers(unsigned layerIndex, const std::vector<Layer>& newConfig)
{
	if (!(worldLayers.size() > layerIndex)) 
	{
		// layer index doesn't exist yet, create it at the next available index
		worldLayers.emplace_back(newConfig);
		return (worldLayers.size() - 1);
	}
	worldLayers[layerIndex].clear();
	worldLayers[layerIndex] = newConfig;
	return layerIndex; 
}