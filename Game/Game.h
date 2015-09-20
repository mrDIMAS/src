#pragma once




enum class FootstepsType {
    Rock,
    Metal,
    Dirt
};

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
class GUI;

extern Player * pPlayer;
extern Menu * pMainMenu;
extern Level * pCurrentLevel;
extern GUI * pGUI;
extern bool g_running;
extern string localizationPath;
extern bool g_showFPS;
extern int g_initialLevel;
extern bool g_continueGame;
extern int g_keyQuickSave;
extern int g_keyQuickLoad;
extern float g_resW;
extern float g_resH;
extern double g_dt;
extern float g_musicVolume;


