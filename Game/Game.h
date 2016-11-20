#pragma once

#include "FPSCounter.h"

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

class Game;

class LoadingScreen {
private:
	unique_ptr<Game> & mGame;
	shared_ptr<ruGUIScene> mScene;
	shared_ptr<ruText> mGUILoadingText;
	shared_ptr<ruRect> mGUILoadingBackground;
	shared_ptr<ruFont> mGUIFont;
public:
	LoadingScreen(unique_ptr<Game> & game, const string & loadingText);
	void Draw();
};

class Game {
private:
	unique_ptr<ruEngine> mEngine;
	unique_ptr<class Level> mLevel;
	unique_ptr<LoadingScreen> mLoadingScreen;
	unique_ptr<class Menu> mMenu;
	string mLocalizationPath;
	LevelName mInitialLevel;
	ruInput::Key mKeyQuickSave;
	ruInput::Key mKeyQuickLoad;
	shared_ptr<ruTimer> mDeltaTimer;
	shared_ptr<ruGUIScene> mOverlayScene;
	shared_ptr<ruText> mFPSText;
	FPSCounter mFPSCounter;
	float mMouseSens;
	bool mRunning;
	bool mShowFPS;
	float mMusicVolume;
	double mGameClock;
	static unique_ptr<Game> msInstance;
public:
	Game();
	~Game();
	void Start();
	void UpdateClock();
	void MainLoop();
	void Shutdown();
	string GetLocalizationPath() const;
	void SetMouseSensitivity(float sens);
	float GetMouseSensitivity() const;
	LevelName GetNewGameLevel() const;
	void ShowFPS(bool state);
	bool IsShowFPSEnabled() const;
	void SetQuickSaveKey(const ruInput::Key & key);
	void SetQuickLoadKey(const ruInput::Key & key);
	void SetMusicVolume(float vol);
	float GetMusicVolume() const;
	const unique_ptr<ruEngine> & GetEngine() const;
	const unique_ptr<Level> & GetLevel() const;
	const unique_ptr<class Menu> & GetMenu() const;
	void LoadLevel(LevelName name, bool continueFromSave = false);
	static void MakeInstance();
	static unique_ptr<Game> & Instance();
	static void DestroyInstance();
};

extern unique_ptr<class GUIProperties> pGUIProp;

