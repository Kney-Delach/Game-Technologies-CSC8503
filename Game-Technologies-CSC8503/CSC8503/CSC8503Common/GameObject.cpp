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
#include "FloatToString.h"

using namespace NCL::CSC8503;

//todo: implement this
void GameObject::DebugDraw()
{
	DrawDebugVolume(); 
	
	const std::string msg = "Selected GameObject: " + name;
	Debug::Print(msg, Vector2(5, 700), Vector4(0,0,0,1));

	const std::string val = GetPhysicsObject()->IsStatic() ? "True" : "false";
	const std::string staticMsg = "Is Static: " + val;
	Debug::Print(staticMsg, Vector2(5, 650), Vector4(0, 0, 0, 1));

	std::string physicsMSG = "Physics Volume: ";

	if (!boundingVolume)
	{
		physicsMSG += "No Volume;";
	}
	else
	{
		switch (boundingVolume->type)
		{
			case VolumeType::AABB:
			{
				physicsMSG += "AABB";
				break;
			}
			case VolumeType::OBB:
			{
				physicsMSG += "OBB";
				break;
			}
			case VolumeType::Sphere:
			{
				physicsMSG += "SPHERE";
				break;
			}
		}
	}
	Debug::Print(physicsMSG, Vector2(5, 600), Vector4(0, 0, 0, 1));
	
	// world position
	const Vector3 pos = GetConstTransform().GetWorldPosition();
	const std::string worldPos = "World Position: (" + FloatToString(pos.x, 2) + ", " + FloatToString(pos.y, 2) + ", " + FloatToString(pos.z, 2) + ")";
	Debug::Print(worldPos, Vector2(5, 550), Vector4(0, 0, 0, 1));

	// rotation
	const Quaternion rot = GetConstTransform().GetWorldOrientation();
	const std::string worldOrientation = "World Orientation: (" + FloatToString(rot.x, 2) + ", " + FloatToString(rot.y, 2) + ", " + FloatToString(rot.z, 2) + ")";
	Debug::Print(worldOrientation, Vector2(5, 500), Vector4(0, 0, 0, 1));
}

GameObject::GameObject(string objectName)
{
	name = objectName;
	layer.SetLayerID(0);
	isActive = true;
	boundingVolume = nullptr;
	physicsObject = nullptr;
	renderObject = nullptr;
	networkObject = nullptr;
}

GameObject::~GameObject()
{
	delete boundingVolume;
	delete physicsObject;
	delete renderObject;
	delete networkObject;
}

// 8.12.2019
bool GameObject::GetBroadphaseAABB(Vector3& outSize) const
{
	if (!boundingVolume) 
	{
		return false;
	}
	outSize = broadphaseAABB;
	return true;
}

// 8.12.2019
// These would be better as a virtual 'ToAABB' type function
void GameObject::UpdateBroadphaseAABB()
{
	if (!boundingVolume) 
	{
		return;
	}
	if (boundingVolume->type == VolumeType::AABB) 
	{
		broadphaseAABB = ((AABBVolume&)*boundingVolume).GetHalfDimensions();
	}
	else if (boundingVolume->type == VolumeType::Sphere) 
	{
		float r = ((SphereVolume&)*boundingVolume).GetRadius();
		broadphaseAABB = Vector3(r, r, r);
	}
	else if (boundingVolume->type == VolumeType::OBB) 
	{
		Matrix3 mat = Matrix3(transform.GetWorldOrientation());
		mat = mat.Absolute();
		Vector3 halfSizes = ((OBBVolume&)*boundingVolume).GetHalfDimensions();
		broadphaseAABB = mat * halfSizes;
	}
}

// 6.12.2019
//todo: change this to a circulating effect
void GameObject::UpdateObjectPosition(float dt, const Vector3& parentPosition, const unsigned index)
{
	this->GetTransform().SetWorldPosition(parentPosition + Vector3(0.f, 2.f * (static_cast<float>(index) + 1.f), 0.f));
}

// 6.12.2019
void GameObject::UpdateIslandStoredObjectPosition(const Vector3& parentPosition, const unsigned row, const unsigned index)
{
	this->GetTransform().SetWorldPosition(parentPosition + Vector3(3.f * row - 6.f, 2.f, index * 2.5f - 6.f));
}

void GameObject::DrawDebugVolume()
{
	if (!boundingVolume) 		
		return;

	if (boundingVolume->type == VolumeType::AABB) 
	{
		boundingVolume->DrawDebug(transform.GetWorldPosition(), Vector4(1, 0, 0, 1));
	}
	else if (boundingVolume->type == VolumeType::Sphere) 
	{
		boundingVolume->DrawDebug(transform.GetWorldPosition(), Vector4(0, 1, 0, 1));
	}
	else if (boundingVolume->type == VolumeType::OBB)
	{
		((OBBVolume*)boundingVolume)->DrawDebugOBB(transform.GetWorldPosition(), transform.GetWorldOrientation(), Vector4(0, 0, 1, 1));
	}
}


//28.11.2019 
// draws a line between two gameobjects, with the color of the object line is going to 
void GameObject::DrawLineBetweenObjects(const GameObject* from, const GameObject* to)
{
	Debug::DrawLine(from->GetConstTransform().GetWorldPosition(), to->GetConstTransform().GetWorldPosition(), to->GetRenderObject()->GetColour());
}

void GameObject::DrawLineInDirection(const GameObject* from, const Vector3& direction, const Vector3& offset)
{
	Debug::DrawLine(from->GetConstTransform().GetWorldPosition() + offset, from->GetConstTransform().GetWorldPosition() + offset + direction, Vector4(1.f,0.f,0.f,1.f));

}

void GameObject::DrawLineBetweenObjectsOffset(const Vector3& offset, const GameObject* from, const GameObject* to)
{
	Debug::DrawLine(from->GetConstTransform().GetWorldPosition() + offset, to->GetConstTransform().GetWorldPosition(), to->GetRenderObject()->GetColour());
}
