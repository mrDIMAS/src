#include "Common.h"

void ParseString ( string str, map<string,string> & values ) {
    if ( str.size() <= 1 ) {
        return;
    }

    values.clear();

    bool equalFound = false;
    bool quoteLF = false, quoteRF = false;
    unsigned int n = 0;

    string varName, var;

    while ( true ) {
        char symbol = str.at ( n );

        if ( symbol == ';' ) {
            quoteLF    = false;
            quoteRF    = false;
            equalFound  = false;

            varName.clear();
            var.clear();
        };

        if ( isalpha ( ( unsigned char ) symbol ) || isdigit ( ( unsigned char ) symbol ) || symbol == '_' ) {
            if ( !equalFound ) {
                varName.push_back ( symbol );
            }
        } else {
            if ( symbol == '=' ) {
                equalFound = true;
            }

            if ( symbol == '"' ) {
                if ( quoteLF == false ) {
                    quoteLF = true;
                } else {
                    quoteRF = true;
                }
            }
        };

        if ( quoteLF ) {
            if ( quoteRF ) {
                values[ varName ] = var;
            } else {
                if ( symbol != '"' ) {
                    var.push_back ( symbol );
                }
            };
        };

        n++;

        if ( n >= str.size() ) {
            break;
        }
    };
}

void ParseFile( string fn, map<string,string> & values) {
    FILE * file = 0;

    fopen_s ( &file, fn.c_str(), "r" );

    if ( !file ) {
        return;
    }

    string str;

    while ( !feof ( file ) ) {
        char symbol = '\0';
        fread ( &symbol, sizeof ( char ), 1, file );
        str.push_back ( symbol );
    };

    fclose ( file );

    if ( str.size() <= 1 ) {
        return;
    }

    values.clear();

    bool equalFound = false;
    bool quoteLF = false, quoteRF = false;
    unsigned int n = 0;

    string varName, var;

    while ( true ) {
        char symbol = str.at ( n );

        if ( symbol == ';' ) {
            quoteLF    = false;
            quoteRF    = false;
            equalFound  = false;

            varName.clear();
            var.clear();
        };

        if ( isalpha ( ( unsigned char ) symbol ) || isdigit ( ( unsigned char ) symbol ) || symbol == '_' ) {
            if ( !equalFound ) {
                varName.push_back ( symbol );
            }
        } else {
            if ( symbol == '=' ) {
                equalFound = true;
            }

            if ( symbol == '"' ) {
                if ( quoteLF == false ) {
                    quoteLF = true;
                } else {
                    quoteRF = true;
                }
            }
        };

        if ( quoteLF ) {
            if ( quoteRF ) {
                values[ varName ] = var;
            } else {
                if ( symbol != '"' ) {
                    var.push_back ( symbol );
                }
            };
        };

        n++;

        if ( n >= str.size() ) {
            break;
        }
    };
}

void CheckDXError( HRESULT errCode )
{
	if( FAILED( errCode )) {
		char buf[1024];
		sprintf_s( buf, "DirectX 9 Error. Code: %d\nError: %s\nDescription: %s", errCode, DXGetErrorString( errCode ), DXGetErrorDescription( errCode ) );
		MessageBoxA( 0, buf, "Fatal error", MB_OK | MB_ICONERROR );
		exit( -1 );
	}
}



