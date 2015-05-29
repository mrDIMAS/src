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

    ruNodeHandle mScene;
    bool mVisible;
    ruTextureHandle mButtonImage;
    ruTextureHandle mSmallButtonImage;
    int mDistBetweenButtons;
	int mMainButtonsAlpha;
    string mLoadSaveGameName;
	string mSaveGameSlotName;

    ruSoundHandle mPickSound;
    Page mPage;
    ruSoundHandle mMusic;
    Parser mLocalization;

    Slider * mpMasterVolume;
    Slider * mpMusicVolume;
    Slider * mpMouseSensivity;
    RadioButton * mpFXAAButton;
    RadioButton * mpFPSButton;
    RadioButton * mpSpotShadowsButton;
    RadioButton * mpHDRButton;
    ScrollList * mpTextureFiltering;

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

	ruGUINodeHandle mGUICanvas;
	ruGUINodeHandle mGUIMainButtonsCanvas;
	ruGUINodeHandle mGUIOptionsCanvas;
	ruGUINodeHandle mGUIOptionsKeysCanvas;
	ruGUINodeHandle mGUIOptionsGraphicsCanvas;
	ruGUINodeHandle mGUISaveGameCanvas;
	ruGUINodeHandle mGUILoadGameCanvas;
	ruGUINodeHandle mGUIOptionsCommonCanvas;

    ruButtonHandle mGUIContinueGameButton;
    ruButtonHandle mGUIStartButton;
    ruButtonHandle mGUISaveGameButton;
    ruButtonHandle mGUILoadGameButton;
    ruButtonHandle mGUIOptionsButton;
    ruButtonHandle mGUIAuthorsButton;
    ruButtonHandle mGUIExitButton;

    ruButtonHandle mGUIOptionsCommonButton;
    ruButtonHandle mGUIOptionsControlsButton;
    ruButtonHandle mGUIOptionsGraphicsButton;

    ruTextHandle mGUIAuthorsText;
    ruRectHandle mGUIAuthorsBackground;

	ModalWindow * mpModalWindow;

    static const int mSaveLoadSlotCount = 7;
    ruButtonHandle mGUISaveGameSlot[mSaveLoadSlotCount];
    ruButtonHandle mGUILoadGameSlot[mSaveLoadSlotCount];

    void SetPage( Page page );
    void SetOptionsPageVisible( bool state );
    void SetAuthorsPageVisible( bool state );
    void SetMainPageVisible( bool state );
    void WriteFloat( ofstream & stream, string name, float value );
    void WriteInteger( ofstream & stream, string name, int value );
    void WriteString( ofstream & stream, string name, string value );
    void WriteConfig();
    
    void LoadConfig();
    void LoadSounds();
    void CreateCamera();
    void LoadTextures();

	// Event handlers
	void OnStartNewGameClick();
	void OnExitGameClick();
	void OnContinueGameClick();
	void OnOptionsClick();
	void OnOptionsGraphicsClick();
	void OnOptionsCommonClick();
	void OnOptionsControlsClick();
	void OnHDRButtonClick();
	void OnLoadSaveClick();
	void OnSaveClick();

	bool mCameraFadeActionDone;
	ruDelegate mCameraFadeDoneAction;
	void CameraStartFadeOut( const ruDelegate & onFadeDoneAction );
	void UpdateCamera();

	void StartContinueGameFromLast();
	void DoContinueGameFromLast();
	void StartNewGame();
	void DoStartNewGame();
	void StartExitGame();
	void DoExitGame(); 
	void DoContinueGameCurrent();
	void StartLoadFromSave();
	void DoLoadFromSave();
	void DoSaveCurrentGame();
public:
    explicit Menu( );
    virtual ~Menu();
    void Show();
    void Hide( );
    void Update( );
	void SyncPlayerControls();
    void CameraFloating();

    bool IsVisible();
    Parser * GetLocalization();

};


#endif