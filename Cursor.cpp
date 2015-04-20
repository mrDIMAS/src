#include "Precompiled.h"
#include "Engine.h"
#include "Cursor.h"
#include "GUIRenderer.h"

Cursor * Cursor::msCurrentCursor = nullptr;

void Cursor::Hide() {
    msCurrentCursor->mVisible = 0;
    ShowCursor( FALSE );
    Engine::Instance().GetDevice()->ShowCursor( FALSE );
}

void Cursor::Show() {
    msCurrentCursor->mVisible = 1;
    ShowCursor( FALSE );
    Engine::Instance().GetDevice()->ShowCursor( FALSE );
}

