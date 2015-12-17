#pragma once

#include "Game.h"

class RadioButton {
private:
    bool mOn;
	ruRect * mCheck;
    ruButton * mGUIButton;
	void OnChange();
public:
    explicit RadioButton( float x, float y, shared_ptr<ruTexture> buttonImage, const string & text );
    void Update(  );
    bool IsEnabled( );
    void SetEnabled( bool state );
    void AttachTo( ruGUINode * node );
	void SetChangeAction( const ruDelegate & delegat );
};