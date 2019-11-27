#pragma once
#include "CollisionDetection.h"
#include "OBBVolume.h"
namespace NCL {
	class OBBVolume;
	namespace CSC8503 {
		class Transform;
		class SATAlgorithm
		{
		public:
			static bool BoundingBoxSAT(const NCL::OBBVolume& volumeA, const Transform& worldTransformA,
				const NCL::OBBVolume& volumeB, const Transform& worldTransformB, CollisionDetection::CollisionInfo& collisionInfo);

			static void OBBSupport(Vector3& min, Vector3& max, const Vector3& objectPos, const Vector3& axis);
		private:
			SATAlgorithm();
			~SATAlgorithm();
		};
	}
}

