#pragma once

enum LevelName {
	LUndefined = -1,
	LCSIntro = 0,
	L0Introduction = 1,
	L1Arrival,
	L2Mine,
	L3ResearchFacility,
	L4Sewers,
};

extern float gMouseSens;

extern unique_ptr<class Menu> pMainMenu;
extern unique_ptr<class GUIProperties> pGUIProp;
extern bool gRunning;
extern string gLocalizationPath;
extern bool gShowFPS;
extern int g_initialLevel;
extern bool g_continueGame;
extern ruInput::Key gKeyQuickSave;
extern ruInput::Key gKeyQuickLoad;
extern float gMusicVolume;


