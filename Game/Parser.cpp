#include "Parser.h"

Parser::Parser(  ) {
    parsed = false;
}

void Parser::ParseFile( string fn ) {
    FILE * file = 0;

    fopen_s ( &file, fn.c_str(), "r" );

    if ( !file )
        return;

    string str;

    while ( !feof ( file ) ) {
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
            if ( !equalFound )
                varName.push_back ( symbol );
        }
        else {
            if ( symbol == '=' )
                equalFound = true;

            if ( symbol == '"' ) {
                if ( quoteLF == false )
                    quoteLF = true;
                else
                    quoteRF = true;
            }
        };

        if ( quoteLF ) {
            if ( quoteRF )
                values[ varName ] = var;
            else if ( symbol != '"' )
                var.push_back ( symbol );
        };

        n++;

        if ( n >= str.size() )
            break;
    };

    parsed = true;
}

bool Parser::Empty() {
    return values.empty();
}

float Parser::GetNumber( string varName ) {
    auto iter = values.find( varName );

    if( iter != values.end() )
        return atof( iter->second.c_str() );

    return 0.0f;
}

const char * Parser::GetString( string varName ) {
    auto iter = values.find( varName );

    if( iter != values.end() )
        return iter->second.c_str();

    return "";
}

bool Parser::IsParsed() {
    return parsed;
}
