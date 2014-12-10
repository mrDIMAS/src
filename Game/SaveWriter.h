#pragma once

#include "Game.h"
#include "ItemPlace.h"
#include "TextFileStream.h"

class SaveWriter : public TextFileStream
{
private:
    void SavePlayerInventory();
    void SaveItemPlaces();
public:
    SaveWriter( string fn );
    ~SaveWriter();
    void SaveWorldState( );
};