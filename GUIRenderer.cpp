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
    CheckDXErrorFatal( gpDevice->CreateVertexBuffer( sizeOfRectBytes, D3DUSAGE_DYNAMIC, D3DFVF_XYZ, D3DPOOL_DEFAULT, &vertexBuffer, 0 ));
    CheckDXErrorFatal( gpDevice->CreateVertexBuffer( maxLineCount * 2 * sizeof( ruLinePoint ), D3DUSAGE_DYNAMIC, D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &lineVertexBuffer, 0 ));

    D3DVERTEXELEMENT9 guivd[ ] = {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0  },
        D3DDECL_END()
    };

    CheckDXErrorFatal( gpDevice->CreateVertexDeclaration( guivd, &vertDecl ));

    D3DVERTEXELEMENT9 linevd[ ] = {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
        D3DDECL_END()
    };

    CheckDXErrorFatal( gpDevice->CreateVertexDeclaration( linevd, &lineDecl )) ;

    D3DVIEWPORT9 vp;
    CheckDXErrorFatal( gpDevice->GetViewport( &vp ));
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

void GUIRenderer::RenderAllGUIElements() {
    pixelShader->Bind();
    vertexShader->Bind();

    gpDevice->SetVertexDeclaration( vertDecl );

    vertexShader->GetConstantTable()->SetMatrix( gpDevice, vProj, &orthoMatrix );
    gpDevice->SetVertexDeclaration( vertDecl );
    gpDevice->SetStreamSource( 0, vertexBuffer, 0, sizeof( Vertex2D ));



    for( auto pRect : GUIRect::msRectList ) {
        if( pRect->IsVisible() ) {
            RenderRect( pRect );
        }
    }

    for( auto pText : GUIText::msTextList ) {
        if( pText->IsVisible() ) {
            g_textRenderer->RenderText( pText );
        }
    }

    if( g_cursor ) {
        CheckDXErrorFatal( gpDevice->SetStreamSource( 0, vertexBuffer, 0, sizeof( Vertex2D )));
        if( g_cursor->IsVisible() ) {
            g_cursor->SetPosition( ruGetMouseX(), ruGetMouseY());
            RenderRect( g_cursor );
        }
    }
}

void GUIRenderer::RenderLines() {
    /*
    if( lines.size() )
    {
    	void * data = nullptr;
    	CheckDXErrorFatal( lineVertexBuffer->Lock( 0, 0, &data, D3DLOCK_DISCARD ));

    	int iLine = 0;
    	for( auto line : lines )
    	{
    		const int pointCount = 2;

    		ruLinePoint points[ pointCount ];
    		points[ 0 ] = line->begin;
    		points[ 1 ] = line->end;

    		int lineBytesCount = sizeof( ruLinePoint ) * pointCount;

    		memcpy( (char*)data + iLine * lineBytesCount, points, lineBytesCount );
    		iLine++;
    	}

    	CheckDXErrorFatal( lineVertexBuffer->Unlock( ));
    	CheckDXErrorFatal( g_pDevice->SetVertexDeclaration( lineDecl ));
    	CheckDXErrorFatal( g_pDevice->SetStreamSource( 0, lineVertexBuffer, 0, sizeof( ruLinePoint )));
    	CheckDXErrorFatal( g_pDevice->DrawPrimitive( D3DPT_LINELIST, 0, lines.size() ));
    }*/
}


void GUIRenderer::RenderRect( GUIRect * rect ) {
    void * data = nullptr;
    Vertex2D vertices[6];
    rect->GetSixVertices( vertices );
    CheckDXErrorFatal( vertexBuffer->Lock( 0, 0, &data, 0 ));
    memcpy( data, vertices, sizeOfRectBytes );
    CheckDXErrorFatal( vertexBuffer->Unlock( ));
    if( rect->GetTexture() ) {
        rect->GetTexture()->Bind( 0 );
    } else {
        gpDevice->SetTexture( 0, nullptr );
    }
    g_dips++;
    CheckDXErrorFatal( gpDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 ));
}

ruGUILine::ruGUILine( const ruLinePoint & theBegin, const ruLinePoint & theEnd ) {
    end = theEnd;
    mVisible = true;
    begin = theBegin;
}

ruGUILine::ruGUILine( ) {
    mVisible = true;
}


