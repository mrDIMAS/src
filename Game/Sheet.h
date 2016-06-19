#pragma once

#include "InteractiveObject.h"

class Sheet : public InteractiveObject {
protected:
	friend class Level;
	explicit Sheet( const shared_ptr<ruGUIScene> & scene, const shared_ptr<ruSceneNode> & object, const string & desc, const string & text );
private:
    string mText;
    string mDescription;
    shared_ptr<ruTexture> mBackgroundTexture;
    shared_ptr<ruRect> mGUIBackground;
    shared_ptr<ruText> mGUIText;
	shared_ptr<ruSound> mPaperFlipSound;
public:
    virtual ~Sheet();
	void Update();
    void SetText( const string & text );
    const string & GetText( ) const;
    void SetDescription( const string & description );
    const string & GetDescription( ) const;
    void SetVisible( bool state );
    static shared_ptr<ruFont> msSheetFont;
};