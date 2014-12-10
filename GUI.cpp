#include "Common.h"
#include "GUIRenderer.h"
#include "Cursor.h"

void ruSetCursorSettings( ruTextureHandle texture, int w, int h )
{
    if( !g_cursor )
        g_cursor = new Cursor;

    g_cursor->Setup( texture, w, h );
}

void ruHideCursor( )
{
    if( g_cursor )
        g_cursor->Hide();
    else
    {
        ShowCursor( FALSE );
        g_device->ShowCursor( FALSE );
    }
}

void ruShowCursor( )
{
    if( g_cursor )
        g_cursor->Show();
    else
    {
        ShowCursor( TRUE );
        g_device->ShowCursor( TRUE );
    }
}

void ruDrawGUIRect( float x, float y, float w, float h, ruTextureHandle texture, ruVector3 color, int alpha )
{
    g_guiRenderer->QueueRect( GUIRect( x, y, w, h, reinterpret_cast< Texture*>( texture.pointer ), color, alpha ));
}

ruFontHandle ruCreateGUIFont( int size, const char * name, int italic, int underlined )
{
    return g_guiRenderer->CreateFont( size, name, italic, underlined );
}

void ruDrawGUIText( const char * text, int x, int y, int w, int h, ruFontHandle font, ruVector3 color, int textAlign, int alpha /*= 255 */ )
{
    g_guiRenderer->RenderText( GUIText( text, x, y, w, h, color, alpha, textAlign, font ));
}

void ruDraw3DLine( ruLinePoint begin, ruLinePoint end )
{
    g_guiRenderer->Render3DLine( GUILine( begin, end ));
}

void ruDrawWireBox( ruLinePoint min, ruLinePoint max )
{
    g_guiRenderer->DrawWireBox( min, max );
}

int ruGetTextWidth( const char * text, ruFontHandle font )
{
    auto fnt = (ID3DXFont*)font.pointer;

    TEXTMETRICA metrics;
    fnt->GetTextMetricsA( &metrics );

    return metrics.tmAveCharWidth * strlen( text );
}

int ruGetTextHeight( const char * text, ruFontHandle font, int boxWidth )
{
    auto fnt = (ID3DXFont*)font.pointer;

    TEXTMETRICA metrics;
    fnt->GetTextMetricsA( &metrics );

    int lines = metrics.tmAveCharWidth * strlen( text ) / boxWidth;
    return metrics.tmHeight * lines;
}

RUAPI ruGUIState ruDrawGUIButton( int x, int y, int w, int h, ruTextureHandle texture, const char * text, ruFontHandle font, ruVector3 color, int textAlign, int alpha )
{
    ruGUIState state;

    memset( &state, 0, sizeof( ruGUIState ));

    int mx = ruGetMouseX();
    int my = ruGetMouseY();

    if( mx > x && mx < ( x + w ) && my > y && my < ( y + h ) )
    {
        state.mouseInside = 1;
        if( ruIsMouseHit( MB_Left ))
            state.mouseLeftClicked = true;
        if( ruIsMouseHit( MB_Right ))
            state.mouseRightClicked = true;

        color = ruVector3( 255, 0, 0 );
    }

    ruDrawGUIRect( x, y, w, h, texture, ruVector3( 255, 255, 255 ), alpha );
    ruDrawGUIText( text, x, y, w, h, font, color, textAlign, alpha );

    return state;
}