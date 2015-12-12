#pragma once

#include "Game.h"

class RadioButton {
private:
    bool mOn;
	ruRectHandle mCheck;
    ruButtonHandle mGUIButton;
	void OnChange();
public:
    explicit RadioButton( float x, float y, ruTextureHandle buttonImage, const string & text );
    void Update(  );
    bool IsEnabled( );
    void SetEnabled( bool state );
    void AttachTo( ruGUINodeHandle node );
	void SetChangeAction( const ruDelegate & delegat );
};