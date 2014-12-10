#pragma once

#include "Common.h"

class CubeTexture
{
public:
    IDirect3DCubeTexture9 * cubeTexture;
    explicit CubeTexture( string fn );
    virtual ~CubeTexture();
    static CubeTexture * Require( string fn );
    static map< string, CubeTexture*> all;
};