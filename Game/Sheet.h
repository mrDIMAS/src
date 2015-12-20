#pragma once

#include "InteractiveObject.h"

class Sheet : public InteractiveObject {
private:
    string mText;
    string mDescription;
    shared_ptr<ruTexture> mBackgroundTexture;
    ruRect * mGUIBackground;
    ruText * mGUIText;
	shared_ptr<ruSound> mPaperFlipSound;
public:
    explicit Sheet( shared_ptr<ruSceneNode> object, string desc, string text );
    virtual ~Sheet();
	void Update();
    void SetText( const string & text );
    const string & GetText( ) const;
    void SetDescription( const string & description );
    const string & GetDescription( ) const;
    void SetVisible( bool state );
    static Sheet * GetSheetPointerByNode( shared_ptr<ruSceneNode> o );
    static vector<Sheet*> msSheetList;    
    static ruFont * msSheetFont;
};