#include "octree.h"
#include <limits>

Octree::Octree( Triangle * triangles, int triangleCount ) {
	mRoot = new OctreeNode;

	for( int i = 0; i < triangleCount; i++ ) {
		Triangle & triangle = triangles[i];
		VMinMax( mRoot->mMin, mRoot->mMax, triangle.mA );
		VMinMax( mRoot->mMin, mRoot->mMax, triangle.mB );
		VMinMax( mRoot->mMin, mRoot->mMax, triangle.mC );
	}

	vector<int> faceIndices;
	faceIndices.reserve( triangleCount );
	for( int i = 0; i < triangleCount; i++ ) {
		faceIndices.push_back( i );
	}

	BuildRecursive( mRoot, triangles, triangleCount, faceIndices );

	mTraceBuffer = new OctreeNode*[ GetLeafCount() ];
	mTraceBufferSize = 0;
}

void Octree::TracePointRecusive( OctreeNode * node, const Vector3 & point ) {
	if( node->IsContainPoint( point )) {
		if( node->mSplit ) {
			for( int i = 0; i < 8; i++ ) {
				TracePointRecusive( node->mChildren[i], point );
			}
		} else {
			mTraceBuffer[ mTraceBufferSize++ ] = node;
		}
	}
}

void Octree::BuildRecursive( OctreeNode * node, Triangle * triangles, int triangleCount, const vector<int> & faceIndices ) {
	if( faceIndices.size() < OCTREE_INDEX_PER_LEAF ) {
		node->mIndices = std::move( faceIndices );
	} else {
		node->Split();

		for( int childNum = 0; childNum < 8; childNum++ ) {
			OctreeNode * child = node->mChildren[childNum];

			vector<int> leafIndices;
			leafIndices.reserve( triangleCount );
			for( int i = 0; i < triangleCount; i++ ) {
				if( triangles[i].IsIntersectAABB( child->mMin, child->mMax )) {
					leafIndices.push_back( i );				
				}			
			}

			BuildRecursive( child, triangles, triangleCount, leafIndices );
		}
	}
}

Octree::~Octree() {
	delete mRoot;
	delete [] mTraceBuffer;
}

int Octree::GetLeafCount() {
	int counter = 0;
	CountLeafsRecursive( mRoot, counter );
	return counter;
}

void Octree::CountLeafsRecursive( OctreeNode * node, int & counter ) {
	if( node->mSplit ) {
		for( int i = 0; i < 8; i++ ) {
			CountLeafsRecursive( node->mChildren[i], counter );
		}
	} else {
		++counter;
	}
}

void Octree::TracePoint( const Vector3 & point ) {
	mTraceBufferSize = 0;
	TracePointRecusive( mRoot, point );
}

bool OctreeNode::IsContainPoint( const Vector3 & point ) {
	return	point.x >= mMin.x && point.x <= mMax.x &&
		point.y >= mMin.y && point.y <= mMax.y &&
		point.z >= mMin.z && point.z <= mMax.z;
}

void OctreeNode::Split() {
	Vector3 center = (mMax + mMin) * 0.5f;

	for(int i = 0; i < 8; i++) {
		mChildren[i] = new OctreeNode;
	}
		
	mChildren[0]->mMin = Vector3( mMin.x, mMin.y, mMin.z );
	mChildren[0]->mMax = Vector3( center.x, center.y, center.z );

	mChildren[1]->mMin = Vector3( center.x, mMin.y, mMin.z );
	mChildren[1]->mMax = Vector3( mMax.x, center.y, center.z );

	mChildren[2]->mMin = Vector3( mMin.x, mMin.y, center.z );
	mChildren[2]->mMax = Vector3( center.x, center.y, mMax.z );

	mChildren[3]->mMin = Vector3( center.x, mMin.y, center.z );
	mChildren[3]->mMax = Vector3( mMax.x, center.y, mMax.z );

	mChildren[4]->mMin = Vector3( mMin.x, center.y, mMin.z );
	mChildren[4]->mMax = Vector3( center.x, mMax.y, center.z );

	mChildren[5]->mMin = Vector3( center.x, center.y, mMin.z );
	mChildren[5]->mMax = Vector3( mMax.x, mMax.y, center.z );

	mChildren[6]->mMin = Vector3( mMin.x, center.y, center.z );
	mChildren[6]->mMax = Vector3( center.x, mMax.y, mMax.z );

	mChildren[7]->mMin = Vector3( center.x, center.y, center.z );
	mChildren[7]->mMax = Vector3( mMax.x, mMax.y, mMax.z );
	
	mSplit = true;
}

OctreeNode::OctreeNode() : mMin( FLT_MAX, FLT_MAX, FLT_MAX ), mMax( -FLT_MAX, -FLT_MAX, -FLT_MAX ), mSplit( false )
{
	for( int i = 0; i < 8; i++ ) {
		mChildren[i] = nullptr;
	}
}

OctreeNode::~OctreeNode() {
	for( int i = 0; i < 8; i++ ) {
		delete mChildren[i];
	}
}

void VMinMax( Vector3 & vMin, Vector3 & vMax, const Vector3 & a )
{
	if( a.x < vMin.x ) vMin.x = a.x;
	if( a.y < vMin.y ) vMin.y = a.y;
	if( a.z < vMin.z ) vMin.z = a.z;

	if( a.x > vMax.x ) vMax.x = a.x;
	if( a.y > vMax.y ) vMax.y = a.y;
	if( a.z > vMax.z ) vMax.z = a.z;
}
