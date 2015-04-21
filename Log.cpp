#include "Precompiled.h"
#include "Log.h"

void Log::Write( const string & message ) {
	static ofstream output( "ruthenium.log" );
	output << message << endl;
}

void Log::Error( const string & message ) {
	Log::Write( message );
	MessageBoxA( 0, message.c_str(), "Fatal error", MB_OK | MB_ICONERROR );
	exit( 0xBADF00D );
}
