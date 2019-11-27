#include "MeshGeometry.h"
#include "Assets.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

#include <fstream>
#include <string>

using namespace NCL;
using namespace Maths;

MeshGeometry::MeshGeometry()
{
	primType	= GeometryPrimitive::Triangles;
}

enum class GeometryChunkTypes {
	VPositions		= 1,
	VNormals		= 2,
	VTangents		= 4,
	VColors			= 8,
	VTex0			= 16,
	VTex1			= 32,
	VWeightValues	= 64,
	VWeightIndices	= 128,
	Indices			= 256,
	BindPose		= 512,
	Material		= 1024
};

enum class GeometryChunkData {
	dFloat, //Just float data
	dShort, //Translate from -32k to 32k to a float
	dByte,	//Translate from -128 to 127 to a float
};

void* ReadVertexData(GeometryChunkData dataType, GeometryChunkTypes chunkType, int numVertices) {
	int numElements = 3;

	if (chunkType == GeometryChunkTypes::VTex0 ||
		chunkType == GeometryChunkTypes::VTex1) {
		numElements = 2;
	}
	else if (chunkType == GeometryChunkTypes::VColors) {
		numElements = 4;
	}

	int bytesPerElement = 4;

	if (dataType == GeometryChunkData::dShort) {
		bytesPerElement = 2;
	}
	else if (dataType == GeometryChunkData::dByte) {
		bytesPerElement = 1;
	}

	char* data = new char[numElements * bytesPerElement];

	return data;
}

void ReadTextFloats(std::ifstream& file, vector<Vector2>& element, int numVertices) {
	for (int i = 0; i < numVertices; ++i) {
		Vector2 temp;
		file >> temp.x;
		file >> temp.y;
		element.emplace_back(temp);
	}
}

void ReadTextFloats(std::ifstream& file, vector<Vector3>& element, int numVertices) {
	for (int i = 0; i < numVertices; ++i) {
		Vector3 temp;
		file >> temp.x;
		file >> temp.y;
		file >> temp.z;
		element.emplace_back(temp);
	}
}

void ReadTextFloats(std::ifstream& file, vector<Vector4>& element, int numVertices) {
	for (int i = 0; i < numVertices; ++i) {
		Vector4 temp;
		file >> temp.x;
		file >> temp.y;
		file >> temp.z;
		file >> temp.w;
		element.emplace_back(temp);
	}
}

void ReadIndices(std::ifstream& file, vector<unsigned int>& elements, int numIndices) {
	for (int i = 0; i < numIndices; ++i) {
		unsigned int temp;
		file >> temp;
		elements.emplace_back(temp);
	}
}

MeshGeometry::MeshGeometry(const std::string&filename) {
	primType = GeometryPrimitive::Triangles;
	std::ifstream file(Assets::MESHDIR + filename);

	std::string filetype;
	int fileVersion;

	file >> filetype;

	if (filetype != "MeshGeometry") {
		std::cout << "File is not a MeshGeometry file!" << std::endl;
		return;
	}

	file >> fileVersion;

	if (fileVersion != 1) {
		std::cout << "MeshGeometry file has incompatible version!" << std::endl;
		return;
	}

	int numMeshes	= 0; //read
	int numVertices = 0; //read
	int numIndices	= 0; //read
	int numChunks   = 0; //read

	file >> numMeshes;
	file >> numVertices;
	file >> numIndices;
	file >> numChunks;
	
	for (int i = 0; i < numChunks; ++i) {
		int chunkType = (int)GeometryChunkTypes::VPositions;

		file >> chunkType;

		switch ((GeometryChunkTypes)chunkType) {
			case GeometryChunkTypes::VPositions:ReadTextFloats(file, positions, numVertices);  break;
			case GeometryChunkTypes::VColors:	ReadTextFloats(file, colours, numVertices);  break;
			case GeometryChunkTypes::VNormals:	ReadTextFloats(file, normals, numVertices);  break;
			case GeometryChunkTypes::VTangents:	ReadTextFloats(file, tangents, numVertices);  break;
			case GeometryChunkTypes::VTex0:		ReadTextFloats(file, texCoords, numVertices);  break;
			//case GeometryChunkTypes::VTex1:ReadTextFloats(file, positions, numVertices);  break;
			//case GeometryChunkTypes::VWeightValues:		ReadTextFloats(file, positions, numVertices);  break;
			//case GeometryChunkTypes::VWeightIndices:	ReadTextFloats(file, positions, numVertices);  break;
			case GeometryChunkTypes::Indices:	ReadIndices(file, indices, numIndices); break;
		}
	}
}

MeshGeometry::~MeshGeometry()
{
}

void	MeshGeometry::TransformVertices(const Matrix4& byMatrix) {

}

void	MeshGeometry::RecalculateNormals() {

}

void	MeshGeometry::RecalculateTangents() {

}

void MeshGeometry::SetVertexPositions(const vector<Vector3>& newVerts) {
	positions = newVerts;
}

void MeshGeometry::SetVertexTextureCoords(const vector<Vector2>& newTex) {
	texCoords = newTex;
}

void MeshGeometry::SetVertexColours(const vector<Vector4>& newColours) {
	colours = newColours;
}

void MeshGeometry::SetVertexNormals(const vector<Vector3>& newNorms) {
	normals = newNorms;
}

void MeshGeometry::SetVertexTangents(const vector<Vector3>& newTans) {
	tangents = newTans;
}

void MeshGeometry::SetVertexIndices(const vector<unsigned int>& newIndices) {
	indices = newIndices;
}