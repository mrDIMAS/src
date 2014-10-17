#include "GUI.h"

GUI * gui = 0;

GUI::GUI() {
    font = CreateGUIFont( 16, "Arial", 0, 0 );
    staminaBarImg = GetTexture( "data/gui/fatigue.png" );
    lifeBarImg = GetTexture( "data/gui/life.png" );
    itemImg = GetTexture( "data/gui/small.png");
}