#pragma once

#include "InteractiveObject.h"

class Sheet : public InteractiveObject {
private:
    string mText;
    string mDescription;
    shared_ptr<ruTexture> mBackgroundTexture;
    ruRect * mGUIBackground;
    ruText * mGUIText;
public:
    explicit Sheet( ruSceneNode * object, string desc, string text );
    virtual ~Sheet();
    void Update();
    void Draw( );
    void SetText( const string & text );
    const string & GetText( ) const;
    void SetDescription( const string & description );
    const string & GetDescription( ) const;
    void SetVisible( bool state );
    static Sheet * GetSheetPointerByNode( ruSceneNode * o );
    static vector<Sheet*> msSheetList;
    static ruSound msPaperFlipSound;
    static ruFont * msSheetFont;
};