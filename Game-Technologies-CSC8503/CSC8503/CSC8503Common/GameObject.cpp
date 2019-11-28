/***************************************************************************
* Filename		: GameObject.cpp
* Name			: Ori Lazar
* Date			: 28/11/2019
* Description	: Contains the implementations of the gameobjects. 
    .---.
  .'_:___".
  |__ --==|
  [  ]  :[|
  |__| I=[|
  / / ____|
 |-/.____.'
/___\ /___\
***************************************************************************/
#include "GameObject.h"
#include "CollisionDetection.h"

#include "Debug.h"

using namespace NCL::CSC8503;

GameObject::GameObject(string objectName)	{
	name			= objectName;
	layer.SetLayerID(0);
	isActive		= true;
	boundingVolume	= nullptr;
	physicsObject	= nullptr;
	renderObject	= nullptr;
	networkObject	= nullptr;
}

GameObject::~GameObject()	{
	delete boundingVolume;
	delete physicsObject;
	delete renderObject;
	delete networkObject;
}

bool GameObject::GetBroadphaseAABB(Vector3&outSize) const {
	if (!boundingVolume) {
		return false;
	}
	outSize = broadphaseAABB;
	return true;
}

// These would be better as a virtual 'ToAABB' type function, really...
void GameObject::UpdateBroadphaseAABB() {
	if (!boundingVolume) {
		return;
	}
	if (boundingVolume->type == VolumeType::AABB) {
		broadphaseAABB = ((AABBVolume&)*boundingVolume).GetHalfDimensions();
	}
	else if (boundingVolume->type == VolumeType::Sphere) {
		float r = ((SphereVolume&)*boundingVolume).GetRadius();
		broadphaseAABB = Vector3(r, r, r);
	}
	else if (boundingVolume->type == VolumeType::OBB) {
		Matrix3 mat = Matrix3(transform.GetWorldOrientation());
		mat = mat.Absolute();
		Vector3 halfSizes = ((OBBVolume&)*boundingVolume).GetHalfDimensions();
		broadphaseAABB = mat * halfSizes;
	}
}

// 28.11.2019 
void GameObject::DrawDebug(const Vector4& color)
{
	renderObject->SetColour(color);
}
//28.11.2019 
// draws a line between two gameobjects, with the color of the object line is going to 
void GameObject::DrawLineBetweenObjects(const GameObject* from, const GameObject* to)
{
	Debug::DrawLine(from->GetConstTransform().GetWorldPosition(), to->GetConstTransform().GetWorldPosition(), to->GetRenderObject()->GetColour());
}