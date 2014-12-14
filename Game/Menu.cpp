#include "Menu.h"
#include "Level.h"
#include "GUI.h"
#include "Player.h"
#include "LevelArrival.h"
#include "LevelMine.h"
#include "SaveWriter.h"
#include "SaveLoader.h"
#include "Utils.h"

Menu * pMainMenu = 0;
bool g_continueGame = false;

Menu::Menu( )
{
    mLocalization.ParseFile( localizationPath + "menu.loc" );

    mScene = ruLoadScene( "data/maps/menu/menu.scene" );

    mDistBetweenButtons = 72;
    mStartPressed = false;
    mFadeSpeed = 0.1f;
    mExitPressed = false;
    mButtonsXOffset = 0;
    mVisible = true;
    mPage = Page::Main;
    mContinuePressed = false;
    mExitingGame = false;
    mReturnToGameByEsc = false;
    mLoadSaveGameName = "";
    mLoadFromSave = false;
    CreateCamera();
    LoadSounds();
    LoadTextures();
    CreateSliders();
    CreateRadioButtons();
    CreateWaitKeys();
    CreateLists();
    LoadConfig();

    ifstream file( "lastGame.save" );
    mCanContinueGameFromLast = file.good();
    file.close();

    for( int i = 0; i < ruGetWorldPointLightCount(); i++ ) {
        ruSetLightFloatingEnabled( ruGetWorldPointLight( i ), true );
        ruSetLightFloatingLimits( ruGetWorldPointLight( i ), ruVector3( -.25, -.25, -.25 ), ruVector3( .25, .25, .25 ) );
    }
}

void Menu::Show()
{
    mpCamera->MakeCurrent();
    mpCamera->FadeIn();
    ruShowNode( mScene );
    if( pCurrentLevel )
        pCurrentLevel->Hide();
    mVisible = true;
    ruShowCursor();
//    ruPausePhysics();
}

void Menu::Hide( )
{
    if( pPlayer )
        pPlayer->mpCamera->MakeCurrent();
    SetPlayerControls();
    ruHideNode( mScene );
    if( pCurrentLevel )
        pCurrentLevel->Show();
    mVisible = false;
    mPage = Page::Main;
    mButtonsXOffset = 0;
    ruHideCursor();
    ruPauseSound( mMusic );
//    ruResumePhysics();
}

