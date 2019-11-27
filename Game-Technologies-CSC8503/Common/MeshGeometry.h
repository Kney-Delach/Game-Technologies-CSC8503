#pragma once
#include <vector>
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
using std::vector;

namespace NCL {
	namespace Maths {
		class Vector2;
		class Vector3;
		class Vector4;
		class Matrix4;
	}
	using namespace Maths;

	enum class GeometryPrimitive {
		Points,
		Lines,
		Triangles,
		TriangleFan,
		TriangleStrip,
		Patches
	};

	class MeshGeometry
	{
	public:		
		virtual ~MeshGeometry();

		GeometryPrimitive GetPrimitiveType() const {
			return primType;
		}

		void SetPrimitiveType(GeometryPrimitive type) {
			primType = type;
		}

		unsigned int GetVertexCount() const {
			return (unsigned int)positions.size();
		}

		unsigned int GetIndexCount()  const {
			return (unsigned int)indices.size();
		}

		const vector<Vector3>&		GetPositionData()		const { return positions;	}
		const vector<Vector2>&		GetTextureCoordData()	const { return texCoords;	}
		const vector<Vector4>&		GetColourData()			const { return colours;		}
		const vector<Vector3>&		GetNormalData()			const { return normals;		}
		const vector<Vector3>&		GetTangentData()		const { return tangents;	}
		const vector<unsigned int>& GetIndexData()			const { return indices;		}

		void SetVertexPositions(const vector<Vector3>& newVerts);
		void SetVertexTextureCoords(const vector<Vector2>& newTex);

		void SetVertexColours(const vector<Vector4>& newColours);
		void SetVertexNormals(const vector<Vector3>& newNorms);
		void SetVertexTangents(const vector<Vector3>& newTans);
		void SetVertexIndices(const vector<unsigned int>& newIndices);


		void	TransformVertices(const Matrix4& byMatrix);

		void RecalculateNormals();
		void RecalculateTangents();

		virtual void UploadToGPU() = 0;

	protected:
		MeshGeometry();
		MeshGeometry(const std::string&filename);

		GeometryPrimitive	primType;
		vector<Vector3>		positions;

		vector<Vector2>		texCoords;
		vector<Vector4>		colours;
		vector<Vector3>		normals;
		vector<Vector3>		tangents;
		vector<unsigned int>	indices;
	};
}