#pragma once

#include "Game.h"
#include "ItemPlace.h"
#include "SaveFile.h"

class SaveWriter : public SaveFile {
public:
	SaveWriter(string fn);
	~SaveWriter();
	void SaveWorldState();
};