#pragma once

#include "Vector.h"

#define OCTREE_INDEX_PER_LEAF 192

void VMinMax( Vector3 & vMin, Vector3 & vMax, const Vector3 & a );

class Triangle {
public:
	Vector3 mA;
	Vector3 mB;
	Vector3 mC;
	Vector3 mMin;
	Vector3 mMax;




	Triangle( const Vector3 & a, const Vector3 & b, const Vector3 & c ) : 
		mA( a ), mB( b ), mC( c ), mMax( -FLT_MAX, -FLT_MAX, -FLT_MAX ), mMin( FLT_MAX, FLT_MAX, FLT_MAX ) 
	{
		VMinMax( mMin, mMax, mA );
		VMinMax( mMin, mMax, mB );
		VMinMax( mMin, mMax, mC );
	}

	bool Triangle::IsIntersectAABB( const Vector3 & bbMin, const Vector3 & bbMax ) {
		Vector3 cA = (mMax + mMin) * 0.5f;
		Vector3 rA = (mMax - mMin) * 0.5f;
		Vector3 cB = (bbMax + bbMin) * 0.5f;
		Vector3 rB = (bbMax - bbMin) * 0.5f;

		if ( fabs( cA.x - cB.x ) > (rA.x + rB.x) ) return false;
		if ( fabs( cA.y - cB.y ) > (rA.y + rB.y) ) return false;
		if ( fabs( cA.z - cB.z ) > (rA.z + rB.z) ) return false;

		return true;
	}
};

class OctreeNode {
public:
	vector<int> mIndices;
	bool mSplit;
	Vector3 mMin;
	Vector3 mMax;
	OctreeNode * mChildren[8];
	OctreeNode();
	~OctreeNode();
	void Split();
	bool IsContainPoint( const Vector3 & point );
	bool IsContainAnyPointOfTriangle( const Triangle & triangle );
};

class Octree {
private:
	void BuildRecursive( OctreeNode * node, Triangle * triangles, int triangleCount, const vector<int> & faceIndices );
	void TracePointRecusive( OctreeNode * node, const Vector3 & point );
public:
	OctreeNode * mRoot;
	Octree( Triangle * triangles, int triangleCount );
	~Octree();

	void TracePoint( const Vector3 & point );
	void CountLeafsRecursive( OctreeNode * node, int & counter );
	int GetLeafCount();

	OctreeNode ** mTraceBuffer;
	int mTraceBufferSize;
};