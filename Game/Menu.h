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
	shared_ptr<ruGUIScene> mGUIScene;
	shared_ptr<ruSceneNode> mScene;
	bool mVisible;
	int mDistBetweenButtons;
	string mLoadSaveGameName;
	string mSaveGameSlotName;
	shared_ptr<ruSound> mPickSound;
	Page mPage;
	ruConfig mConfig;
	shared_ptr<ruSound> mMusic;
	ruConfig mLocalization;
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
	unique_ptr<ScrollList> mTextureFiltering;
	unique_ptr<ScrollList> mWindowMode;
	unique_ptr<ScrollList> mSpotLightShadowMapSize;
	unique_ptr<ScrollList> mPointLightShadowMapSize;
	unique_ptr<ScrollList> mResolutionList;
	unique_ptr<ScrollList> mLanguage;
	shared_ptr<ruText> mSettingsApplied;
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
	ruVector3 mCameraInitialPosition;
	ruVector3 mCameraAnimationOffset;
	ruVector3 mCameraAnimationNewOffset;
	unique_ptr<GameCamera> mpCamera;
	shared_ptr<ruGUINode> mCanvas;
	shared_ptr<ruGUINode> mGUIMainButtonsCanvas;
	shared_ptr<ruGUINode> mGUIOptionsCanvas;
	shared_ptr<ruGUINode> mOptionsKeysCanvas;
	shared_ptr<ruGUINode> mOptionsGraphicsCanvas;
	shared_ptr<ruGUINode> mSaveGameCanvas;
	shared_ptr<ruGUINode> mLoadGameCanvas;
	shared_ptr<ruGUINode> mOptionsCommonCanvas;
	shared_ptr<ruButton> mContinueGameButton;
	shared_ptr<ruButton> mStartButton;
	shared_ptr<ruButton> mSaveGameButton;
	shared_ptr<ruButton> mLoadGameButton;
	shared_ptr<ruButton> mOptionsButton;
	shared_ptr<ruButton> mAuthorsButton;
	shared_ptr<ruButton> mExitButton;
	shared_ptr<ruButton> mGUIOptionsCommonButton;
	shared_ptr<ruButton> mGUIOptionsControlsButton;
	shared_ptr<ruButton> mGUIOptionsGraphicsButton;
	shared_ptr<ruText> mGUIAuthorsText;
	shared_ptr<ruRect> mAuthorsBackground;
	shared_ptr<ruText> mWindowText;
	shared_ptr<ruText> mCaption;
	unique_ptr<ModalWindow> mModalWindow;
	constexpr static int mSaveLoadSlotCount = 7;
	vector<string> mSaveFilesList;
	shared_ptr<ruButton> mSaveGameSlot[mSaveLoadSlotCount];
	shared_ptr<ruButton> mLoadGameSlot[mSaveLoadSlotCount];
	shared_ptr<ruText> mLoadGameFileTime[mSaveLoadSlotCount];
	shared_ptr<ruText> mSaveGameFileTime[mSaveLoadSlotCount];
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
	ruDelegate mCameraFadeDoneAction;
	void CameraStartFadeOut(const ruDelegate & onFadeDoneAction);
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