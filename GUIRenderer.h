#pragma once

#include "Shader.h"

class GUIRenderer
{
public:
  class Rect
  {
  private:
    float x;
    float y;
    float w;
    float h;

    Texture * texture;

    int color;

  public:
    friend class GUIRenderer;

    Rect( float theX, float theY, float theWidth, float theHeight, Texture * theTexture, Vector3 theColor, int theAlpha );
  };

  class Text
  {
  private:
    ID3DXFont * font;

    string text;

    float x;
    float y;
    float w;
    float h;

    int color;
    int textAlign;
  public:
    friend class GUIRenderer;

    Text( string theText, float theX, float theY, float theWidth, float theHeight, Vector3 theColor, int theAlpha, int theTextAlign, FontHandle theFont );
  };

  class Line
  {
  private:
    LinePoint begin;
    LinePoint end;

  public:
    friend class GUIRenderer;

    Line( const LinePoint & theBegin, const LinePoint & theEnd );
  };

private:
  IDirect3DVertexBuffer9 * vb;
  IDirect3DVertexBuffer9 * lineVB;
  IDirect3DVertexDeclaration9 * lineDecl;
  IDirect3DVertexDeclaration9 * vertDecl;
  vector<ID3DXFont*> fonts;
  queue<Text> texts;
  queue<Rect> rects;
  queue<Line> lines;
public: 
  GUIRenderer();
  ~GUIRenderer();
  FontHandle CreateFont( int size, const char * name, int italic, int underlined );
  void RenderRect( const Rect & r );
  void RenderText( const Text & text );
  void Render3DLine( const Line & line );
  void DrawWireBox( LinePoint min, LinePoint max );
  void RenderAllGUIElements();
};