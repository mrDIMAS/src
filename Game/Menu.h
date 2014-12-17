#ifndef _MENU_
#define _MENU_

#include "GameCamera.h"
#include "GUI.h"
#include "NumericSlider.h"
#include "RadioButton.h"
#include "WaitKeyButton.h"
#include "Player.h"
#include <Windows.h>
#include "ScrollList.h"
#include "Parser.h"

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

    ruNodeHandle mScene;
    bool mVisible;
    ruTextureHandle mButtonImage;
    ruTextureHandle mSmallButtonImage;
    int mDistBetweenButtons;

    float mFadeSpeed;
    bool mCanContinueGameFromLast;
    bool mStartPressed;
    bool mExitPressed;
    bool mContinuePressed;
    bool mReturnToGameByEsc;
	bool mExitingGame;
	int mMainButtonsAlpha;
    string mLoadSaveGameName;
    bool mLoadFromSave;
    ruSoundHandle mPickSound;
    Page mPage;
    ruSoundHandle mMusic;
    Parser mLocalization;

    NumericSlider * mpMasterVolume;
    NumericSlider * mpMusicVolume;
    NumericSlider * mpMouseSensivity;
    RadioButton * mpFXAAButton;
    RadioButton * mpFPSButton;
    RadioButton * mpPointShadowsButton;
    RadioButton * mpSpotShadowsButton;
    RadioButton * mpHDRButton;
    ScrollList * mpTextureFiltering;
    ScrollList * mpGraphicsQuality;

    // Keys
    WaitKeyButton * mpMoveForwardKey;
    WaitKeyButton * mpMoveBackwardKey;
    WaitKeyButton * mpStrafeLeftKey;
    WaitKeyButton * mpStrafeRightKey;
    WaitKeyButton * mpJumpKey;
    WaitKeyButton * mpFlashLightKey;
    WaitKeyButton * mpRunKey;
    WaitKeyButton * mpInventoryKey;
    WaitKeyButton * mpUseKey;
    WaitKeyButton * mpQuickSaveKey;
    WaitKeyButton * mpQuickLoadKey;
    WaitKeyButton * mpStealthKey;
	WaitKeyButton * mpLookLeftKey;
	WaitKeyButton * mpLookRightKey;
    ruVector3 mCameraInitialPosition;
    ruVector3 mCameraAnimationOffset;
    ruVector3 mCameraAnimationNewOffset;

    GameCamera * mpCamera;

    ruButtonHandle mGUIContinueGameButton;
    ruButtonHandle mGUIStartButton;
    ruButtonHandle mGUISaveGameButton;
    ruButtonHandle mGUILoadGameButton;
    ruButtonHandle mGUIOptionsButton;
    ruButtonHandle mGUIAuthorsButton;
    ruButtonHandle mGUIExitButton;

    ruButtonHandle mGUIOptionsCommonButton;
    ruButtonHandle mGUIOptionsKeysButton;
    ruButtonHandle mGUIOptionsGraphicsButton;

    ruTextHandle mGUIAuthorsText;
    ruRectHandle mGUIAuthorsBackground;


    static const int mSaveLoadSlotCount = 7;
    ruButtonHandle mGUISaveGameSlot[mSaveLoadSlotCount];
    ruButtonHandle mGUILoadGameSlot[mSaveLoadSlotCount];

    void SetPage( Page page );
    void SetOptionsPageVisible( bool state );
    void SetAuthorsPageVisible( bool state );
    void SetOptionsGraphicsPageVisible( bool state );
    void SetMainPageVisible( bool state );
    void SetOptionsKeysPageVisible( bool state );
    void SetOptionsCommonPageVisible( bool state );
    void SetSaveSlotsVisible( bool state );
    void SetLoadSlotsVisible( bool state );
    void SetAllVisible( bool state );
    void WriteFloat( ofstream & stream, string name, float value );
    void WriteInteger( ofstream & stream, string name, int value );
    void WriteString( ofstream & stream, string name, string value );
    void WriteConfig();
    void SetPlayerControls();
    void LoadConfig();
    void CreateSliders();
    void CreateWaitKeys();
    void CreateLists();
    void LoadSounds();
    void CreateCamera();
    void LoadTextures();
public:
    explicit Menu( );
    virtual ~Menu();
    void Show();
    void Hide( );
    void Update( );

    void CameraFloating();

    bool IsVisible();
    Parser * GetLocalization();

};


#endif