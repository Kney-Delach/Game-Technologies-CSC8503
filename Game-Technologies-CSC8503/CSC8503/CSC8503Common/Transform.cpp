#include "Transform.h"

using namespace NCL::CSC8503;

Transform::Transform()
{
	parent		= nullptr;
	localScale	= Vector3(1, 1, 1);
}

Transform::Transform(const Vector3& position, Transform* p) {
	parent = p;
	SetWorldPosition(position);
}

Transform::~Transform()
{
}

void Transform::UpdateMatrices() {
	localMatrix = 
		Matrix4::Translation(localPosition) * 
		Matrix4(localOrientation) *
		Matrix4::Scale(localScale);

	if (parent) {
		worldMatrix			= parent->GetWorldMatrix() * localMatrix;
		worldOrientation	= parent->GetWorldOrientation() * localOrientation;
	}
	else {
		worldMatrix			= localMatrix;
		worldOrientation	= localOrientation;
	}
}

void Transform::SetWorldPosition(const Vector3& worldPos) {
	if (parent) {
		Vector3 parentPos = parent->GetWorldMatrix().GetPositionVector();
		Vector3 posDiff = parentPos - worldPos;

		localPosition = posDiff;
		localMatrix.SetPositionVector(posDiff);
	}
	else {
		localPosition = worldPos;

		worldMatrix.SetPositionVector(worldPos);
	}
}

void Transform::SetLocalPosition(const Vector3& localPos) {
	localPosition = localPos;
}

void Transform::SetWorldScale(const Vector3& worldScale) {
	if (parent) {

	}
	else {
		localScale = worldScale;
	}
}

void Transform::SetLocalScale(const Vector3& newScale) {
	localScale = newScale;
}