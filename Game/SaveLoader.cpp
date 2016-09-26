#include "Precompiled.h"

#include "SaveLoader.h"
#include "Level.h"
#include "Ladder.h"

void SaveLoader::RestoreWorldState() {
	int levNum = 0;
	
	*this & levNum;

    // load level
    Level::Change( levNum );

    // deserialize it's objects
    if( Level::Current() ) {
        Level::Current()->Serialize( *this );
    }

	Game_UpdateClock(); 
}

SaveLoader::~SaveLoader() {

}

SaveLoader::SaveLoader( string fn ) : SaveFile( fn, false ) {

}
