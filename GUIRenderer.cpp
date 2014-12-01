#include "GUIRenderer.h"
#include "Camera.h"
#include "Texture.h"
#include "Cursor.h"
#include "Vertex.h"
#include "TextRenderer.h"
#include "BitmapFont.h"

GUIRenderer * g_guiRenderer = 0;

GUIRenderer::GUIRenderer() {
    int maxLineCount = 16536;

    sizeOfRectBytes = 6 * sizeof( Vertex2D  );
    CheckDXErrorFatal( g_device->CreateVertexBuffer( sizeOfRectBytes, D3DUSAGE_DYNAMIC, D3DFVF_XYZ, D3DPOOL_DEFAULT, &vertexBuffer, 0 ));
    CheckDXErrorFatal( g_device->CreateVertexBuffer( maxLineCount * 2 * sizeof( LinePoint ), D3DUSAGE_DYNAMIC, D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &lineVertexBuffer, 0 ));

    D3DVERTEXELEMENT9 guivd[ ] = {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0  },
        D3DDECL_END()
    };

    CheckDXErrorFatal( g_device->CreateVertexDeclaration( guivd, &vertDecl ));

    D3DVERTEXELEMENT9 linevd[ ] = {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
        D3DDECL_END()
    };

    CheckDXErrorFatal( g_device->CreateVertexDeclaration( linevd, &lineDecl )) ;

    D3DVIEWPORT9 vp;
    CheckDXErrorFatal( g_device->GetViewport( &vp ));
    D3DXMatrixOrthoOffCenterLH ( &orthoMatrix, 0, vp.Width, vp.Height, 0, 0, 1024 );
}

GUIRenderer::~GUIRenderer() {
    CheckDXErrorFatal( vertexBuffer->Release());
    CheckDXErrorFatal( vertDecl->Release());
    CheckDXErrorFatal( lineDecl->Release());
    CheckDXErrorFatal( lineVertexBuffer->Release());

    for( size_t i = 0; i < fonts.size(); i++ ) {
        fonts.at( i )->Release();
    }
}

FontHandle GUIRenderer::CreateFont( int size, const char * name, int italic, int underlined ) {
	BitmapFont * font = new BitmapFont( name, size );
    FontHandle handle;
    handle.pointer = font;
    return handle;
}

void GUIRenderer::RenderRect( const GUIRect & r ) {
    rects.push( r );
}

void GUIRenderer::RenderText( const GUIText & text ) {
    auto iter = texts.find( text.font );
	if( iter == texts.end() ) {
		texts[ text.font ] = vector<GUIText>();
	}
	texts[ text.font ].push_back( text );
}

void GUIRenderer::Render3DLine( const GUILine & line ) {
    lines.push( line );
}

void GUIRenderer::DrawWireBox( LinePoint min, LinePoint max ) {
    Vector3 lmin = min.position;
    Vector3 lmax = max.position;
    Render3DLine( GUILine( LinePoint( Vector3(lmin.x, lmin.y, lmin.z), min.color ), LinePoint( Vector3(lmax.x, lmin.y, lmin.z), max.color )));
    Render3DLine( GUILine( LinePoint( Vector3(lmax.x, lmin.y, lmin.z), min.color ), LinePoint( Vector3(lmax.x, lmax.y, lmin.z), max.color )));
    Render3DLine( GUILine( LinePoint( Vector3(lmax.x, lmax.y, lmin.z), min.color ), LinePoint( Vector3(lmin.x, lmax.y, lmin.z), max.color )));
    Render3DLine( GUILine( LinePoint( Vector3(lmin.x, lmax.y, lmin.z), min.color ), LinePoint( Vector3(lmin.x, lmin.y, lmin.z), max.color )));
    Render3DLine( GUILine( LinePoint( Vector3(lmin.x, lmin.y, lmin.z), min.color ), LinePoint( Vector3(lmin.x, lmin.y, lmax.z), max.color )));
    Render3DLine( GUILine( LinePoint( Vector3(lmax.x, lmin.y, lmin.z), min.color ), LinePoint( Vector3(lmax.x, lmin.y, lmax.z), max.color )));
    Render3DLine( GUILine( LinePoint( Vector3(lmax.x, lmax.y, lmin.z), min.color ), LinePoint( Vector3(lmax.x, lmax.y, lmax.z), max.color )));
    Render3DLine( GUILine( LinePoint( Vector3(lmin.x, lmax.y, lmin.z), min.color ), LinePoint( Vector3(lmin.x, lmax.y, lmax.z), max.color )));
    Render3DLine( GUILine( LinePoint( Vector3(lmin.x, lmin.y, lmax.z), min.color ), LinePoint( Vector3(lmax.x, lmin.y, lmax.z), max.color )));
    Render3DLine( GUILine( LinePoint( Vector3(lmax.x, lmin.y, lmax.z), min.color ), LinePoint( Vector3(lmax.x, lmax.y, lmax.z), max.color )));
    Render3DLine( GUILine( LinePoint( Vector3(lmax.x, lmax.y, lmax.z), min.color ), LinePoint( Vector3(lmin.x, lmax.y, lmax.z), max.color )));
    Render3DLine( GUILine( LinePoint( Vector3(lmin.x, lmax.y, lmax.z), min.color ), LinePoint( Vector3(lmin.x, lmin.y, lmax.z), max.color )));
}

