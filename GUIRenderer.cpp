#include "GUIRenderer.h"
#include "Camera.h"
#include "Texture.h"
#include "Cursor.h"
#include "Vertex.h"

GUIRenderer * g_guiRenderer = 0;

GUIRenderer::GUIRenderer() {
    int maxLineCount = 16536;

    sizeOfRectBytes = 6 * sizeof( Vertex2D  );
    g_device->CreateVertexBuffer( sizeOfRectBytes, D3DUSAGE_DYNAMIC, D3DFVF_XYZ, D3DPOOL_DEFAULT, &vertexBuffer, 0 );
    g_device->CreateVertexBuffer( maxLineCount * 2 * sizeof( LinePoint ), D3DUSAGE_DYNAMIC, D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &lineVertexBuffer, 0 );

    D3DVERTEXELEMENT9 guivd[ ] = {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0  },
        D3DDECL_END()
    };

    g_device->CreateVertexDeclaration( guivd, &vertDecl ) ;

    D3DVERTEXELEMENT9 linevd[ ] = {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
        D3DDECL_END()
    };

    g_device->CreateVertexDeclaration( linevd, &lineDecl ) ;

    D3DVIEWPORT9 vp;
    g_device->GetViewport( &vp );
    D3DXMatrixOrthoOffCenterLH ( &orthoMatrix, 0, vp.Width, vp.Height, 0, 0, 1024 );
}

GUIRenderer::~GUIRenderer() {
    vertexBuffer->Release();
    vertDecl->Release();
    lineDecl->Release();
    lineVertexBuffer->Release();

    for( size_t i = 0; i < fonts.size(); i++ ) {
        fonts.at( i )->Release();
    }
}

FontHandle GUIRenderer::CreateFont( int size, const char * name, int italic, int underlined ) {
    ID3DXFont * font;

    D3DXCreateFontA( g_device, size, 0 , FW_BOLD, 0, italic, RUSSIAN_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, name, &font );

    fonts.push_back( font );

    FontHandle handle;
    handle.pointer = font;

    return handle;
}

void GUIRenderer::RenderRect( const Rect & r ) {
    rects.push( r );
}

void GUIRenderer::RenderText( const Text & text ) {
    texts.push( text );
}

void GUIRenderer::Render3DLine( const Line & line ) {
    lines.push( line );
}

void GUIRenderer::DrawWireBox( LinePoint min, LinePoint max ) {
    Vector3 lmin = min.position;
    Vector3 lmax = max.position;
    Render3DLine( Line( LinePoint( Vector3(lmin.x, lmin.y, lmin.z), min.color ), LinePoint( Vector3(lmax.x, lmin.y, lmin.z), max.color )));
    Render3DLine( Line( LinePoint( Vector3(lmax.x, lmin.y, lmin.z), min.color ), LinePoint( Vector3(lmax.x, lmax.y, lmin.z), max.color )));
    Render3DLine( Line( LinePoint( Vector3(lmax.x, lmax.y, lmin.z), min.color ), LinePoint( Vector3(lmin.x, lmax.y, lmin.z), max.color )));
    Render3DLine( Line( LinePoint( Vector3(lmin.x, lmax.y, lmin.z), min.color ), LinePoint( Vector3(lmin.x, lmin.y, lmin.z), max.color )));
    Render3DLine( Line( LinePoint( Vector3(lmin.x, lmin.y, lmin.z), min.color ), LinePoint( Vector3(lmin.x, lmin.y, lmax.z), max.color )));
    Render3DLine( Line( LinePoint( Vector3(lmax.x, lmin.y, lmin.z), min.color ), LinePoint( Vector3(lmax.x, lmin.y, lmax.z), max.color )));
    Render3DLine( Line( LinePoint( Vector3(lmax.x, lmax.y, lmin.z), min.color ), LinePoint( Vector3(lmax.x, lmax.y, lmax.z), max.color )));
    Render3DLine( Line( LinePoint( Vector3(lmin.x, lmax.y, lmin.z), min.color ), LinePoint( Vector3(lmin.x, lmax.y, lmax.z), max.color )));
    Render3DLine( Line( LinePoint( Vector3(lmin.x, lmin.y, lmax.z), min.color ), LinePoint( Vector3(lmax.x, lmin.y, lmax.z), max.color )));
    Render3DLine( Line( LinePoint( Vector3(lmax.x, lmin.y, lmax.z), min.color ), LinePoint( Vector3(lmax.x, lmax.y, lmax.z), max.color )));
    Render3DLine( Line( LinePoint( Vector3(lmax.x, lmax.y, lmax.z), min.color ), LinePoint( Vector3(lmin.x, lmax.y, lmax.z), max.color )));
    Render3DLine( Line( LinePoint( Vector3(lmin.x, lmax.y, lmax.z), min.color ), LinePoint( Vector3(lmin.x, lmin.y, lmax.z), max.color )));
}

