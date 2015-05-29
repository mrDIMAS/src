#pragma once

#include "Game.h"

string GetKeyName( unsigned int vk );
bool FileExist( const string & file );
void GetFilesWithExtension( string folder, vector< string > & names );