#include "Precompiled.h"

#include "GUIProperties.h"

unique_ptr<GUIProperties> pGUIProp;

GUIProperties::GUIProperties() {
	float brightness = 0.35f;

	mBackColor = ruVector3( 120, 120, 120 ) * brightness;
	mForeColor = ruVector3( 255, 255, 255 ) * brightness;
	mNoticeColor = ruVector3(200, 200, 200); //ruVector3( 242, 188, 0 );

    mFont = ruFont::LoadFromFile( 12, "data/fonts/font5.ttf" );
}