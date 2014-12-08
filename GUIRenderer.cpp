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
    CheckDXErrorFatal( g_device->CreateVertexBuffer( maxLineCount * 2 * sizeof( ruLinePoint ), D3DUSAGE_DYNAMIC, D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &lineVertexBuffer, 0 ));

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
    D3DXMatrixOrthoOffCenterLH ( &orthoMatrix, 0, vp.Width, vp.Height, 0, 0.0f, 1024.0f );

	string vertexShaderSource = 
		"float4x4 gProj;\n"

		"struct VSOutput {\n"
		"	float4 position : POSITION;\n"
		"	float2 texCoord : TEXCOORD0;\n"
		"   float4 color : TEXCOORD1;\n"
		"};\n"

		"VSOutput main( float4 position : POSITION, float2 texCoord : TEXCOORD0, float4 color : COLOR0 ) {\n "
		"	VSOutput output;\n"
		"	output.position = mul( position, gProj );\n"
		"	output.texCoord = texCoord;\n"
		"	output.color = color;"
		"	return output;\n"
		"};\n";

	vertexShader = new VertexShader( vertexShaderSource );

	vProj = vertexShader->GetConstantTable()->GetConstantByName( 0, "gProj" );

	string pixelShaderSource = 
		"sampler diffuse : register( s0 );\n"
		"float4 main( float2 texCoord : TEXCOORD0, float4 color : TEXCOORD1 ) : COLOR0 {\n"
		"	return color * tex2D( diffuse, texCoord );\n"			
		"};\n";

	pixelShader = new PixelShader( pixelShaderSource );
}

GUIRenderer::~GUIRenderer() {
	delete vertexShader;
	delete pixelShader;
    CheckDXErrorFatal( vertexBuffer->Release());
    CheckDXErrorFatal( vertDecl->Release());
    CheckDXErrorFatal( lineDecl->Release());
    CheckDXErrorFatal( lineVertexBuffer->Release());
}

ruFontHandle GUIRenderer::CreateFont( int size, const char * name, int italic, int underlined ) {
	BitmapFont * font = new BitmapFont( name, size );
    ruFontHandle handle;
    handle.pointer = font;
    return handle;
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

void GUIRenderer::DrawWireBox( ruLinePoint min, ruLinePoint max ) {
    ruVector3 lmin = min.position;
    ruVector3 lmax = max.position;
    Render3DLine( GUILine( ruLinePoint( ruVector3(lmin.x, lmin.y, lmin.z), min.color ), ruLinePoint( ruVector3(lmax.x, lmin.y, lmin.z), max.color )));
    Render3DLine( GUILine( ruLinePoint( ruVector3(lmax.x, lmin.y, lmin.z), min.color ), ruLinePoint( ruVector3(lmax.x, lmax.y, lmin.z), max.color )));
    Render3DLine( GUILine( ruLinePoint( ruVector3(lmax.x, lmax.y, lmin.z), min.color ), ruLinePoint( ruVector3(lmin.x, lmax.y, lmin.z), max.color )));
    Render3DLine( GUILine( ruLinePoint( ruVector3(lmin.x, lmax.y, lmin.z), min.color ), ruLinePoint( ruVector3(lmin.x, lmin.y, lmin.z), max.color )));
    Render3DLine( GUILine( ruLinePoint( ruVector3(lmin.x, lmin.y, lmin.z), min.color ), ruLinePoint( ruVector3(lmin.x, lmin.y, lmax.z), max.color )));
    Render3DLine( GUILine( ruLinePoint( ruVector3(lmax.x, lmin.y, lmin.z), min.color ), ruLinePoint( ruVector3(lmax.x, lmin.y, lmax.z), max.color )));
    Render3DLine( GUILine( ruLinePoint( ruVector3(lmax.x, lmax.y, lmin.z), min.color ), ruLinePoint( ruVector3(lmax.x, lmax.y, lmax.z), max.color )));
    Render3DLine( GUILine( ruLinePoint( ruVector3(lmin.x, lmax.y, lmin.z), min.color ), ruLinePoint( ruVector3(lmin.x, lmax.y, lmax.z), max.color )));
    Render3DLine( GUILine( ruLinePoint( ruVector3(lmin.x, lmin.y, lmax.z), min.color ), ruLinePoint( ruVector3(lmax.x, lmin.y, lmax.z), max.color )));
    Render3DLine( GUILine( ruLinePoint( ruVector3(lmax.x, lmin.y, lmax.z), min.color ), ruLinePoint( ruVector3(lmax.x, lmax.y, lmax.z), max.color )));
    Render3DLine( GUILine( ruLinePoint( ruVector3(lmax.x, lmax.y, lmax.z), min.color ), ruLinePoint( ruVector3(lmin.x, lmax.y, lmax.z), max.color )));
    Render3DLine( GUILine( ruLinePoint( ruVector3(lmin.x, lmax.y, lmax.z), min.color ), ruLinePoint( ruVector3(lmin.x, lmin.y, lmax.z), max.color )));
}

void GUIRenderer::RenderAllGUIElements() {
	IDirect3DStateBlock9 * state;
	CheckDXErrorFatal( g_device->CreateStateBlock( D3DSBT_ALL, &state ));
	
	pixelShader->Bind();
	vertexShader->Bind();

	PrepareToDraw2D();

	CheckDXErrorFatal( vertexShader->GetConstantTable()->SetMatrix( g_device, vProj, &orthoMatrix ));
	CheckDXErrorFatal( g_device->SetVertexDeclaration( vertDecl ));
	CheckDXErrorFatal( g_device->SetStreamSource( 0, vertexBuffer, 0, sizeof( Vertex2D )));	
	
	while( !rects.empty() ) {
		RenderRect( rects.front() );
		rects.pop();
	}	
	for( auto & kv : texts ) {
		g_textRenderer->RenderTextGroup( kv.second, kv.first );
		kv.second.clear();
	}
	if( g_cursor ) {
		CheckDXErrorFatal( g_device->SetStreamSource( 0, vertexBuffer, 0, sizeof( Vertex2D )));	
		if( g_cursor->visible ) {
			RenderRect( GUIRect( ruGetMouseX(), ruGetMouseY(), g_cursor->w, g_cursor->h, g_cursor->tex, ruVector3( 255, 255, 255 ), 255 ) );
		}
	}

	CheckDXErrorFatal( state->Apply());
	state->Release();
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

        ruLinePoint points[ pointCount ];
        points[ 0 ] = line.begin;
        points[ 1 ] = line.end;

        int lineBytesCount = sizeof( ruLinePoint ) * pointCount;

        memcpy( (char*)data + linesToRender * lineBytesCount, points, lineBytesCount );

        linesToRender++;

        lines.pop();
    }

    CheckDXErrorFatal( lineVertexBuffer->Unlock( ));
    CheckDXErrorFatal( g_device->SetVertexDeclaration( lineDecl ));
    CheckDXErrorFatal( g_device->SetStreamSource( 0, lineVertexBuffer, 0, sizeof( ruLinePoint )));

    if( linesToRender )
        CheckDXErrorFatal( g_device->DrawPrimitive( D3DPT_LINELIST, 0, linesToRender ));

    CheckDXErrorFatal( state->Apply() );
    state->Release();
}


