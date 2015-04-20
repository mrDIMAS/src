#include "Precompiled.h"

#include "GUIRenderer.h"
#include "Camera.h"
#include "Texture.h"
#include "Cursor.h"
#include "Vertex.h"
#include "TextRenderer.h"
#include "BitmapFont.h"
#include "Engine.h"

GUIRenderer::GUIRenderer() {
    int maxLineCount = 16536;

    sizeOfRectBytes = 6 * sizeof( Vertex2D  );
    CheckDXErrorFatal( Engine::Instance().GetDevice()->CreateVertexBuffer( sizeOfRectBytes, D3DUSAGE_DYNAMIC, D3DFVF_XYZ, D3DPOOL_DEFAULT, &vertexBuffer, 0 ));

    D3DVERTEXELEMENT9 guivd[ ] = {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0  },
        D3DDECL_END()
    };

    CheckDXErrorFatal( Engine::Instance().GetDevice()->CreateVertexDeclaration( guivd, &vertDecl ));

    D3DVIEWPORT9 vp;
    CheckDXErrorFatal( Engine::Instance().GetDevice()->GetViewport( &vp ));
    D3DXMatrixOrthoOffCenterLH ( &orthoMatrix, 0, vp.Width, vp.Height, 0, 0.0f, 1024.0f );

    vertexShader = new VertexShader( "data/shaders/gui.vso", true );
    pixelShader = new PixelShader( "data/shaders/gui.pso", true );
}

GUIRenderer::~GUIRenderer() {
    delete vertexShader;
    delete pixelShader;
    CheckDXErrorFatal( vertexBuffer->Release());
    CheckDXErrorFatal( vertDecl->Release());
}

ruFontHandle GUIRenderer::CreateFont( int size, const string & name, int italic, int underlined ) {
    BitmapFont * font = new BitmapFont( name, size );
    ruFontHandle handle;
    handle.pointer = font;
    return handle;
}

void GUIRenderer::RenderAllGUIElements() {
    pixelShader->Bind();
    vertexShader->Bind();

    Engine::Instance().GetDevice()->SetVertexDeclaration( vertDecl );

    Engine::Instance().SetVertexShaderMatrix( 0, &orthoMatrix );
    Engine::Instance().GetDevice()->SetVertexDeclaration( vertDecl );
    Engine::Instance().GetDevice()->SetStreamSource( 0, vertexBuffer, 0, sizeof( Vertex2D ));



    for( auto pRect : GUIRect::msRectList ) {
        if( pRect->IsVisible() ) {
            RenderRect( pRect );
        }
    }

    for( auto pText : GUIText::msTextList ) {
        if( pText->IsVisible() ) {
            Engine::Instance().GetTextRenderer()->RenderText( pText );
        }
    }

    if( Cursor::msCurrentCursor ) {
        CheckDXErrorFatal( Engine::Instance().GetDevice()->SetStreamSource( 0, vertexBuffer, 0, sizeof( Vertex2D )));
        if( Cursor::msCurrentCursor->IsVisible() ) {
            Cursor::msCurrentCursor->SetPosition( ruGetMouseX(), ruGetMouseY());
            RenderRect( Cursor::msCurrentCursor );
        }
    }
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
        Engine::Instance().GetDevice()->SetTexture( 0, nullptr );
    }
    Engine::Instance().RegisterDIP();
    CheckDXErrorFatal( Engine::Instance().GetDevice()->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 ));
}

TextQuad::TextQuad()
{

}
