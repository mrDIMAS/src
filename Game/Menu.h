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

class Menu
{
public:
    enum class Page
    {
        Main,
        Options,
        Authors,
        OptionsKeys,
        OptionsCommon,
        OptionsGraphics,
        SaveGame,
        LoadGame,
    };

    explicit Menu( );
    virtual ~Menu();

    void WriteFloat( ofstream & stream, string name, float value );
    void WriteInteger( ofstream & stream, string name, int value );
    void Writestring( ofstream & stream, string name, string value );
    void WriteConfig();
    void SetPlayerControls();
    void LoadConfig();
    void CreateSliders();
    void CreateRadioButtons();
    void CreateWaitKeys();
    void CreateLists();
    void LoadSounds();
    void CreateCamera();
    void LoadTextures();
    void Show();
    void Hide( );
    void Update( );
    void SetPage( Page page );

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
    string mLoadSaveGameName;
    bool mLoadFromSave;
    float mButtonsXOffset;
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

    ruVector3 mCameraInitialPosition;
    ruVector3 mCameraAnimationOffset;
    ruVector3 mCameraAnimationNewOffset;

    GameCamera * mpCamera;
};


#endif