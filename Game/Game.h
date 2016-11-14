#pragma once

enum class LevelName : int {
	Undefined = -1,
	Intro,
	Arrival,
	Mine,
	ResearchFacility,
	Sewers,
	Forest,
	Ending,
	Count
};

extern float gMouseSens;

extern unique_ptr<class Menu> pMainMenu;
extern unique_ptr<class GUIProperties> pGUIProp;
extern bool gRunning;
extern string gLocalizationPath;
extern bool gShowFPS;
extern LevelName g_initialLevel;
extern ruInput::Key gKeyQuickSave;
extern ruInput::Key gKeyQuickLoad;
extern float gMusicVolume;


