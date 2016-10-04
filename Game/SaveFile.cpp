#include "Precompiled.h"
#include "SaveFile.h"

SaveFile::~SaveFile() {
    mStream.flush();
    mStream.close();
}

SaveFile::SaveFile( const string & fileName, bool save ) : mSave(save) {
    int flags = save ? ( fstream::out | fstream::trunc | fstream::binary) : ( fstream::in | fstream::binary);
    mStream.open( fileName, flags );
	if( !mStream.good() ) {
		throw std::runtime_error( StringBuilder( "Unable to open " ) << fileName << " save file!" );
	}
}


