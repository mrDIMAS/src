#ifndef _MENU_
#define _MENU_

#include "GameCamera.h"
#include "GUIProperties.h"
#include "NumericSlider.h"
#include "RadioButton.h"
#include "WaitKeyButton.h"
#include "Player.h"
#include <Windows.h>
#include "ScrollList.h"
#include "Parser.h"
#include "ModalWindow.h"

class Menu {
private:
	enum class Page {
		Main,
		Options,
		Authors,
		OptionsKeys,
		OptionsCommon,
		OptionsGraphics,
		SaveGame,
		LoadGame,
	};
	unique_ptr<Game> & mGame;
	shared_ptr<IGUIScene> mGUIScene;
	shared_ptr<ISceneNode> mScene;
	bool mVisible;
	int mDistBetweenButtons;
	string mLoadSaveGameName;
	string mSaveGameSlotName;
	shared_ptr<ISound> mPickSound;
	int mCameraChangeTime;
	shared_ptr<ISceneNode> mCameraPos1;
	shared_ptr<ISceneNode> mCameraPos2;
	Page mPage;
	Config mConfig;
	shared_ptr<ISound> mMusic;
	Config mLocalization;
	unique_ptr<Slider> mFOVSlider;
	unique_ptr<Slider> mMasterVolume;
	unique_ptr<Slider> mMusicVolume;
	unique_ptr<Slider> mMouseSensivity;
	unique_ptr<RadioButton> mFXAAButton;
	unique_ptr<RadioButton> mParallaxButton;
	unique_ptr<RadioButton> mFPSButton;
	unique_ptr<RadioButton> mSpotShadowsButton;
	unique_ptr<RadioButton> mPointShadowsButton;
	unique_ptr<RadioButton> mHDRButton;
	unique_ptr<RadioButton> mVolumetricFogButton;
	unique_ptr<RadioButton> mBloom;
	unique_ptr<RadioButton> mSoftParticles;
	unique_ptr<RadioButton> mVSync;
	unique_ptr<RadioButton> mDynamicDirectionalLightShadows;
	unique_ptr<RadioButton> mMouseInversionX;
	unique_ptr<RadioButton> mMouseInversionY;
	unique_ptr<ScrollList> mTextureFiltering;
	unique_ptr<ScrollList> mWindowMode;
	unique_ptr<ScrollList> mSpotLightShadowMapSize;
	unique_ptr<ScrollList> mPointLightShadowMapSize;
	unique_ptr<ScrollList> mResolutionList;
	unique_ptr<ScrollList> mLanguage;
	shared_ptr<IText> mSettingsApplied;
	shared_ptr<IText> mLangSettingsApplied;
	unique_ptr<ScrollList> mDirectionalLightShadowMapSize;
	unique_ptr<WaitKeyButton> mMoveForwardKey;
	unique_ptr<WaitKeyButton> mMoveBackwardKey;
	unique_ptr<WaitKeyButton> mStrafeLeftKey;
	unique_ptr<WaitKeyButton> mStrafeRightKey;
	unique_ptr<WaitKeyButton> mJumpKey;
	unique_ptr<WaitKeyButton> mFlashLightKey;
	unique_ptr<WaitKeyButton> mRunKey;
	unique_ptr<WaitKeyButton> mInventoryKey;
	unique_ptr<WaitKeyButton> mUseKey;
	unique_ptr<WaitKeyButton> mQuickSaveKey;
	unique_ptr<WaitKeyButton> mQuickLoadKey;
	unique_ptr<WaitKeyButton> mStealthKey;
	unique_ptr<WaitKeyButton> mLookLeftKey;
	unique_ptr<WaitKeyButton> mLookRightKey;
	Vector3 mCameraInitialPosition;
	Vector3 mCameraAnimationOffset;
	Vector3 mCameraAnimationNewOffset;
	unique_ptr<GameCamera> mpCamera;
	shared_ptr<IGUINode> mCanvas;
	shared_ptr<IGUINode> mGUIMainButtonsCanvas;
	shared_ptr<IGUINode> mGUIOptionsCanvas;
	shared_ptr<IGUINode> mOptionsKeysCanvas;
	shared_ptr<IGUINode> mOptionsGraphicsCanvas;
	shared_ptr<IGUINode> mSaveGameCanvas;
	shared_ptr<IGUINode> mLoadGameCanvas;
	shared_ptr<IGUINode> mOptionsCommonCanvas;
	shared_ptr<IButton> mContinueGameButton;
	shared_ptr<IButton> mStartButton;
	shared_ptr<IButton> mSaveGameButton;
	shared_ptr<IButton> mLoadGameButton;
	shared_ptr<IButton> mOptionsButton;
	shared_ptr<IButton> mAuthorsButton;
	shared_ptr<IButton> mExitButton;
	shared_ptr<IButton> mGUIOptionsCommonButton;
	shared_ptr<IButton> mGUIOptionsControlsButton;
	shared_ptr<IButton> mGUIOptionsGraphicsButton;
	shared_ptr<IText> mGUIAuthorsText;
	shared_ptr<IRect> mAuthorsBackground;
	shared_ptr<IText> mWindowText;
	shared_ptr<IText> mCaption;
	unique_ptr<ModalWindow> mModalWindow;
	constexpr static int mSaveLoadSlotCount = 7;
	vector<string> mSaveFilesList;
	shared_ptr<IButton> mSaveGameSlot[mSaveLoadSlotCount];
	shared_ptr<IButton> mLoadGameSlot[mSaveLoadSlotCount];
	shared_ptr<IText> mLoadGameFileTime[mSaveLoadSlotCount];
	shared_ptr<IText> mSaveGameFileTime[mSaveLoadSlotCount];
	void FillListOfSaveFiles();
	void SetPage(Page page, bool hideModalWindow = true);
	void SetAuthorsPageVisible(bool state);
	void SetMainPageVisible(bool state);
	void WriteConfig();
	void ReadConfig();
	// Event handlers
	void OnStartNewGameClick();
	void OnExitGameClick();
	void OnContinueGameClick();
	void OnLoadSaveClick();
	void OnCreateSaveClick();
	void OnMusicVolumeChange();
	bool mCameraFadeActionDone;
	Delegate mCameraFadeDoneAction;
	void CameraStartFadeOut(const Delegate & onFadeDoneAction);
	void UpdateCamera();
	void DoExitGame();
	void ApplySettings();
public:
	Menu(unique_ptr<Game> & game);
	virtual ~Menu();
	void Show();
	void Hide();
	void Update();
	void SyncPlayerControls();
	void AnimateCamera();
	float GetMusicVolume();
	bool IsVisible();
};


#endif