#pragma once

#include "Common.h"

class FTFont {
private:
    FT_Face face;
public:
    FTFont( const char * file ) {
        FT_Error error = FT_New_Face( g_ftLibrary, file, 0, &face );
        if( error == FT_Err_Unknown_File_Format ) {
            throw runtime_error( "FTFont: unsupported format!" );
        } else if ( error ) {
            throw runtime_error( "FTFont: Unable to create font face!" );
        }
        
        for( int i = 0; i < 256; i++ ) {
            FT_Load_Glyph( face, FT_Get_Char_Index( face, i ), FT_LOAD_DEFAULT );
            int error = FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL );
            if( error ) {
                continue;
            }
        }
    }
}; 