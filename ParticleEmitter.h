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


#include "SceneNode.h"

class ParticleEmitter : RendererComponent {
public:
    struct ParticleVertex {
        ruVector3 pos;
        ruVector2 tex;
        int color;
    };

    struct ParticleFace {
        short v1;
        short v2;
        short v3;
    };

    class Particle {
    public:
        ruVector3 position;
        ruVector3 color;
        ruVector3 speed;
        float mOpacity;
        float size;

        explicit Particle( const ruVector3 & thePosition, const ruVector3 & theSpeed, const ruVector3 & theColor, float theTranslucency, float theSize );
        Particle( );
    };

    vector< Particle > particles;

    ParticleVertex * vertices;
    ParticleFace * faces;

    IDirect3DVertexBuffer9 * vertexBuffer;
    IDirect3DIndexBuffer9 * indexBuffer;

    SceneNode * mOwner;

    int mAliveParticleCount;
	int mMaxParticleCount;

	D3DXMATRIX mWorldTransform;

    bool firstTimeUpdate;

    ruParticleSystemProperties props;

    int RGBAToInt( ruVector3 color, int alpha );
    ruVector3 RandomVector3( ruVector3 & min, ruVector3 & max );

	static vector<ParticleEmitter*> msParticleEmitters;
public:
    ParticleEmitter( SceneNode * theParent, int theParticleCount, ruParticleSystemProperties creationProps );
    ~ParticleEmitter();
	void OnResetDevice();
	void OnLostDevice();
    void ResurrectParticles();
    float GetThickness( );
    bool IsEnabled();
    bool HasAliveParticles();
    void ResurrectParticle( Particle & p );
    bool IsLightAffects( );
    SceneNode * GetOwner();
    void Update( );
    void Bind();
    void Render();
};