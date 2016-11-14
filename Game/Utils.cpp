#include "Precompiled.h"

#include "Utils.h"


bool IsFileExists( const string & file ) {
    ifstream f( file );
    return f.good();
}