void GUIRenderer::PrepareToDraw2D() {
	CheckDXErrorFatal( g_device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE ));
	CheckDXErrorFatal( g_device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ));
	CheckDXErrorFatal( g_device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA));
	CheckDXErrorFatal( g_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ));
	CheckDXErrorFatal( g_device->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD));
	CheckDXErrorFatal( g_device->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE ));
	CheckDXErrorFatal( g_device->SetRenderState( D3DRS_ZENABLE, FALSE ));
	CheckDXErrorFatal( g_device->SetRenderState( D3DRS_ZWRITEENABLE, FALSE ));
    CheckDXErrorFatal( g_device->SetVertexDeclaration( vertDecl ));
}

void GUIRenderer::RenderRect( GUIRect & rect ) {
	void * data = nullptr;
	Vertex2D vertices[6];
	vertices[ 0 ] = Vertex2D( rect.x, rect.y, 0, 0, 0, rect.color );
	vertices[ 1 ] = Vertex2D( rect.x + rect.w, rect.y, 0, 1, 0, rect.color );
	vertices[ 2 ] = Vertex2D( rect.x, rect.y + rect.h, 0, 0, 1, rect.color );
	vertices[ 3 ] = Vertex2D( rect.x + rect.w, rect.y, 0, 1, 0, rect.color );
	vertices[ 4 ] = Vertex2D( rect.x + rect.w, rect.y + rect.h, 0, 1, 1, rect.color );
	vertices[ 5 ] = Vertex2D( rect.x, rect.y + rect.h, 0, 0, 1, rect.color );
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
}

GUILine::GUILine( const ruLinePoint & theBegin, const ruLinePoint & theEnd ) {
    end = theEnd;
    begin = theBegin;
}

GUIText::GUIText( string theText, float theX, float theY, float theWidth, float theHeight, ruVector3 theColor, int theAlpha, int theTextAlign, ruFontHandle theFont ) {
	rect.left = theX;
	rect.top = theY;
	rect.right = theX + theWidth;
	rect.bottom = theY + theHeight;
    text = theText;
    font = (BitmapFont *)theFont.pointer;
    textAlign = theTextAlign;
    color = D3DCOLOR_ARGB( theAlpha, (int)theColor.x, (int)theColor.y, (int)theColor.z );
}

GUIRect::GUIRect( float theX, float theY, float theWidth, float theHeight, Texture * theTexture, ruVector3 theColor, int theAlpha ) {
    x = theX;
    y = theY;
    w = theWidth;
    h = theHeight;
    texture = theTexture;
    color = D3DCOLOR_ARGB( theAlpha, (int)theColor.x, (int)theColor.y, (int)theColor.z );
}