void GUIRenderer::RenderAllGUIElements() {
    // Set default shaders
    g_device->SetVertexShader( 0 );
    g_device->SetPixelShader( 0 );

    // Set render states
    g_device->SetRenderState( D3DRS_LIGHTING, FALSE );

    D3DXMATRIX identity;
    D3DXMatrixIdentity( &identity );

    g_device->SetTransform( D3DTS_WORLD, &identity );

    RenderLines();

    g_device->SetTransform( D3DTS_VIEW, &identity );
    RenderRects();
    RenderTexts();
    if( g_cursor ) {
        RenderCursor();
    }
}

void GUIRenderer::RenderLines() {
    IDirect3DStateBlock9 * state;
    g_device->CreateStateBlock( D3DSBT_ALL, &state );
    g_device->SetTransform( D3DTS_VIEW, &g_camera->view );

    void * data = nullptr;
    lineVertexBuffer->Lock( 0, 0, &data, D3DLOCK_DISCARD );

    int linesToRender = 0;
    while( !lines.empty() ) {
        Line line = lines.front();

        const int pointCount = 2;

        LinePoint points[ pointCount ];
        points[ 0 ] = line.begin;
        points[ 1 ] = line.end;

        int lineBytesCount = sizeof( LinePoint ) * pointCount;

        memcpy( (char*)data + linesToRender * lineBytesCount, points, lineBytesCount );

        linesToRender++;

        lines.pop();
    }

    lineVertexBuffer->Unlock( );
    g_device->SetVertexDeclaration( lineDecl );
    g_device->SetStreamSource( 0, lineVertexBuffer, 0, sizeof( LinePoint ));

    if( linesToRender )
        g_device->DrawPrimitive( D3DPT_LINELIST, 0, linesToRender );

    state->Apply();
    state->Release();
}

void GUIRenderer::RenderTexts() {
    while( !texts.empty() ) {
        Text & t = texts.front();

        RECT r;

        r.left = t.x;
        r.top = t.y;
        r.right = t.x + t.w;
        r.bottom = t.y + t.h;

        t.font->DrawTextA( 0, t.text.c_str(), -1, &r, DT_WORDBREAK | DT_NOCLIP | t.textAlign, t.color );

        texts.pop();
    }
}

void GUIRenderer::RenderRects() {
    IDirect3DStateBlock9 * state;
    g_device->CreateStateBlock( D3DSBT_ALL, &state );

    PrepareToDraw2D();

    g_device->SetStreamSource( 0, vertexBuffer, 0, sizeof( Vertex2D ));

    void * data = nullptr;
    Vertex2D vertices[6];

    while( !rects.empty() ) {
        Rect rect = rects.front();

        vertices[ 0 ] = Vertex2D( rect.x,           rect.y,           0, 0, 0, rect.color );
        vertices[ 1 ] = Vertex2D( rect.x + rect.w,  rect.y,           0, 1, 0, rect.color );
        vertices[ 2 ] = Vertex2D( rect.x,           rect.y + rect.h,  0, 0, 1, rect.color );
        vertices[ 3 ] = Vertex2D( rect.x + rect.w,  rect.y,           0, 1, 0, rect.color );
        vertices[ 4 ] = Vertex2D( rect.x + rect.w,  rect.y + rect.h,  0, 1, 1, rect.color );
        vertices[ 5 ] = Vertex2D( rect.x,           rect.y + rect.h,  0, 0, 1, rect.color );

        vertexBuffer->Lock( 0, 0, &data, 0 );
        memcpy( data, vertices, sizeOfRectBytes );
        vertexBuffer->Unlock( );

        if( rect.texture ) {
            rect.texture->Bind( 0 );
        } else {
			g_device->SetTexture( 0, nullptr );
		}

        g_device->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );

        g_device->SetTexture( 0, 0 );

        rects.pop();
    }

    state->Apply();
    state->Release();
}

