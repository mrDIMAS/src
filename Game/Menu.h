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

	shared_ptr<ruGUIScene> mGUIScene;
    shared_ptr<ruSceneNode> mScene;
    bool mVisible;
    shared_ptr<ruTexture> mButtonImage;
    shared_ptr<ruTexture> mSmallButtonImage;
    int mDistBetweenButtons;
    string mLoadSaveGameName;
	string mSaveGameSlotName;

    shared_ptr<ruSound> mPickSound;
    Page mPage;
    shared_ptr<ruSound> mMusic;
    Parser mLocalization;

	unique_ptr<Slider> mFOVSlider;
    unique_ptr<Slider> mpMasterVolume;
    unique_ptr<Slider> mpMusicVolume;
    unique_ptr<Slider> mpMouseSensivity;
    unique_ptr<RadioButton> mpFXAAButton;
	unique_ptr<RadioButton> mpParallaxButton;
    unique_ptr<RadioButton> mpFPSButton;
    unique_ptr<RadioButton> mpShadowsButton;
    unique_ptr<RadioButton> mpHDRButton;
    unique_ptr<ScrollList> mpTextureFiltering;

    // Keys
    unique_ptr<WaitKeyButton> mpMoveForwardKey;
    unique_ptr<WaitKeyButton> mpMoveBackwardKey;
    unique_ptr<WaitKeyButton> mpStrafeLeftKey;
    unique_ptr<WaitKeyButton> mpStrafeRightKey;
    unique_ptr<WaitKeyButton> mpJumpKey;
    unique_ptr<WaitKeyButton> mpFlashLightKey;
    unique_ptr<WaitKeyButton> mpRunKey;
    unique_ptr<WaitKeyButton> mpInventoryKey;
    unique_ptr<WaitKeyButton> mpUseKey;
    unique_ptr<WaitKeyButton> mpQuickSaveKey;
    unique_ptr<WaitKeyButton> mpQuickLoadKey;
    unique_ptr<WaitKeyButton> mpStealthKey;
	unique_ptr<WaitKeyButton> mpLookLeftKey;
	unique_ptr<WaitKeyButton> mpLookRightKey;
    ruVector3 mCameraInitialPosition;
    ruVector3 mCameraAnimationOffset;
    ruVector3 mCameraAnimationNewOffset;

    unique_ptr<GameCamera> mpCamera;

	shared_ptr<ruGUINode> mGUICanvas;
	shared_ptr<ruGUINode> mGUIMainButtonsCanvas;
	shared_ptr<ruGUINode> mGUIOptionsCanvas;
	shared_ptr<ruGUINode> mGUIOptionsKeysCanvas;
	shared_ptr<ruGUINode> mGUIOptionsGraphicsCanvas;
	shared_ptr<ruGUINode> mGUISaveGameCanvas;
	shared_ptr<ruGUINode> mGUILoadGameCanvas;
	shared_ptr<ruGUINode> mGUIOptionsCommonCanvas;

    shared_ptr<ruButton> mGUIContinueGameButton;
    shared_ptr<ruButton> mGUIStartButton;
    shared_ptr<ruButton> mGUISaveGameButton;
    shared_ptr<ruButton> mGUILoadGameButton;
    shared_ptr<ruButton> mGUIOptionsButton;
    shared_ptr<ruButton> mGUIAuthorsButton;
    shared_ptr<ruButton> mGUIExitButton;

    shared_ptr<ruButton> mGUIOptionsCommonButton;
    shared_ptr<ruButton> mGUIOptionsControlsButton;
    shared_ptr<ruButton> mGUIOptionsGraphicsButton;

    shared_ptr<ruText> mGUIAuthorsText;
    shared_ptr<ruRect> mGUIAuthorsBackground;

	shared_ptr<ruText> mGUIWindowText;
	shared_ptr<ruText> mGUICaption;

	unique_ptr<ModalWindow> mpModalWindow;

	constexpr static int mSaveLoadSlotCount = 7;
    shared_ptr<ruButton> mGUISaveGameSlot[mSaveLoadSlotCount];
    shared_ptr<ruButton> mGUILoadGameSlot[mSaveLoadSlotCount];
	shared_ptr<ruText> mGUILoadGameFileTime[mSaveLoadSlotCount];
	shared_ptr<ruText> mGUISaveGameFileTime[mSaveLoadSlotCount];

    void SetPage( Page page, bool hideModalWindow = true );
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
		ruEngine::SetParallaxEnabled( mpParallaxButton->IsChecked() );
	}
	void OnLoadSaveClick();
	void OnSaveClick();
	void OnMouseSensivityChange();
	void OnMusicVolumeChange();
	void OnSoundVolumeChange();
	void OnFovChanged();

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