void GUIRenderer::RenderAllGUIElements() {
    // Set default shaders
    CheckDXErrorFatal( g_device->SetVertexShader( 0 ));
    CheckDXErrorFatal( g_device->SetPixelShader( 0 ));

    // Set render states
    CheckDXErrorFatal( g_device->SetRenderState( D3DRS_LIGHTING, FALSE ));

    D3DXMATRIX identity;
    D3DXMatrixIdentity( &identity );

    CheckDXErrorFatal( g_device->SetTransform( D3DTS_WORLD, &identity ));

    RenderLines();

    CheckDXErrorFatal( g_device->SetTransform( D3DTS_VIEW, &identity ));
    RenderRects();
    RenderTexts();
    if( g_cursor ) {
        RenderCursor();
    }
}

void GUIRenderer::RenderLines() {
    IDirect3DStateBlock9 * state;
    CheckDXErrorFatal( g_device->CreateStateBlock( D3DSBT_ALL, &state ) );
    CheckDXErrorFatal( g_device->SetTransform( D3DTS_VIEW, &g_camera->view ));

    void * data = nullptr;
    CheckDXErrorFatal( lineVertexBuffer->Lock( 0, 0, &data, D3DLOCK_DISCARD ));

    int linesToRender = 0;
    while( !lines.empty() ) {
        GUILine line = lines.front();

        const int pointCount = 2;

        LinePoint points[ pointCount ];
        points[ 0 ] = line.begin;
        points[ 1 ] = line.end;

        int lineBytesCount = sizeof( LinePoint ) * pointCount;

        memcpy( (char*)data + linesToRender * lineBytesCount, points, lineBytesCount );

        linesToRender++;

        lines.pop();
    }

    CheckDXErrorFatal( lineVertexBuffer->Unlock( ));
    CheckDXErrorFatal( g_device->SetVertexDeclaration( lineDecl ));
    CheckDXErrorFatal( g_device->SetStreamSource( 0, lineVertexBuffer, 0, sizeof( LinePoint )));

    if( linesToRender )
        CheckDXErrorFatal( g_device->DrawPrimitive( D3DPT_LINELIST, 0, linesToRender ));

    CheckDXErrorFatal( state->Apply() );
    state->Release();
}

void GUIRenderer::RenderTexts() {
	for( auto & kv : texts ) {
		g_textRenderer->RenderTextGroup( kv.second, kv.first );
	}

	// clear text groups
	for( auto & kv : texts ) {
		kv.second.clear();
	}	
}