void Menu::Update( )
{
    ruSetAmbientColor( ruVector3( 0, 0, 0 ));

    if( mVisible ) {
        mpCamera->Update();

        if( ruIsKeyHit( KEY_Esc ) )
            mReturnToGameByEsc = pCurrentLevel != nullptr;

        if( mStartPressed || mContinuePressed || mReturnToGameByEsc || mLoadFromSave ) {
            mpCamera->FadeOut();

            mButtonsXOffset -= 6;

            if( mpCamera->FadeComplete() ) {
                if( !pCurrentLevel && mContinuePressed )
                    SaveLoader( "lastGame.save" ).RestoreWorldState();
                if( mLoadFromSave )
                    SaveLoader( mLoadSaveGameName ).RestoreWorldState();
                if( !pCurrentLevel && mStartPressed )
                    Level::Change( g_initialLevel );
                mStartPressed = false;
                mContinuePressed = false;
                mReturnToGameByEsc = false;
                mLoadFromSave = false;
                Hide();
                return;
            }
        }

        if( mExitPressed ) {
            mExitingGame = true;
            mpCamera->FadeOut();
        }

        if( mExitingGame ) {
            mButtonsXOffset -= 12;

            if( mpCamera->FadeComplete() ) {
                g_running = false;

                WriteConfig();
            }
        }

        mCameraAnimationOffset = mCameraAnimationOffset.Lerp( mCameraAnimationNewOffset, 0.0085f );

        if( ( mCameraAnimationOffset - mCameraAnimationNewOffset ).Length2() < 0.025 )
            mCameraAnimationNewOffset = ruVector3( frandom( -1.5, 1.5 ), frandom( -1.5, 1.5 ), frandom( -1.5, 1.5 ) );

        ruSetNodePosition( mpCamera->mNode, mCameraInitialPosition + mCameraAnimationOffset );

        int mainButtonsX = mButtonsXOffset + 20;
        int startOffsetIfInGame = pCurrentLevel ? 0.5 * mDistBetweenButtons : 0;

        ruGUIState continueGame;
        ruGUIState start;
        ruGUIState saveGame;
        ruGUIState loadGame;
        ruGUIState options;
        ruGUIState authors;
        ruGUIState exit;

		int saveGamePosX = ( mPage == Page::SaveGame ) ? mainButtonsX + 20 : mainButtonsX;
		int loadGamePosX = ( mPage == Page::LoadGame ) ? mainButtonsX + 20 : mainButtonsX;
		int optionsPosX = ( mPage == Page::Options || mPage == Page::OptionsGraphics || mPage == Page::OptionsKeys || mPage == Page::OptionsCommon ) ? mainButtonsX + 20 : mainButtonsX;
		if( pCurrentLevel || mCanContinueGameFromLast )
			continueGame = ruDrawGUIButton( mainButtonsX, g_resH - 4.0 * mDistBetweenButtons + startOffsetIfInGame, 128, 32, mButtonImage, mLocalization.GetString( "continueButton" ), pGUI->mFont, ruVector3( 0, 255, 0 ), 1 );
		if( !pCurrentLevel )
			start = ruDrawGUIButton( mainButtonsX, g_resH - 3.5 * mDistBetweenButtons, 128, 32, mButtonImage, mLocalization.GetString( "startButton" ), pGUI->mFont, ruVector3( 0, 255, 0 ), 1 );
		saveGame = ruDrawGUIButton( saveGamePosX, g_resH - 3.0 * mDistBetweenButtons, 128, 32, mButtonImage, mLocalization.GetString( "saveButton" ), pGUI->mFont, ruVector3( 0, 255, 0 ), 1 );
		loadGame = ruDrawGUIButton( loadGamePosX, g_resH - 2.5 * mDistBetweenButtons, 128, 32, mButtonImage, mLocalization.GetString( "loadButton" ), pGUI->mFont, ruVector3( 0, 255, 0 ), 1 );
		options = ruDrawGUIButton( optionsPosX, g_resH - 2.0 * mDistBetweenButtons, 128, 32, mButtonImage, mLocalization.GetString( "optionsButton" ), pGUI->mFont, ruVector3( 0, 255, 0 ), 1 );
		authors = ruDrawGUIButton( mainButtonsX, g_resH - 1.5 * mDistBetweenButtons, 128, 32, mButtonImage, mLocalization.GetString( "authorsButton" ), pGUI->mFont, ruVector3( 0, 255, 0 ), 1 );
		exit = ruDrawGUIButton( mainButtonsX, g_resH - 1.0 * mDistBetweenButtons, 128, 32, mButtonImage, mLocalization.GetString( "exitButton" ), pGUI->mFont, ruVector3( 0, 255, 0 ), 1 );

        if( mPage == Page::Authors ) {
            int w = 500;
            int h = 400;
            int x = ( g_resW - w ) / 2;
            int y = ( g_resH - h ) / 2;
            ruDrawGUIRect( x, y, w, h, ruTextureHandle::Empty() );
            ruDrawGUIText( mLocalization.GetString( "authorsText" ), x + 50, y + 50, w - 100, h - 100, pGUI->mFont, ruVector3( 255, 127, 127 ), 0 );
        }

        if( mPage == Page::Options ) { // options window
            ruGUIState optionsCommon = ruDrawGUIButton( mButtonsXOffset + 200, g_resH - 2.5 * mDistBetweenButtons, 128, 32, mButtonImage, mLocalization.GetString( "commonSettings" ), pGUI->mFont, ruVector3( 0, 255, 0 ), 1 );
            ruGUIState optionsKeys = ruDrawGUIButton( mButtonsXOffset + 200, g_resH - 2.0 * mDistBetweenButtons, 128, 32, mButtonImage, mLocalization.GetString( "controls" ), pGUI->mFont, ruVector3( 0, 255, 0 ), 1 );
            ruGUIState optionsGraphics = ruDrawGUIButton( mButtonsXOffset + 200, g_resH - 1.5 * mDistBetweenButtons, 128, 32, mButtonImage, mLocalization.GetString( "graphics" ), pGUI->mFont, ruVector3( 0, 255, 0 ), 1 );

            if( optionsCommon.mouseLeftClicked )
                SetPage( Page::OptionsCommon );

            if( optionsKeys.mouseLeftClicked )
                SetPage( Page::OptionsKeys );

            if( optionsGraphics.mouseLeftClicked )
                SetPage( Page::OptionsGraphics );
        }

        if( mPage == Page::OptionsCommon ) {
            float x = mButtonsXOffset + 200;
            float y = g_resH - 2.5 * mDistBetweenButtons;

            float buttonWidth = 80;
            float buttonHeight = 32;
            float textX = x + buttonWidth * 1.5f;

            mpMouseSensivity->Draw( x, y - 3.0f * buttonHeight, mButtonImage, mLocalization.GetString( "mouseSens" ));
            mouseSens = mpMouseSensivity->GetValue() / 100.0f;

            buttonWidth = 32;

            mpMasterVolume->Draw( x, y, mButtonImage, mLocalization.GetString( "masterVolume" ));
            ruSetMasterVolume( mpMasterVolume->GetValue() / 100.0f );

            y = y + 1.5f * buttonHeight;

            mpMusicVolume->Draw( x, y, mButtonImage, mLocalization.GetString(  "musicVolume" ));
            ruSetSoundVolume( mMusic, mpMusicVolume->GetValue() / 100.0f );
            g_musicVolume = mpMusicVolume->GetValue() / 100.0f;
            if( pCurrentLevel )
                ruSetSoundVolume( pCurrentLevel->mMusic, g_musicVolume );
        }

        if( mPage == Page::LoadGame ) {
            float x = mButtonsXOffset + 200;
            float y = g_resH - 4.0 * mDistBetweenButtons;

            float buttonWidth = 80;
            float buttonHeight = 32;
            float textX = x + buttonWidth * 1.5f;

            vector< string > nameList;
            GetFilesWithDefExt( "*.save", nameList );

            for( int i = 0; i < nameList.size(); i++ ) {
                ruGUIState save = ruDrawGUIButton( x, y, 128, 32, mButtonImage, nameList[i].c_str(), pGUI->mFont, ruVector3( 0, 255, 0 ), 1 );
                if( save.mouseLeftClicked ) {
                    mLoadSaveGameName = nameList[i];
                    mLoadFromSave = true;
                    SetPage( Page::Main );
                }
                y += 1.1f * buttonHeight;
            }
        }

        if( mPage == Page::SaveGame ) {
            float x = mButtonsXOffset + 200;
            float y = g_resH - 4.0 * mDistBetweenButtons;

            float buttonWidth = 80;
            float buttonHeight = 32;
            float textX = x + buttonWidth * 1.5f;

            vector< string > nameList;
            GetFilesWithDefExt( "*.save", nameList );

            for( int iName = nameList.size() - 1; iName < 6; iName++ ) {
                string saveName = "Slot";
                saveName += ( (char)iName + (char)'0' );
                saveName += ".save";
                nameList.push_back( saveName );
            }

            for( int iName = 0; iName < nameList.size(); iName++ ) {
                ruGUIState save = ruDrawGUIButton( x, y, 128, 32, mButtonImage, nameList[iName].c_str(), pGUI->mFont, ruVector3( 0, 255, 0 ), 1 );
                if( save.mouseLeftClicked ) {
                    if( pCurrentLevel ) {
                        SaveWriter( nameList[iName] ).SaveWorldState();
                        SetPage( Page::Main );
                        break;
                    }
                }
                y += 1.1f * buttonHeight;
            }
        }

        if( mPage == Page::OptionsGraphics ) {
            // first column
            float x = mButtonsXOffset + 200;
            float y = g_resH - 2.5 * mDistBetweenButtons;

            // FXAA Options
            mpFXAAButton->Draw( x + 60, y, mButtonImage, mLocalization.GetString( "fxaa" ));
            if( mpFXAAButton->IsEnabled() )
                ruEnableFXAA();
            else
                ruDisableFXAA();

            // show fps
            y = g_resH - 2.0 * mDistBetweenButtons;
            mpFPSButton->Draw( x + 60, y, mButtonImage, mLocalization.GetString( "showFPS" ));
            g_showFPS = mpFPSButton->IsEnabled();

            // texture filtering
            y = g_resH - 2.0 * mDistBetweenButtons;
            mpTextureFiltering->Draw( x, y, mButtonImage, mLocalization.GetString( "filtering" ));
            if( mpTextureFiltering->GetCurrentValue() == 0 )
                ruSetRendererTextureFiltering( ruTextureFilter::Anisotropic, ruGetRendererMaxAnisotropy() );
            else
                ruSetRendererTextureFiltering( ruTextureFilter::Linear, 0 );

            y = g_resH - 1.5 * mDistBetweenButtons;
            mpGraphicsQuality->Draw( x, y, mButtonImage, mLocalization.GetString( "graphQuality"));
            if( mpGraphicsQuality->GetCurrentValue() == 0 )  // low
                ruSetRenderQuality( 0 );
            else   // high
                ruSetRenderQuality( 1 );
            // second column
            x += 280;

            y = g_resH - 2.5 * mDistBetweenButtons;
            mpPointShadowsButton->Draw( x, y, mButtonImage, mLocalization.GetString( "pointLightShadows" ));
            ruEnablePointLightShadows( mpPointShadowsButton->IsEnabled() );

            y = g_resH - 2.0 * mDistBetweenButtons;
            mpSpotShadowsButton->Draw( x, y, mButtonImage, mLocalization.GetString( "spotLightShadows" ));
            ruEnableSpotLightShadows( mpSpotShadowsButton->IsEnabled() );

            // hdr
            y = g_resH - 1.5 * mDistBetweenButtons;
            mpHDRButton->Draw( x, y, mButtonImage, mLocalization.GetString( "hdr" ));
            ruSetHDREnabled( mpHDRButton->IsEnabled() );
        }

        if( mPage == Page::OptionsKeys ) {
            float x = mButtonsXOffset + 200;
            float y = g_resH - 2.5 * mDistBetweenButtons;

            // First column
            mpMoveForwardKey->Draw( x, y, mSmallButtonImage, mLocalization.GetString( "forward" ) );

            y += 32 * 1.1f;
            mpMoveBackwardKey->Draw( x, y, mSmallButtonImage, mLocalization.GetString( "backward" ) );

            y += 32 * 1.1f;
            mpStrafeLeftKey->Draw( x, y, mSmallButtonImage, mLocalization.GetString( "strafeLeft" ) );

            y += 32 * 1.1f;
            mpStrafeRightKey->Draw( x, y, mSmallButtonImage, mLocalization.GetString( "strafeRight" ) );

            // Second column
            x += 150;
            y = g_resH - 2.5 * mDistBetweenButtons;

            mpJumpKey->Draw( x, y, mSmallButtonImage, mLocalization.GetString( "jump" ) );

            y += 32 * 1.1f;
            mpFlashLightKey->Draw( x, y, mSmallButtonImage, mLocalization.GetString( "flashLight" ) );

            y += 32 * 1.1f;
            mpRunKey->Draw( x, y, mSmallButtonImage, mLocalization.GetString( "run" ) );

            y += 32 * 1.1f;
            mpInventoryKey->Draw( x, y, mSmallButtonImage, mLocalization.GetString( "inventory" ) );

            // Third column
            x += 150;
            y = g_resH - 2.5 * mDistBetweenButtons;

            mpUseKey->Draw( x, y, mSmallButtonImage, mLocalization.GetString( "use" ) );

            y += 32 * 1.1f;
            mpQuickSaveKey->Draw( x, y, mSmallButtonImage, mLocalization.GetString( "quickSave" ));

            y += 32 * 1.1f;
            mpQuickLoadKey->Draw( x, y, mSmallButtonImage, mLocalization.GetString( "quickLoad" ) );

            y += 32 * 1.1f;
            mpStealthKey->Draw( x, y, mSmallButtonImage, mLocalization.GetString( "stealth" ) );
        }

        if( !mStartPressed && !mReturnToGameByEsc ) {
            ruPlaySound( mMusic );
            if( start.mouseLeftClicked ) {
                mStartPressed = true;
                SetPage( Page::Main );
            }
            if( exit.mouseLeftClicked ) {
                mExitPressed = true;
                if( pPlayer && pCurrentLevel ) {
                    if( !pPlayer->mDead )
                        SaveWriter( "lastGame.save" ).SaveWorldState();
                }
                SetPage( Page::Main );
            }
            if( continueGame.mouseLeftClicked ) {
                mContinuePressed = true;
                SetPage( Page::Main );
            }
            if( loadGame.mouseLeftClicked )
                SetPage( Page::LoadGame );
            if( saveGame.mouseLeftClicked )
                SetPage( Page::SaveGame );
            if( options.mouseLeftClicked )
                SetPage( Page::Options );
            if( authors.mouseLeftClicked )
                SetPage( Page::Authors );
        }
    } else {
        if( ruIsKeyHit( KEY_Esc ) )
            pMainMenu->Show();
    }
}

