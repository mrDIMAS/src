#ifndef _COMMON_
#define _COMMON_

extern FT_Library g_ftLibrary;
extern bool g_debugMode;
extern bool g_fxaaEnabled;
extern int g_debugDraw;
extern int g_textureChanges;
extern bool g_engineRunning;
extern string g_texturePath;
extern ofstream g_log;

void ParseString( string str, map<string,string> & values);
bool IsFullNPOTTexturesSupport();
int NearestPow2( int number );
void CheckDXErrorFatalFunc( HRESULT errCode, const string & file, int line );
void CreateLogFile();
void LogMessage( string message );
void CloseLogFile();
void LogError( string message );

#ifdef _DEBUG
#	define CheckDXErrorFatal( func ) CheckDXErrorFatalFunc( func, __FILE__, __LINE__ )
#else
#	define CheckDXErrorFatal( func ) (func)
#endif


#endif