void GUIRenderer::PrepareToDraw2D() {
    g_device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    g_device->SetRenderState( D3DRS_ZENABLE, FALSE );
    g_device->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
    g_device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
    g_device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    g_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    g_device->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );
    g_device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    g_device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    g_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    g_device->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD);
    g_device->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );

    g_device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    g_device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    g_device->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );

    g_device->SetTransform( D3DTS_PROJECTION, &orthoMatrix );

    g_device->SetVertexDeclaration( vertDecl );
}

void GUIRenderer::RenderCursor() {
    IDirect3DStateBlock9 * state;
    g_device->CreateStateBlock( D3DSBT_ALL, &state );

    PrepareToDraw2D();

    g_device->SetStreamSource( 0, vertexBuffer, 0, sizeof( Vertex2D ));

    int x = mi::MouseX();
    int y = mi::MouseY();
    int w = g_cursor->w;
    int h = g_cursor->h;
    int color = 0xFFFFFFFF;

    Vertex2D vertices[6];
    vertices[ 0 ] = Vertex2D( x,      y,      0, 0, 0, color );
    vertices[ 1 ] = Vertex2D( x + w,  y,      0, 1, 0, color );
    vertices[ 2 ] = Vertex2D( x,      y + h,  0, 0, 1, color );
    vertices[ 3 ] = Vertex2D( x + w,  y,      0, 1, 0, color );
    vertices[ 4 ] = Vertex2D( x + w,  y + h,  0, 1, 1, color );
    vertices[ 5 ] = Vertex2D( x,      y + h,  0, 0, 1, color );

    void * data = nullptr;
    vertexBuffer->Lock( 0, 0, &data, 0 );
    memcpy( data, vertices, sizeOfRectBytes );
    vertexBuffer->Unlock( );

    if( g_cursor->tex && g_cursor->visible ) {
        g_cursor->tex->Bind( 0 );
        g_device->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );
    }

    state->Apply();
    state->Release();
}

GUIRenderer::Line::Line( const LinePoint & theBegin, const LinePoint & theEnd ) {
    end = theEnd;
    begin = theBegin;
}

GUIRenderer::Text::Text( string theText, float theX, float theY, float theWidth, float theHeight, Vector3 theColor, int theAlpha, int theTextAlign, FontHandle theFont ) {
    x = theX;
    y = theY;
    w = theWidth;
    h = theHeight;
    text = theText;
    font = (ID3DXFont*)( theFont.pointer );

    if( theTextAlign == 0 ) {
        textAlign = DT_LEFT;
    }
    if( theTextAlign == 1 ) {
        textAlign = DT_CENTER | DT_VCENTER;
    }
    color = D3DCOLOR_ARGB( theAlpha, (int)theColor.x, (int)theColor.y, (int)theColor.z );
}

GUIRenderer::Rect::Rect( float theX, float theY, float theWidth, float theHeight, Texture * theTexture, Vector3 theColor, int theAlpha ) {
    x = theX;
    y = theY;
    w = theWidth;
    h = theHeight;
    texture = theTexture;
    color = D3DCOLOR_ARGB( theAlpha, (int)theColor.x, (int)theColor.y, (int)theColor.z );
}
