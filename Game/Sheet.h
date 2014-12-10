#pragma once

#include "InteractiveObject.h"

class Sheet : public InteractiveObject
{
public:
    Sheet( ruNodeHandle object, string desc, string text );
    void Update();
    void Draw( );
    static Sheet * GetSheetByObject( ruNodeHandle o );

    string txt;
    string desc;
    ruTextureHandle noteTex;
    static vector<Sheet*> sheets;
    static ruSoundHandle paperFlip;
    static ruFontHandle sheetFont;

    static Sheet * Sheet::GetByObject( ruNodeHandle obj )
    {
        for( auto sheet : sheets )
            if( sheet->mObject == obj )
                return sheet;
        return 0;
    }
};