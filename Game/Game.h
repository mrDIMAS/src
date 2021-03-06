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
	shared_ptr<IGUIScene> mScene;
	shared_ptr<IText> mGUILoadingText;
	shared_ptr<IRect> mGUILoadingBackground;
	shared_ptr<IFont> mGUIFont;
public:
	LoadingScreen(unique_ptr<Game> & game, const string & loadingText);
	void Draw();
};

class Game {
private:
	shared_ptr<ITimer> mAutoSaveTimer;
	unique_ptr<IEngine> mEngine;
	unique_ptr<class Level> mLevel;
	unique_ptr<LoadingScreen> mLoadingScreen;
	unique_ptr<class Menu> mMenu;
	string mLocalizationPath;
	LevelName mInitialLevel;
	IInput::Key mKeyQuickSave;
	IInput::Key mKeyQuickLoad;
	shared_ptr<ITimer> mDeltaTimer;
	shared_ptr<IGUIScene> mOverlayScene;
	shared_ptr<IText> mFPSText;
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
	void LoadState(const string & savefileName);
	void SaveState(const string & savefileName);
	string GetLocalizationPath() const;
	void SetMouseSensitivity(float sens);
	float GetMouseSensitivity() const;
	LevelName GetNewGameLevel() const;
	void ShowFPS(bool state);
	bool IsShowFPSEnabled() const;
	void SetQuickSaveKey(const IInput::Key & key);
	void SetQuickLoadKey(const IInput::Key & key);
	void SetMusicVolume(float vol);
	float GetMusicVolume() const;
	const unique_ptr<IEngine> & GetEngine() const;
	const unique_ptr<Level> & GetLevel() const;
	const unique_ptr<class Menu> & GetMenu() const;
	void LoadLevel(LevelName name, bool continueFromSave = false);
	static void MakeInstance();
	static unique_ptr<Game> & Instance();
	static void DestroyInstance();
};

extern unique_ptr<class GUIProperties> pGUIProp;

