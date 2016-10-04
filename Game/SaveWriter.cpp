#include "Precompiled.h"

#include "SaveWriter.h"
#include "Level.h"
#include "Player.h"
#include "Ladder.h"
#include "Enemy.h"

void SaveWriter::SaveWorldState() {	
    if( Level::Current() ) {
		bool visible = Level::Current()->IsVisible();

		if (!visible) {
			Level::Current()->Show();
		}

		*this & Level::Current()->mTypeNum;
        Level::Current()->Serialize( *this );

		if (!visible) {
			Level::Current()->Hide();
		}
    }
}

SaveWriter::~SaveWriter() {

}

SaveWriter::SaveWriter( string fn ) : SaveFile( fn, true ) {

}
