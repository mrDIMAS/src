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

    shared_ptr<ruSceneNode> mScene;
    bool mVisible;
    shared_ptr<ruTexture> mButtonImage;
    shared_ptr<ruTexture> mSmallButtonImage;
    int mDistBetweenButtons;
	int mMainButtonsAlpha;
    string mLoadSaveGameName;
	string mSaveGameSlotName;

    ruSound mPickSound;
    Page mPage;
    ruSound mMusic;
    Parser mLocalization;

    Slider * mpMasterVolume;
    Slider * mpMusicVolume;
    Slider * mpMouseSensivity;
    RadioButton * mpFXAAButton;
	RadioButton * mpParallaxButton;
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

	ruGUINode * mGUICanvas;
	ruGUINode * mGUIMainButtonsCanvas;
	ruGUINode * mGUIOptionsCanvas;
	ruGUINode * mGUIOptionsKeysCanvas;
	ruGUINode * mGUIOptionsGraphicsCanvas;
	ruGUINode * mGUISaveGameCanvas;
	ruGUINode * mGUILoadGameCanvas;
	ruGUINode * mGUIOptionsCommonCanvas;

    ruButton * mGUIContinueGameButton;
    ruButton * mGUIStartButton;
    ruButton * mGUISaveGameButton;
    ruButton * mGUILoadGameButton;
    ruButton * mGUIOptionsButton;
    ruButton * mGUIAuthorsButton;
    ruButton * mGUIExitButton;

    ruButton * mGUIOptionsCommonButton;
    ruButton * mGUIOptionsControlsButton;
    ruButton * mGUIOptionsGraphicsButton;

    ruText * mGUIAuthorsText;
    ruRect * mGUIAuthorsBackground;

	ruText * mGUIWindowText;
	ruText * mGUICaption;

	ModalWindow * mpModalWindow;

    static const int mSaveLoadSlotCount = 7;
    ruButton * mGUISaveGameSlot[mSaveLoadSlotCount];
    ruButton * mGUILoadGameSlot[mSaveLoadSlotCount];

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
	void OnParallaxButtonClick() {
		ruEngine::SetParallaxEnabled( mpParallaxButton->IsEnabled() );
	}
	void OnLoadSaveClick();
	void OnSaveClick();
	void OnMouseSensivityChange();
	void OnMusicVolumeChange();
	void OnSoundVolumeChange();

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
	float GetMusicVolume();
    bool IsVisible();
    Parser * GetLocalization();

};


#endif