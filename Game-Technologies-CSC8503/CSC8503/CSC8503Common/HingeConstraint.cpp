#include "HingeConstraint.h"
#include "../../Common/Vector3.h"
#include "GameObject.h"
#include "Debug.h"
#include "../../Common/Maths.h"

using namespace NCL;
using namespace NCL::Maths;
using namespace CSC8503;

HingeConstraint::HingeConstraint(GameObject* a, GameObject* b)
{
	objectA = a;
	objectB = b;
}

HingeConstraint::~HingeConstraint()
{
}

void HingeConstraint::UpdateConstraint(float dt)
{
	Vector3 relativePosition = objectA->GetConstTransform().GetWorldPosition() - objectB->GetConstTransform().GetWorldPosition();
	relativePosition.y = 0;
	const Vector3 direction = relativePosition.Normalised();

	objectA->GetTransform().SetLocalOrientation(Quaternion::EulerAnglesToQuaternion(0, RadiansToDegrees(atan2f(-direction.z, direction.x)), 0)); // atan2 gives angle between x and z axis in radians
	objectB->GetTransform().SetLocalOrientation(objectA->GetTransform().GetLocalOrientation());
}