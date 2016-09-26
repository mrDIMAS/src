#pragma once

#include "Common.h"
#include "Vertex.h"

struct Material {
	string diffuse;
	string normal;
	float opacity;
};

struct BoneWeight {
	float weight;
	int id;
};

struct Weight {
	BoneWeight bones[4];
	int boneCount;
};

class Mesh {
public:
	vector< Vertex > vertices;
	vector< unsigned short > indices;
	vector< Weight > weights;
	Vector3 min, max, center;
	float radius;
	Material mat;

	Mesh();
	~Mesh() {

	}
	void AddVertex(const Vertex & v, const Weight & w);
	void AddVertex(const Vertex & v);
	void CalculateAABB();
	void CalculateNormals();
	void CalculateTangent();
};