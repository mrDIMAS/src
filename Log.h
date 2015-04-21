#ifndef _LOG_
#define _LOG_

class Log {
public:
	static void Write( const string & message );
	static void Error( const string & message );
};

#endif