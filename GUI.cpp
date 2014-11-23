#include "Common.h"
#include "GUIRenderer.h"
#include "Cursor.h"

void SetCursorSettings( TextureHandle texture, int w, int h ) {
    if( !g_cursor ) {
        g_cursor = new Cursor;
    }

    g_cursor->Setup( texture, w, h );
}

void HideCursor( ) {
    if( g_cursor ) {
        g_cursor->Hide();
    } else {
        ShowCursor( FALSE );
        g_device->ShowCursor( FALSE );
    }
}

void ShowCursor( ) {
    if( g_cursor ) {
        g_cursor->Show();
    } else {
        ShowCursor( TRUE );
        g_device->ShowCursor( TRUE );
    }
}

void DrawGUIRect( float x, float y, float w, float h, TextureHandle texture, Vector3 color, int alpha ) {
    g_guiRenderer->RenderRect( GUIRenderer::Rect( x, y, w, h, reinterpret_cast< Texture*>( texture.pointer ), color, alpha ));
}

FontHandle CreateGUIFont( int size, const char * name, int italic, int underlined ) {
    return g_guiRenderer->CreateFont( size, name, italic, underlined );
}

void DrawGUIText( const char * text, int x, int y, int w, int h, FontHandle font, Vector3 color, int textAlign, int alpha /*= 255 */ ) {
    g_guiRenderer->RenderText( GUIRenderer::Text( text, x, y, w, h, color, alpha, textAlign, font ));
}

void Draw3DLine( LinePoint begin, LinePoint end ) {
    g_guiRenderer->Render3DLine( GUIRenderer::Line( begin, end ));
}

void DrawWireBox( LinePoint min, LinePoint max ) {
    g_guiRenderer->DrawWireBox( min, max );
}

int GetTextWidth( const char * text, FontHandle font ) {
    auto fnt = (ID3DXFont*)font.pointer;

    TEXTMETRICA metrics;
    fnt->GetTextMetricsA( &metrics );

    return metrics.tmAveCharWidth * strlen( text );
}

int GetTextHeight( const char * text, FontHandle font, int boxWidth ) {
    auto fnt = (ID3DXFont*)font.pointer;

    TEXTMETRICA metrics;
    fnt->GetTextMetricsA( &metrics );

    int lines = metrics.tmAveCharWidth * strlen( text ) / boxWidth;
    return metrics.tmHeight * lines;
}

API GUIState DrawGUIButton( int x, int y, int w, int h, TextureHandle texture, const char * text, FontHandle font, Vector3 color, int textAlign, int alpha ) {
    GUIState state;

    memset( &state, 0, sizeof( GUIState ));

    int mx = GetMouseX();
    int my = GetMouseY();

    if( mx > x && mx < ( x + w ) && my > y && my < ( y + h ) ) {
        state.mouseInside = 1;
        if( IsMouseHit( MB_Left )) {
            state.mouseLeftClicked = true;
        }
        if( IsMouseHit( MB_Right )) {
            state.mouseRightClicked = true;
        }

        color = Vector3( 255, 0, 0 );
    }

    DrawGUIRect( x, y, w, h, texture, Vector3( 255, 255, 255 ), alpha );
    DrawGUIText( text, x, y, w, h, font, color, textAlign, alpha );

    return state;
}