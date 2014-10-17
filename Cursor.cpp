#include "Cursor.h"

Cursor * g_cursor = 0;

void Cursor::Hide() {
    g_cursor->visible = 0;

    ShowCursor( FALSE );
    g_device->ShowCursor( FALSE );
}

void Cursor::Show() {
    g_cursor->visible = 1;

    ShowCursor( FALSE );
    g_device->ShowCursor( FALSE );
}

void Cursor::Setup( TextureHandle texture, int w, int h ) {
    tex = reinterpret_cast<Texture*>( texture.pointer );
    this->w = w;
    this->h = h;
    Show();
}

Cursor::Cursor() {
    w = 0;
    h = 0;
    visible = 0;
    tex = 0;
}
