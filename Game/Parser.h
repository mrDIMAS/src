#pragma once

#include <iostream>
#include <map>
#include <exception>
#include <string>

using namespace std;

class Parser {
private:
    map< string, string > mValueList;
    string mFileName;
    bool mParsed;
public:
    explicit Parser( );
    void ParseFile( string fn );
    bool Empty();
    bool IsParsed();
    const char * GetString( string varName );
    float GetNumber( string varName );
};