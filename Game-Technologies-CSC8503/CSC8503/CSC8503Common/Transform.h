#pragma once
#include "../../Common/Matrix4.h"
#include "../../Common/Matrix3.h"
#include "../../Common/Vector3.h"
#include "../../Common/Quaternion.h"

#include <vector>

using std::vector;

using namespace NCL::Maths;

namespace NCL {
	namespace CSC8503 {
		class Transform
		{
		public:
			Transform();
			Transform(const Vector3& position, Transform* parent = nullptr);
			~Transform();

			void SetWorldPosition(const Vector3& worldPos);
			void SetLocalPosition(const Vector3& localPos);

			void SetWorldScale(const Vector3& worldScale);
			void SetLocalScale(const Vector3& localScale);

			Transform* GetParent() const {
				return parent;
			}

			void SetParent(Transform* newParent) {
				parent = newParent;
			}

			Matrix4 GetWorldMatrix() const {
				return worldMatrix;
			}

			Matrix4 GetLocalMatrix() const {
				return localMatrix;
			}

			Vector3 GetWorldPosition() const {
				return worldMatrix.GetPositionVector();
			}

			Vector3 GetLocalPosition() const {
				return localPosition;
			}

			Vector3 GetLocalScale() const {
				return localScale;
			}

			Quaternion GetLocalOrientation() const {
				return localOrientation;
			}

			void SetLocalOrientation(const Quaternion& newOr) {
				localOrientation = newOr;
			}

			Quaternion GetWorldOrientation() const {
				return worldOrientation;
			}

			Matrix3 GetInverseWorldOrientationMat() const {
				return worldOrientation.Conjugate().ToMatrix3();
			}

			void UpdateMatrices();

		protected:
			Matrix4		localMatrix;
			Matrix4		worldMatrix;

			Vector3		localPosition;
			Vector3		localScale;
			Quaternion	localOrientation;
			Quaternion  worldOrientation;

			Transform*	parent;

			vector<Transform*> children;
		};
	}
}

