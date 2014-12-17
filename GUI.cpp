#include "Common.h"
#include "GUIRenderer.h"
#include "Cursor.h"

void ruSetCursorSettings( ruTextureHandle texture, int w, int h ) {
    if( !g_cursor ) {
        g_cursor = new Cursor( w, h, (Texture*)texture.pointer );
    }
}

void ruHideCursor( ) {
    if( g_cursor ) {
        g_cursor->Hide();
    } else {
        ShowCursor( FALSE );
        gpDevice->ShowCursor( FALSE );
    }
}

void ruShowCursor( ) {
    if( g_cursor ) {
        g_cursor->Show();
    } else {
        ShowCursor( TRUE );
        gpDevice->ShowCursor( TRUE );
    }
}

bool ruGUINodeHandle::operator == ( const ruGUINodeHandle & node ) {
    return pointer == node.pointer;
}

bool ruButtonHandle::operator == ( const ruButtonHandle & node ) {
    return pointer == node.pointer;
}

bool ruRectHandle::operator == ( const ruRectHandle & node ) {
    return pointer == node.pointer;
}

bool ruTextHandle::operator == ( const ruTextHandle & node ) {
    return pointer == node.pointer;
}

bool ruLineHandle::operator == ( const ruLineHandle & node ) {
    return pointer == node.pointer;
}


ruFontHandle ruCreateGUIFont( int size, const char * name, int italic, int underlined ) {
    ruFontHandle font;
    font.pointer = new BitmapFont( name, size );
    return font;
}

ruRectHandle ruCreateGUIRect( float x, float y, float w, float h, ruTextureHandle texture, ruVector3 color, int alpha ) {
    ruRectHandle rect;
    rect.pointer = new GUIRect( x, y, w, h, (Texture*)texture.pointer, color, alpha, true );
    return rect;
}

ruTextHandle ruCreateGUIText( const char * text, int x, int y, int w, int h, ruFontHandle font, ruVector3 color, int textAlign, int alpha ) {
    ruTextHandle t;
    t.pointer = new GUIText( text, x, y, w, h, color, alpha, textAlign, (BitmapFont*)font.pointer );
    return t;
}

ruButtonHandle ruCreateGUIButton( int x, int y, int w, int h, ruTextureHandle texture, const char * text, ruFontHandle font, ruVector3 color, int textAlign, int alpha ) {
    ruButtonHandle button;
    button.pointer = new GUIButton( x, y, w, h, (Texture*)texture.pointer, text, (BitmapFont*)font.pointer, color, textAlign, alpha );
    return button;
}

ruLineHandle ruCreateGUILine( ruLinePoint begin, ruLinePoint end ) {
    ruLineHandle line;
    line.pointer = new ruGUILine( begin, end );
    return line;
}

void ruSetGUINodePosition( ruGUINodeHandle node, float x, float y ) {
    ((GUINode*)node.pointer)->SetPosition( x, y );
}

void ruSetGUINodeSize( ruGUINodeHandle node, float w, float h ) {
    ((GUINode*)node.pointer)->SetSize( w, h );
}

void ruSetGUINodeColor( ruGUINodeHandle node, ruVector3 color ) {
    ((GUINode*)node.pointer)->SetColor( color );
}

void ruSetGUINodeAlpha( ruGUINodeHandle node, int alpha ) {
    ((GUINode*)node.pointer)->SetAlpha( alpha );
}

void ruSetGUINodeVisible( ruGUINodeHandle node, bool visible ) {
    ((GUINode*)node.pointer)->SetVisible( visible );
}

void ruSetGUINodeText( ruTextHandle node, const char * text ) {
    ((GUIText*)node.pointer)->SetText( text );
}

bool ruIsGUINodeVisible( ruGUINodeHandle node ) {
    return ((GUINode*)node.pointer)->IsVisible();
}

ruVector2 ruGetGUINodePosition( ruGUINodeHandle node ) {
    return ((GUINode*)node.pointer)->GetPosition();
}

ruVector2 ruGetGUINodeSize( ruGUINodeHandle node ) {
    return ((GUINode*)node.pointer)->GetSize();
}

ruVector3 ruGetGUINodeColor( ruGUINodeHandle node ) {
    return ((GUINode*)node.pointer)->GetColor();
}

void ruSetGUINodeTexture( ruGUINodeHandle node, ruTextureHandle texture ) {
    ((GUINode*)node.pointer)->SetTexture( (Texture*)texture.pointer );
}

int ruGetGUINodeAlpha( ruGUINodeHandle node ) {
    return ((GUINode*)node.pointer)->GetAlpha();
}

bool ruIsButtonPressed( ruButtonHandle node ) {
    return ((GUIButton*)node.pointer)->IsLeftPressed();
}

bool ruIsButtonPicked( ruButtonHandle node ) {
    return ((GUIButton*)node.pointer)->IsPicked();
}

bool ruIsButtonHit( ruButtonHandle node ) {
    return ((GUIButton*)node.pointer)->IsLeftHit();
}

ruTextHandle ruGetButtonText( ruButtonHandle node ) {
    ruTextHandle text;
    text.pointer = ((GUIButton*)node.pointer)->GetText();
    return text;
}

void ruFreeGUINode( ruGUINodeHandle node ) {
    delete ((GUIButton*)node.pointer);
}