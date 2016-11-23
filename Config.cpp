#include "Precompiled.h"
#include <iostream>
#include <exception>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <streambuf>

Config::Config( ) {

}

Config::Config(const string & filename) {
	Load(filename);
}

void Config::LoadString( const string & str ) {
	mValues.clear( );
	bool equalFound = false;
	bool quoteLF = false, quoteRF = false;
	unsigned int n = 0;
	string varName, var;
	while ( true ) {
		char symbol = str[ n ];
		if ( symbol == ';' ) {
			quoteLF = false;
			quoteRF = false;
			equalFound = false;
			varName.clear( );
			var.clear( );
		};
		if ( isalpha( (unsigned char)symbol ) || isdigit( (unsigned char)symbol ) || symbol == '_' ) {
			if ( !equalFound ) {
				varName.push_back( symbol );
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
				mValues[ varName ] = var;
			} else if ( symbol != '"' ) {
				var.push_back( symbol );
			}
		};
		n++;
		if ( n >= str.size( ) ) {
			break;
		}
	};
}

void Config::Load( const string & fileName ) {
	std::ifstream file( fileName );
	if ( !file.is_open( ) ) throw std::runtime_error( "Unable to parse file " + fileName );
	std::string buffer( ( std::istreambuf_iterator<char>( file ) ), std::istreambuf_iterator<char>( ) );
	file.close( );
	mFileName = fileName;
	LoadString( buffer );
}

void Config::Save( const string & fileName ) const {
	ofstream file( fileName );
	for ( auto & keyVal : mValues ) {
		file << keyVal.first << " = " << "\"" << keyVal.second << "\";" << endl;
	}
}

void Config::Save( ) const {
	Save( mFileName );
}

map<string, string> Config::GetValuesCopy( ) const {
	return mValues;
}

float Config::GetNumber( const string & varName ) {
	return atof( GetExisting( varName )->second.c_str( ) );
}

bool Config::GetBoolean( const string & varName ) {
	return atoi( GetExisting( varName )->second.c_str( ) ) != 0;
}

void Config::SetNumber( const string & varName, float value ) {
	GetExisting( varName )->second = to_string( value );
}

void Config::SetString( const string & varName, const string & value ) {
	GetExisting( varName )->second = value;
}

void Config::AddNumber( const string & newVarName, float value ) {
	mValues[ newVarName ] = to_string( value );
}

void Config::AddString( const string & newVarName, const string & value ) {
	mValues[ newVarName ] = value;
}

bool Config::IsEmpty( ) const {
	return mValues.empty( );
}

void Config::SetNumber( const string & varName, int value ) {
	GetExisting( varName )->second = to_string( value );
}

void Config::AddNumber( const string & newVarName, int value ) {
	mValues[ newVarName ] = to_string( value );
}

string Config::GetString( const string & varName ) {
	return GetExisting( varName )->second;
}

void Config::SetBoolean( const string & varName, bool value ) {
	GetExisting( varName )->second = value ? "1" : "0";
}

void Config::AddBoolean( const string & varName, bool value ) {
	mValues[ varName ] = value ? "1" : "0";
}

map<string, string>::iterator Config::GetExisting( const string & varName ) {
	map<string, string>::iterator var = mValues.find( varName );
	if ( var == mValues.end( ) ) {
		stringstream ss;
		ss << "Unable to find value '" << varName << "' in " << mFileName << "!";
		throw std::runtime_error( ss.str( ) );
	}
	return var;
}