void Menu::SetPage( Page page )
{
    if( mPage == page )
        mPage = Page::Main;
    else
        mPage = page;
}

void Menu::LoadTextures()
{
    mButtonImage = ruGetTexture( "data/gui/button.png" );
    mSmallButtonImage = ruGetTexture( "data/gui/smallbutton.png" );
}

void Menu::CreateCamera()
{
    mpCamera = new GameCamera;
    mCameraInitialPosition = ruGetNodePosition( ruFindByName( "Camera") );
    mCameraAnimationNewOffset = ruVector3( 0.5, 0.5, 0.5 );
}

void Menu::LoadSounds()
{
    mPickSound = ruLoadSound2D( "data/sounds/menupick.ogg" );
    mMusic = ruLoadMusic( "data/music/menu.ogg" );
}

void Menu::CreateWaitKeys()
{
    mpMoveForwardKey = new WaitKeyButton();
    mpMoveBackwardKey = new WaitKeyButton();
    mpStrafeLeftKey = new WaitKeyButton();
    mpStrafeRightKey = new WaitKeyButton();
    mpJumpKey = new WaitKeyButton();
    mpFlashLightKey = new WaitKeyButton();
    mpRunKey = new WaitKeyButton();
    mpInventoryKey = new WaitKeyButton();
    mpUseKey = new WaitKeyButton();
    mpQuickLoadKey = new WaitKeyButton();
    mpQuickSaveKey = new WaitKeyButton();
    mpStealthKey = new WaitKeyButton();
}

