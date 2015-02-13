#pragma once

#include "Game.h"

string GetKeyName( unsigned int vk );
bool FileExist( const string & file );
void GetFilesWithDefExt( string folder, vector< string > & names );