#pragma once

#include "Game.h"

class RadioButton {
private:
    bool mOn;
    ruTextHandle mGUIText;
    ruButtonHandle mGUIButton;
	void OnChange() {
		mOn = !mOn;
	}
public:
    explicit RadioButton( float x, float y, ruTextureHandle buttonImage, const string & text );
    void Update(  );
    bool IsEnabled( );
    void SetEnabled( bool state );
    void AttachTo( ruGUINodeHandle node );
	void SetChangeAction( const ruDelegate & delegat );
};