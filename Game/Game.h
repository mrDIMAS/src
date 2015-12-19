#pragma once

enum LevelName {
    LUndefined = -1,
	LCSIntro = 0,
    L0Introduction = 1,
    L1Arrival,
    L2Mine,
    L3ResearchFacility,
	L4Sewers,
    // For testing
    LXTestingChamber,
};

void RaiseError( const string & text );

extern float mouseSens;

class Item;
class Level;
class Player;
class Menu;
class GUIProperties;

extern Player * pPlayer;
extern Menu * pMainMenu;
extern Level * pCurrentLevel;
extern GUIProperties * pGUIProp;
extern bool g_running;
extern string localizationPath;
extern bool g_showFPS;
extern int g_initialLevel;
extern bool g_continueGame;
extern ruInput::Key g_keyQuickSave;
extern ruInput::Key g_keyQuickLoad;
extern float g_resW;
extern float g_resH;
extern double g_dt;
extern float g_musicVolume;


