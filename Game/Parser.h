#pragma once

#include <iostream>
#include <map>
#include <exception>
#include <string>

using namespace std;

class Parser {
private:
    map< string, string > values;
    bool parsed;
public:
    Parser( );
    void ParseFile( string fn );
    bool Empty();
    bool IsParsed();
    const char * GetString( string varName );
    float GetNumber( string varName );
};