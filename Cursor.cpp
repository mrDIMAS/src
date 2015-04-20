#include "Precompiled.h"

#include "Cursor.h"
#include "GUIRenderer.h"

Cursor * g_cursor = 0;

void Cursor::Hide() {
    g_cursor->mVisible = 0;
    ShowCursor( FALSE );
    gpDevice->ShowCursor( FALSE );
}

void Cursor::Show() {
    g_cursor->mVisible = 1;
    ShowCursor( FALSE );
    gpDevice->ShowCursor( FALSE );
}

