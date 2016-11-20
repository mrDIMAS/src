#include "Precompiled.h"

#include "SaveWriter.h"
#include "Level.h"
#include "Player.h"
#include "Ladder.h"
#include "Enemy.h"

void SaveWriter::SaveWorldState()
{
	auto & level = Game::Instance()->GetLevel();
	if(level) {
		bool visible = level->IsVisible();

		if(!visible) {
			level->Show();
		}

		// oh, those casts...
		int name = (int)level->mName;
		*this & name;

		level->Serialize(*this);

		if(!visible) {
			level->Hide();
		}
	}
}

SaveWriter::~SaveWriter()
{

}

SaveWriter::SaveWriter(string fn) : SaveFile(fn, true)
{

}
