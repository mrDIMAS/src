#pragma once

#include "Game.h"

const char * GetKeyName( unsigned int vk );
bool FileExist( const char * file );
void GetFilesWithDefExt( string folder, vector< string > & names );