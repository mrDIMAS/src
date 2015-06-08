#include "Precompiled.h"

#include "GUI.h"

GUI * pGUI = 0;

GUI::GUI() {
    mFont = ruCreateGUIFont( 12, "data/fonts/font2.ttf" );
    staminaBarImg = ruGetTexture( "data/gui/fatigue.png" );
    lifeBarImg = ruGetTexture( "data/gui/life.png" );
    itemImg = ruGetTexture( "data/gui/small.png");
}