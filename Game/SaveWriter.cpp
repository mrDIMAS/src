#include "Precompiled.h"

#include "SaveWriter.h"
#include "Level.h"
#include "Player.h"
#include "Ladder.h"
#include "Enemy.h"

void SaveWriter::SaveWorldState() {	
    if( Level::Current() ) {
		WriteInteger( Level::Current()->mTypeNum );
        Level::Current()->Serialize( *this );
    }
}

SaveWriter::~SaveWriter() {

}

SaveWriter::SaveWriter( string fn ) : SaveFile( fn, true ) {

}
