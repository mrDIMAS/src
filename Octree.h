/*******************************************************************************
*                               Ruthenium Engine                               *
*            Copyright (c) 2013-2016 Stepanov Dmitriy aka mrDIMAS              *
*                                                                              *
* This file is part of Ruthenium Engine.                                      *
*                                                                              *
* Ruthenium Engine is free software: you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as published by  *
* the Free Software Foundation, either version 3 of the License, or            *
* (at your option) any later version.                                          *
*                                                                              *
* Ruthenium Engine is distributed in the hope that it will be useful,         *
* but WITHOUT ANY WARRANTY; without even the implied warranty of               *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                *
* GNU Lesser General Public License for more details.                          *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with Ruthenium Engine.  If not, see <http://www.gnu.org/licenses/>.   *
*                                                                              *
*******************************************************************************/

#pragma once


#include "Mesh.h"
#include "AABB.h"

//#define _OCTREE_DEBUG

class Octree {
public:
    class Triangle : public Mesh::Triangle {
    public:
        bool mRendered;
        explicit Triangle( unsigned short _a, unsigned short _b, unsigned short _c );
    };

    class Node {
    private:
        AABB mAABB;
        bool mSplit;
        vector< Triangle* > mTriangles;
        Node * mChild[ 8 ];
    public:
        friend class Octree;
        explicit Node();
        virtual ~Node();
        void AddTriangle( Triangle * t );
        void Split( );
    };

    Node * mRoot;
    Mesh * mMesh;
    int mSplitLimit;
    vector< Triangle * > mTriangleList;
    vector< Mesh::Triangle > mVisibleTriangleList;
    int mVisibleNodeCount;
    int mVisibleTriangleCount;
	ruVector3 GetAABBMin( vector< ruVector3 > & vertices );
	ruVector3 GetAABBMax( vector< ruVector3 > & vertices );
public:
    explicit Octree( Mesh * m, int _nodeSplitCriteria = 64 );
    virtual ~Octree();
    void PrepareTriangles( Node * node );
    void Build( vector< ruVector3 > & vertices, vector< Triangle* > & tris, Node * node );
    bool CubeInFrustum( const AABB & box );
    void GetVisibleTrianglesList( Node * node, vector< Mesh::Triangle > & triangles );
    vector< Mesh::Triangle > & GetTrianglesToRender(  );
    void NodeVisualize( Node * node );
    void VisualizeHierarchy();
};