void Menu::CreateRadioButtons()
{
    mpFXAAButton = new RadioButton;
    mpFPSButton = new RadioButton;
    mpPointShadowsButton = new RadioButton;
    mpSpotShadowsButton = new RadioButton;
    mpHDRButton = new RadioButton;
}

void Menu::CreateSliders()
{
    mpMasterVolume = new NumericSlider( 0, 100, 2.5f );
    mpMusicVolume = new NumericSlider( 0, 100, 2.5f );
    mpMouseSensivity = new NumericSlider( 0, 100, 2.5f );
}

void Menu::LoadConfig()
{
    // Load config
    Parser config;
    config.ParseFile( "config.cfg" );

    if( !config.Empty() ) {
        mpMasterVolume->SetValue( config.GetNumber( "masterVolume" ));
        
		mpMusicVolume->SetValue( config.GetNumber( "musicVolume" ));
		g_musicVolume = mpMusicVolume->GetValue();

        mpMouseSensivity->SetValue( config.GetNumber( "mouseSens" ));
        mpFXAAButton->SetEnabled( config.GetNumber( "fxaaEnabled" ) != 0 );
        if( mpFXAAButton->IsEnabled() )
            ruEnableFXAA();
        else
            ruDisableFXAA();;
        mpMoveForwardKey->SetSelected( config.GetNumber( "keyMoveForward" ) );
        mpMoveBackwardKey->SetSelected( config.GetNumber( "keyMoveBackward" ) );
        mpStrafeLeftKey->SetSelected( config.GetNumber( "keyStrafeLeft" ) );
        mpStrafeRightKey->SetSelected( config.GetNumber( "keyStrafeRight" ) );
        mpJumpKey->SetSelected( config.GetNumber( "keyJump" ) );
        mpRunKey->SetSelected( config.GetNumber( "keyRun" ) );
        mpFlashLightKey->SetSelected( config.GetNumber( "keyFlashLight" ) );
        mpInventoryKey->SetSelected( config.GetNumber( "keyInventory" ) );
        mpUseKey->SetSelected( config.GetNumber( "keyUse" ) );

        mpQuickSaveKey->SetSelected( config.GetNumber( "keyQuickSave" ) );
		g_keyQuickSave = mpQuickSaveKey->GetSelectedKey();

        mpQuickLoadKey->SetSelected( config.GetNumber( "keyQuickLoad" ) );
		g_keyQuickLoad = mpQuickLoadKey->GetSelectedKey();

        mpStealthKey->SetSelected( config.GetNumber( "keyStealth" ) );

        mpSpotShadowsButton->SetEnabled( config.GetNumber( "spotShadowsEnabled" ) != 0 );
        ruEnableSpotLightShadows( mpSpotShadowsButton->IsEnabled() );

        mpPointShadowsButton->SetEnabled( config.GetNumber( "pointShadowsEnabled" ) != 0 );
        ruEnablePointLightShadows( mpPointShadowsButton->IsEnabled() );

        ruSetMasterVolume( mpMasterVolume->GetValue() / 100.0f );
        ruSetSoundVolume( mMusic, mpMusicVolume->GetValue() / 100.0f );

		mpHDRButton->SetEnabled( config.GetNumber( "hdrEnabled" ) != 0 );
        ruSetHDREnabled( mpHDRButton->IsEnabled() );

		mpGraphicsQuality->SetCurrentValue( config.GetNumber( "graphicsQuality" ));
		ruSetRenderQuality( mpGraphicsQuality->GetCurrentValue() );

		mpTextureFiltering->SetCurrentValue( config.GetNumber( "textureFiltering" ));
		ruSetRendererTextureFiltering( mpTextureFiltering->GetCurrentValue(), ruGetRendererMaxAnisotropy() );
    }
}

