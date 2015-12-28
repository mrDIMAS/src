#pragma once

#include "Game.h"

class RadioButton {
private:
    bool mOn;
	shared_ptr<ruRect> mCheck;
    shared_ptr<ruButton> mGUIButton;
	void SelectCheckTexture() {
		if( mOn ) {
			mCheck->SetTexture( ruTexture::Request( "data/gui/menu/checkbox_checked.tga" ));
		} else {
			mCheck->SetTexture( ruTexture::Request( "data/gui/menu/checkbox.tga" ));
		}
	}
	void OnChange();
public:
    explicit RadioButton( float x, float y, shared_ptr<ruTexture> buttonImage, const string & text );
    void Update(  );
    bool IsEnabled( );
    void SetEnabled( bool state );
    void AttachTo( shared_ptr<ruGUINode> node );
	void SetChangeAction( const ruDelegate & delegat );
};