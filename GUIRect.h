#pragma once

#include "GUINode.h"
#include "Vertex.h"

class Texture;

class GUIRect : public GUINode {
public:
    static vector<GUIRect*> msRectList;

    explicit GUIRect( float theX, float theY, float theWidth, float theHeight, Texture * theTexture, ruVector3 theColor, int theAlpha, bool selfRegister );
    explicit GUIRect();
    virtual ~GUIRect();

    void GetSixVertices( Vertex2D * vertices );
};