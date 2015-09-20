#pragma once

#include "Game.h"
#include "SaveFile.h"

class SaveLoader : public SaveFile {
public:
    SaveLoader( string fn );
    ~SaveLoader();
    void RestoreWorldState( );
};