#pragma once


struct RaycasterFace {
	int a, b, c;
	Texture * texture;
};


class RaycasterShape {
public:
	vector<ruVector3> mVertices;
	vector<RaycasterFace> mFaces;

	RaycasterShape( vector<Mesh*> meshes ) {
		for( auto pMesh : meshes ) {
			
		}
	}
};

class Raycaster {
private:

public:

};