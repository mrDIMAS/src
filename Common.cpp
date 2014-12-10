#include "Common.h"
#include "Utility.h"

ofstream g_log;

void ParseString ( string str, map<string,string> & values )
{
    if ( str.size() <= 1 )
        return;

    values.clear();

    bool equalFound = false;
    bool quoteLF = false, quoteRF = false;
    unsigned int n = 0;

    string varName, var;

    while ( true )
    {
        char symbol = str.at ( n );

        if ( symbol == ';' )
        {
            quoteLF    = false;
            quoteRF    = false;
            equalFound  = false;

            varName.clear();
            var.clear();
        };

        if ( isalpha ( ( unsigned char ) symbol ) || isdigit ( ( unsigned char ) symbol ) || symbol == '_' )
        {
            if ( !equalFound )
                varName.push_back ( symbol );
        }
        else
        {
            if ( symbol == '=' )
                equalFound = true;

            if ( symbol == '"' )
            {
                if ( quoteLF == false )
                    quoteLF = true;
                else
                    quoteRF = true;
            }
        };

        if ( quoteLF )
        {
            if ( quoteRF )
                values[ varName ] = var;
            else
            {
                if ( symbol != '"' )
                    var.push_back ( symbol );
            };
        };

        n++;

        if ( n >= str.size() )
            break;
    };
}

void ParseFile( string fn, map<string,string> & values)
{
    FILE * file = 0;

    fopen_s ( &file, fn.c_str(), "r" );

    if ( !file )
        return;

    string str;

    while ( !feof ( file ) )
    {
        char symbol = '\0';
        fread ( &symbol, sizeof ( char ), 1, file );
        str.push_back ( symbol );
    };

    fclose ( file );

    if ( str.size() <= 1 )
        return;

    values.clear();

    bool equalFound = false;
    bool quoteLF = false, quoteRF = false;
    unsigned int n = 0;

    string varName, var;

    while ( true )
    {
        char symbol = str.at ( n );

        if ( symbol == ';' )
        {
            quoteLF    = false;
            quoteRF    = false;
            equalFound  = false;

            varName.clear();
            var.clear();
        };

        if ( isalpha ( ( unsigned char ) symbol ) || isdigit ( ( unsigned char ) symbol ) || symbol == '_' )
        {
            if ( !equalFound )
                varName.push_back ( symbol );
        }
        else
        {
            if ( symbol == '=' )
                equalFound = true;

            if ( symbol == '"' )
            {
                if ( quoteLF == false )
                    quoteLF = true;
                else
                    quoteRF = true;
            }
        };

        if ( quoteLF )
        {
            if ( quoteRF )
                values[ varName ] = var;
            else
            {
                if ( symbol != '"' )
                    var.push_back ( symbol );
            };
        };

        n++;

        if ( n >= str.size() )
            break;
    };
}

void CheckDXErrorFatal( HRESULT errCode )
{
    /*
    if( FAILED( errCode )) {
    	string message = Format( "DirectX 9 Error. Code: %d\nError: %s\nDescription: %s", errCode, DXGetErrorString( errCode ), DXGetErrorDescription( errCode ) );
    	LogMessage( message );
    	MessageBoxA( 0, message.c_str(), "Fatal error", MB_OK | MB_ICONERROR );
    	exit( -1 );
    }*/
}

void CreateLogFile()
{
    g_log.open( "ruthenium.log" );
}

void LogMessage( string message )
{
    g_log << message << endl;
}

void LogError( string message )
{
    g_log << message << endl;
    g_log.close();
    MessageBoxA( 0, message.c_str(), "Fatal error", MB_OK | MB_ICONERROR );
    exit( -1 );
}

void CloseLogFile()
{
    g_log.close();
}
