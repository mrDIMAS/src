#include "Precompiled.h"

#include "Utils.h"


bool IsFileExists( const string & file ) {
    ifstream f( file );
    return f.good();
}

void GetFilesWithExtension( string folder, vector< string > & names ) {
    WIN32_FIND_DATAA fd;
    HANDLE hFind = FindFirstFileA( folder.c_str(), &fd);
    if(hFind != INVALID_HANDLE_VALUE) {
        do {
            if( !(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
                names.push_back(fd.cFileName);
            }
        } while( FindNextFileA(hFind, &fd));
        FindClose(hFind);
    }
}
