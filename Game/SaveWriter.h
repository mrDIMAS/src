#pragma once

#include "Game.h"
#include "ItemPlace.h"
#include "SaveFile.h"

class SaveWriter : public SaveFile {
private:
    void SavePlayerInventory();
    void SaveItemPlaces();
public:
    SaveWriter( string fn );
    ~SaveWriter();
    void SaveWorldState( );
};