void Menu::SetPlayerControls()
{
    if( pPlayer ) {
        pPlayer->mKeyMoveForward = mpMoveForwardKey->GetSelectedKey();
        pPlayer->mKeyMoveBackward = mpMoveBackwardKey->GetSelectedKey();
        pPlayer->mKeyStrafeLeft = mpStrafeLeftKey->GetSelectedKey();
        pPlayer->mKeyStrafeRight = mpStrafeRightKey->GetSelectedKey();
        pPlayer->mKeyJump = mpJumpKey->GetSelectedKey();
        pPlayer->mKeyRun = mpRunKey->GetSelectedKey();
        pPlayer->mKeyFlashLight = mpFlashLightKey->GetSelectedKey();
        pPlayer->mKeyInventory = mpInventoryKey->GetSelectedKey();
        pPlayer->mKeyUse = mpUseKey->GetSelectedKey();
        pPlayer->mKeyStealth = mpStealthKey->GetSelectedKey();
    }
}

void Menu::WriteConfig()
{
    ofstream config( "config.cfg" );
    WriteFloat( config, "mouseSens", mpMouseSensivity->GetValue() );
    WriteFloat( config, "masterVolume", mpMasterVolume->GetValue() );
    WriteFloat( config, "musicVolume", mpMusicVolume->GetValue() );
    WriteInteger( config, "fxaaEnabled", mpFXAAButton->IsEnabled() ? 1 : 0 );
    WriteInteger( config, "keyMoveForward", mpMoveForwardKey->GetSelectedKey() );
    WriteInteger( config, "keyMoveBackward", mpMoveBackwardKey->GetSelectedKey() );
    WriteInteger( config, "keyStrafeLeft", mpStrafeLeftKey->GetSelectedKey() );
    WriteInteger( config, "keyStrafeRight", mpStrafeRightKey->GetSelectedKey() );
    WriteInteger( config, "keyJump", mpJumpKey->GetSelectedKey() );
    WriteInteger( config, "keyFlashLight", mpFlashLightKey->GetSelectedKey() );
    WriteInteger( config, "keyRun", mpRunKey->GetSelectedKey() );
    WriteInteger( config, "keyInventory", mpInventoryKey->GetSelectedKey() );
    WriteInteger( config, "keyUse", mpUseKey->GetSelectedKey() );
    WriteInteger( config, "keyQuickSave", mpQuickSaveKey->GetSelectedKey() );
    WriteInteger( config, "keyQuickLoad", mpQuickLoadKey->GetSelectedKey() );
    WriteInteger( config, "spotShadowsEnabled", ruIsSpotLightShadowsEnabled() ? 1 : 0 );
    WriteInteger( config, "pointShadowsEnabled", ruIsPointLightShadowsEnabled() ? 1 : 0 );
    WriteInteger( config, "hdrEnabled", ruIsHDREnabled() ? 1 : 0  );
    WriteInteger( config, "keyStealth", mpStealthKey->GetSelectedKey() );
	WriteInteger( config, "graphicsQuality", mpGraphicsQuality->GetCurrentValue() );
	WriteInteger( config, "textureFiltering", mpTextureFiltering->GetCurrentValue() );
    config.close();
}