void GUIRenderer::RenderRects() {
    IDirect3DStateBlock9 * state;
    CheckDXErrorFatal( g_device->CreateStateBlock( D3DSBT_ALL, &state ));

    PrepareToDraw2D();

    CheckDXErrorFatal( g_device->SetStreamSource( 0, vertexBuffer, 0, sizeof( Vertex2D )));

    void * data = nullptr;
    Vertex2D vertices[6];

    while( !rects.empty() ) {
        GUIRect rect = rects.front();

        vertices[ 0 ] = Vertex2D( rect.x,           rect.y,           0, 0, 0, rect.color );
        vertices[ 1 ] = Vertex2D( rect.x + rect.w,  rect.y,           0, 1, 0, rect.color );
        vertices[ 2 ] = Vertex2D( rect.x,           rect.y + rect.h,  0, 0, 1, rect.color );
        vertices[ 3 ] = Vertex2D( rect.x + rect.w,  rect.y,           0, 1, 0, rect.color );
        vertices[ 4 ] = Vertex2D( rect.x + rect.w,  rect.y + rect.h,  0, 1, 1, rect.color );
        vertices[ 5 ] = Vertex2D( rect.x,           rect.y + rect.h,  0, 0, 1, rect.color );

        CheckDXErrorFatal( vertexBuffer->Lock( 0, 0, &data, 0 ));
        memcpy( data, vertices, sizeOfRectBytes );
        CheckDXErrorFatal( vertexBuffer->Unlock( ));

        if( rect.texture ) {
            rect.texture->Bind( 0 );
        } else {
			g_device->SetTexture( 0, nullptr );
		}

		g_dips++;
        CheckDXErrorFatal( g_device->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 ));
        CheckDXErrorFatal( g_device->SetTexture( 0, 0 ));
        rects.pop();
    }

    CheckDXErrorFatal( state->Apply());
    CheckDXErrorFatal( state->Release());
}

void GUIRenderer::PrepareToDraw2D() {
    CheckDXErrorFatal( g_device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE ));
    CheckDXErrorFatal( g_device->SetRenderState( D3DRS_ZENABLE, FALSE ));
    CheckDXErrorFatal( g_device->SetRenderState( D3DRS_ZWRITEENABLE, FALSE ));
    CheckDXErrorFatal( g_device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE));
    CheckDXErrorFatal( g_device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA));
    CheckDXErrorFatal( g_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA));
    CheckDXErrorFatal( g_device->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 ));
    CheckDXErrorFatal( g_device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ));
    CheckDXErrorFatal( g_device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA));
    CheckDXErrorFatal( g_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA));
    CheckDXErrorFatal( g_device->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD));
    CheckDXErrorFatal( g_device->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE ));

    CheckDXErrorFatal( g_device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE ));
    CheckDXErrorFatal( g_device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE ));
    CheckDXErrorFatal( g_device->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE ));

    CheckDXErrorFatal( g_device->SetTransform( D3DTS_PROJECTION, &orthoMatrix ));

    CheckDXErrorFatal( g_device->SetVertexDeclaration( vertDecl ));
}

void GUIRenderer::RenderCursor() {
    IDirect3DStateBlock9 * state;
    CheckDXErrorFatal( g_device->CreateStateBlock( D3DSBT_ALL, &state ));

    PrepareToDraw2D();

    CheckDXErrorFatal( g_device->SetStreamSource( 0, vertexBuffer, 0, sizeof( Vertex2D )));

    int x = GetMouseX();
    int y = GetMouseY();
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
    CheckDXErrorFatal( vertexBuffer->Lock( 0, 0, &data, 0 ));
    memcpy( data, vertices, sizeOfRectBytes );
    CheckDXErrorFatal( vertexBuffer->Unlock( ));

    if( g_cursor->tex && g_cursor->visible ) {
        g_cursor->tex->Bind( 0 );
        CheckDXErrorFatal( g_device->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 ));
    }

    CheckDXErrorFatal( state->Apply());
    state->Release();
}

GUILine::GUILine( const LinePoint & theBegin, const LinePoint & theEnd ) {
    end = theEnd;
    begin = theBegin;
}

GUIText::GUIText( string theText, float theX, float theY, float theWidth, float theHeight, Vector3 theColor, int theAlpha, int theTextAlign, FontHandle theFont ) {
	rect.left = theX;
	rect.top = theY;
	rect.right = theX + theWidth;
	rect.bottom = theY + theHeight;
    text = theText;
    font = (BitmapFont *)theFont.pointer;
    textAlign = theTextAlign;
    color = D3DCOLOR_ARGB( theAlpha, (int)theColor.x, (int)theColor.y, (int)theColor.z );
}

GUIRect::GUIRect( float theX, float theY, float theWidth, float theHeight, Texture * theTexture, Vector3 theColor, int theAlpha ) {
    x = theX;
    y = theY;
    w = theWidth;
    h = theHeight;
    texture = theTexture;
    color = D3DCOLOR_ARGB( theAlpha, (int)theColor.x, (int)theColor.y, (int)theColor.z );
}
