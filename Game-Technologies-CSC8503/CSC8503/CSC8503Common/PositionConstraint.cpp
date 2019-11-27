#include "PositionConstraint.h"
#include "../../Common/Vector3.h"
#include "GameObject.h"
#include "Debug.h"

using namespace NCL;
using namespace NCL::Maths;
using namespace CSC8503;

PositionConstraint::PositionConstraint(GameObject* a, GameObject* b, float d)
{
	objectA		= a;
	objectB		= b;
	distance	= d;
}

PositionConstraint::~PositionConstraint()
{

}

//a simple constraint that stops objects from being more than <distance> away
//from each other...this would be all we need to simulate a rope, or a ragdoll
void PositionConstraint::UpdateConstraint(float dt)	{
}
