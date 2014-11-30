#include "GUI.h"

GUI * gui = 0;

GUI::GUI() {
    font = CreateGUIFont( 14, "data/fonts/font1.otf", 0, 0 );
    staminaBarImg = GetTexture( "data/gui/fatigue.png" );
    lifeBarImg = GetTexture( "data/gui/life.png" );
    itemImg = GetTexture( "data/gui/small.png");
}