#pragma once

#include "InteractiveObject.h"

class Sheet : public InteractiveObject {
private:
    string mText;
    string mDescription;
    ruTextureHandle mBackgroundTexture;
    ruRectHandle mGUIBackground;
    ruTextHandle mGUIText;
public:
    explicit Sheet( ruNodeHandle object, string desc, string text );
    virtual ~Sheet();
    void Update();
    void Draw( );
    void SetText( const string & text );
    const string & GetText( ) const;
    void SetDescription( const string & description );
    const string & GetDescription( ) const;
    void SetVisible( bool state );
    static Sheet * GetSheetPointerByNode( ruNodeHandle o );
    static vector<Sheet*> msSheetList;
    static ruSoundHandle msPaperFlipSound;
    static ruFontHandle msSheetFont;
};