void Menu::Writestring( ofstream & stream, string name, string value )
{
    stream << name << "=\"" << value << "\";\n";
}

void Menu::WriteInteger( ofstream & stream, string name, int value )
{
    stream << name << "=\"" << value << "\";\n";
}

void Menu::WriteFloat( ofstream & stream, string name, float value )
{
    stream << name << "=\"" << value << "\";\n";
}

void Menu::CreateLists()
{
    mpTextureFiltering = new ScrollList();

    mpTextureFiltering->AddValue( mLocalization.GetString( "anisotropic" ));
    mpTextureFiltering->AddValue( mLocalization.GetString( "trilinear" ));

    mpGraphicsQuality = new ScrollList();
    mpGraphicsQuality->AddValue( mLocalization.GetString( "gqLow" ));
    mpGraphicsQuality->AddValue( mLocalization.GetString( "gqHigh" ));
}

Menu::~Menu()
{
    delete mpMasterVolume;
    delete mpMouseSensivity;
    delete mpMusicVolume;
    delete mpFXAAButton;
    delete mpTextureFiltering;
    delete mpMoveForwardKey;
    delete mpMoveBackwardKey;
    delete mpStrafeLeftKey;
    delete mpStrafeRightKey;
    delete mpJumpKey;
    delete mpInventoryKey;
    delete mpRunKey;
    delete mpFlashLightKey;
    delete mpCamera;
    delete mpUseKey;
    delete mpQuickLoadKey;
    delete mpQuickSaveKey;
    delete mpPointShadowsButton;
    delete mpHDRButton;
    delete mpSpotShadowsButton;
    delete mpStealthKey;
    delete mpFPSButton;
    delete mpGraphicsQuality;
}
