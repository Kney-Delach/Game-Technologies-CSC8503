#include "HeightConstraint.h"
#include "../../Common/Vector3.h"
#include "GameObject.h"
#include "Debug.h"

using namespace NCL;
using namespace NCL::Maths;
using namespace CSC8503;

HeightConstraint::HeightConstraint(GameObject* a, float h)
{
	objectA = a;
	height = h;
}

HeightConstraint::~HeightConstraint()
{
}

void HeightConstraint::UpdateConstraint(float dt)
{
	Vector3 newPos = objectA->GetTransform().GetWorldPosition();
	newPos.y = height;
	
	objectA->GetTransform().SetWorldPosition(newPos);
	objectA->GetTransform().SetLocalPosition(newPos);
}