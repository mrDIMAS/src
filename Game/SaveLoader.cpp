#include "Precompiled.h"

#include "SaveLoader.h"
#include "Level.h"
#include "Ladder.h"

void SaveLoader::RestoreWorldState()
{
	int levNum = 0;

	*this & levNum;

	// load level
	Game::Instance()->LoadLevel((LevelName)levNum);

	// deserialize it's objects
	if(Game::Instance()->GetLevel()) {
		Game::Instance()->GetLevel()->Serialize(*this);
	}
}

SaveLoader::~SaveLoader()
{

}

SaveLoader::SaveLoader(string fn) : SaveFile(fn, false)
{

}
