#pragma once
#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"
namespace NCL {
	namespace Maths {
		class Simplex
		{
		public:

			struct SupportPoint {
				Vector3 pos;
				Vector3 onA;
				Vector3 onB;
				Vector3 realA;
				Vector3 realB;
			};

			Simplex();
			~Simplex();

			void SetToTri(SupportPoint a, SupportPoint b, SupportPoint c);

			void SetToLine(SupportPoint a, SupportPoint b);

			void Add(SupportPoint a);

			int GetSize() const {
				return size;
			}

			Vector3 GetVertex(int i) const {
				return verts[IndexToSlot(i)].pos;
			}

			SupportPoint GetSupportPoint(int i) const {
				return verts[IndexToSlot(i)];
			}

			void RemoveOldestSupportPoint();

			float GetClosestDistance();

			void DetermineBestCollisions(Vector3& onA, Vector3& onB);

			bool ExtractCollisionInfo(Vector3& normal, float& penetration);

			int OriginInsideTetrahedron();
			Vector3 ClosestPointToTetrahedron(int planeStates, Vector4& interpolates);

			Vector3 closestPointToTri(const Vector3& a, const Vector3&b, const Vector3& c);

			Vector3 BarycentricTriangleWeighting(const Vector3& a, const Vector3&b, const Vector3&c);

			Vector3 ClosestPoint();

		protected:


			int IndexToSlot(int i) const {
				int slot = (index - 1) - i;
				if (slot < 0) {
					slot += size;
				}
				return slot;
			}

			SupportPoint verts[4];
			int index;
			int size;
		};
	}
}

