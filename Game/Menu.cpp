#include "Precompiled.h"

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

Menu::Menu( ) {
    mLocalization.ParseFile( localizationPath + "menu.loc" );

    mScene = ruLoadScene( "data/maps/menu/menu.scene" );

    mDistBetweenButtons = 72;
    mVisible = true;
    mPage = Page::Main;
    mLoadSaveGameName = "";

    CreateCamera();
    LoadSounds();
    LoadTextures();

    for( int i = 0; i < ruGetWorldPointLightCount(); i++ ) {
        ruSetLightFloatingEnabled( ruGetWorldPointLight( i ), true );
        ruSetLightFloatingLimits( ruGetWorldPointLight( i ), ruVector3( -.25, -.25, -.25 ), ruVector3( .25, .25, .25 ) );
    }
		
    const float buttonHeight = 32;
    const float buttonWidth = 128;
	const float buttonXOffset = 5;
	const ruVector3 buttonColor = ruVector3( 255, 255, 255 );
	const ruVector3 canvasColor = ruVector3( 19, 83, 28 );
	// Setup
	mGUICanvas = ruCreateGUIRect( 0, 0, 0, 0, ruTextureHandle::Empty() );
	ruSetGUINodeChildAlphaControl( mGUICanvas, true );
	{
		mGUIMainButtonsCanvas = ruCreateGUIRect( 20, g_resH - 4.0 * mDistBetweenButtons, buttonWidth + buttonXOffset, buttonHeight * 8, ruGetTexture( "data/gui/tab.png" ), canvasColor );    
		ruSetGUINodeChildAlphaControl( mGUIMainButtonsCanvas, true );
		ruAttachGUINode( mGUIMainButtonsCanvas, mGUICanvas );
		{
			mGUIContinueGameButton = ruCreateGUIButton( buttonXOffset, 0, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString( "continueButton" ), pGUI->mFont, buttonColor, 1 );
			ruAttachGUINode( mGUIContinueGameButton, mGUIMainButtonsCanvas );
			ruAddGUINodeAction( mGUIContinueGameButton, ruGUIAction::OnClick, ruDelegate::Bind( this, &Menu::OnContinueGameClick ));

			mGUIStartButton = ruCreateGUIButton( buttonXOffset, 0, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString( "startButton" ), pGUI->mFont, buttonColor, 1 );
			ruAttachGUINode( mGUIStartButton, mGUIMainButtonsCanvas );
			ruAddGUINodeAction( mGUIStartButton, ruGUIAction::OnClick, ruDelegate::Bind( this, &Menu::OnStartNewGameClick ));

			mGUISaveGameButton = ruCreateGUIButton( buttonXOffset, 0, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString( "saveButton" ), pGUI->mFont, buttonColor, 1 );
			ruAttachGUINode( mGUISaveGameButton, mGUIMainButtonsCanvas );

			mGUILoadGameButton = ruCreateGUIButton( buttonXOffset, 0, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString( "loadButton" ), pGUI->mFont, buttonColor, 1 );
			ruAttachGUINode( mGUILoadGameButton, mGUIMainButtonsCanvas );

			mGUIOptionsButton = ruCreateGUIButton( buttonXOffset, 0, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString( "optionsButton" ), pGUI->mFont, buttonColor, 1 );
			ruAttachGUINode( mGUIOptionsButton, mGUIMainButtonsCanvas );
			ruAddGUINodeAction( mGUIOptionsButton, ruGUIAction::OnClick, ruDelegate::Bind( this, &Menu::OnOptionsClick ));

			mGUIAuthorsButton = ruCreateGUIButton( buttonXOffset, 0, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString( "authorsButton" ), pGUI->mFont, buttonColor, 1 );
			ruAttachGUINode( mGUIAuthorsButton, mGUIMainButtonsCanvas );

			mGUIExitButton = ruCreateGUIButton( buttonXOffset, 0, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString( "exitButton" ), pGUI->mFont, buttonColor, 1 );
			ruAttachGUINode( mGUIExitButton, mGUIMainButtonsCanvas );
			ruAddGUINodeAction( mGUIExitButton, ruGUIAction::OnClick, ruDelegate::Bind( this, &Menu::OnExitGameClick ));
		}

		mpModalWindow = new ModalWindow( g_resW / 2 - 200, g_resH / 2 - 100, 400, 200, ruGetTexture( "data/gui/tab.png" ), mButtonImage, canvasColor );
		mpModalWindow->AttachTo( mGUICanvas );

		mGUIOptionsCanvas = ruCreateGUIRect( 200, g_resH - 2.5 * mDistBetweenButtons, buttonWidth, buttonHeight * 3, ruGetTexture( "data/gui/tab.png" ), canvasColor );  
		ruAttachGUINode( mGUIOptionsCanvas, mGUICanvas );
		{
			mGUIOptionsCommonButton = ruCreateGUIButton( 0, 0, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString( "commonSettings" ), pGUI->mFont, buttonColor, 1 );
			ruAttachGUINode( mGUIOptionsCommonButton, mGUIOptionsCanvas );
			ruAddGUINodeAction( mGUIOptionsCommonButton, ruGUIAction::OnClick, ruDelegate::Bind( this, &Menu::OnOptionsCommonClick ));

			mGUIOptionsControlsButton = ruCreateGUIButton( 0, 0.5 * mDistBetweenButtons, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString( "controls" ), pGUI->mFont, buttonColor, 1 );
			ruAttachGUINode( mGUIOptionsControlsButton, mGUIOptionsCanvas );
			ruAddGUINodeAction( mGUIOptionsControlsButton, ruGUIAction::OnClick, ruDelegate::Bind( this, &Menu::OnOptionsControlsClick ));

			mGUIOptionsGraphicsButton = ruCreateGUIButton( 0, mDistBetweenButtons, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString( "graphics" ), pGUI->mFont, buttonColor, 1 );
			ruAttachGUINode( mGUIOptionsGraphicsButton, mGUIOptionsCanvas );
			ruAddGUINodeAction( mGUIOptionsGraphicsButton, ruGUIAction::OnClick, ruDelegate::Bind( this, &Menu::OnOptionsGraphicsClick ));
		}

		mGUIOptionsKeysCanvas = ruCreateGUIRect( 200, g_resH - 2.5 * mDistBetweenButtons, buttonWidth * 5, buttonHeight * 5, ruGetTexture( "data/gui/tab.png" ), canvasColor );  
		ruAttachGUINode( mGUIOptionsCanvas, mGUICanvas );
		ruSetGUINodeVisible( mGUIOptionsKeysCanvas, false );
		{
			float x = 10, y = 10;
			mpMoveForwardKey = new WaitKeyButton( x, y, mSmallButtonImage, mLocalization.GetString( "forward" ));
			mpMoveForwardKey->AttachTo( mGUIOptionsKeysCanvas );
			y += 32 * 1.1f;
			mpMoveBackwardKey = new WaitKeyButton( x, y, mSmallButtonImage, mLocalization.GetString( "backward" ) );
			mpMoveBackwardKey->AttachTo( mGUIOptionsKeysCanvas );
			y += 32 * 1.1f;
			mpStrafeLeftKey = new WaitKeyButton( x, y, mSmallButtonImage, mLocalization.GetString( "strafeLeft" ));
			mpStrafeLeftKey->AttachTo( mGUIOptionsKeysCanvas );
			y += 32 * 1.1f;
			mpStrafeRightKey = new WaitKeyButton( x, y, mSmallButtonImage, mLocalization.GetString( "strafeRight" ));
			mpStrafeRightKey->AttachTo( mGUIOptionsKeysCanvas );
			x += 150;
			y = 10;
			mpJumpKey = new WaitKeyButton( x, y, mSmallButtonImage, mLocalization.GetString( "jump" ));
			mpJumpKey->AttachTo( mGUIOptionsKeysCanvas );
			y += 32 * 1.1f;
			mpFlashLightKey = new WaitKeyButton( x, y, mSmallButtonImage, mLocalization.GetString( "flashLight" ));
			mpFlashLightKey->AttachTo( mGUIOptionsKeysCanvas );
			y += 32 * 1.1f;
			mpRunKey = new WaitKeyButton( x, y, mSmallButtonImage, mLocalization.GetString( "run" ));
			mpRunKey->AttachTo( mGUIOptionsKeysCanvas );
			y += 32 * 1.1f;
			mpInventoryKey = new WaitKeyButton( x, y, mSmallButtonImage, mLocalization.GetString( "inventory" ));
			mpInventoryKey->AttachTo( mGUIOptionsKeysCanvas );
			// Third column
			x += 150;
			y = 10;
			mpUseKey = new WaitKeyButton( x, y, mSmallButtonImage, mLocalization.GetString( "use" ));
			mpUseKey->AttachTo( mGUIOptionsKeysCanvas );
			y += 32 * 1.1f;
			mpQuickLoadKey = new WaitKeyButton( x, y, mSmallButtonImage, mLocalization.GetString( "quickLoad" ));
			mpQuickLoadKey->AttachTo( mGUIOptionsKeysCanvas );
			y += 32 * 1.1f;
			mpQuickSaveKey = new WaitKeyButton( x, y, mSmallButtonImage, mLocalization.GetString( "quickSave" ));
			mpQuickSaveKey->AttachTo( mGUIOptionsKeysCanvas );
			y += 32 * 1.1f;
			mpStealthKey = new WaitKeyButton( x, y, mSmallButtonImage, mLocalization.GetString( "stealth" ));
			mpStealthKey->AttachTo( mGUIOptionsKeysCanvas );
			// Fourth column
			x += 150;
			y = 10;
			mpLookLeftKey = new WaitKeyButton( x, y, mSmallButtonImage, mLocalization.GetString( "lookLeft" ));
			mpLookLeftKey->AttachTo( mGUIOptionsKeysCanvas );
			y += 32 * 1.1f;
			mpLookRightKey = new WaitKeyButton( x, y, mSmallButtonImage, mLocalization.GetString( "lookRight" ));
			mpLookRightKey->AttachTo( mGUIOptionsKeysCanvas );
		}

		mGUIOptionsGraphicsCanvas = ruCreateGUIRect( 200, g_resH - 3.0 * mDistBetweenButtons, buttonWidth * 3, buttonHeight * 6, ruGetTexture( "data/gui/tab.png" ), canvasColor );  
		ruAttachGUINode( mGUIOptionsGraphicsCanvas, mGUICanvas );
		ruSetGUINodeVisible( mGUIOptionsGraphicsCanvas, false );
		{
			float x = 10, y = 10;
			mpFXAAButton = new RadioButton( x, y, mButtonImage, mLocalization.GetString( "fxaa" ) );
			mpFXAAButton->AttachTo( mGUIOptionsGraphicsCanvas );

			mpFPSButton = new RadioButton( x, y + 0.5 * mDistBetweenButtons, mButtonImage, mLocalization.GetString( "showFPS" ));
			mpFPSButton->AttachTo( mGUIOptionsGraphicsCanvas );

			mpSpotShadowsButton = new RadioButton( x, y + mDistBetweenButtons, mButtonImage, mLocalization.GetString( "spotLightShadows" ));
			mpSpotShadowsButton->AttachTo( mGUIOptionsGraphicsCanvas );

			mpHDRButton = new RadioButton( x, y + 1.5 * mDistBetweenButtons, mButtonImage, mLocalization.GetString( "hdr" ));
			mpHDRButton->AttachTo( mGUIOptionsGraphicsCanvas );
			mpHDRButton->SetChangeAction( ruDelegate::Bind( this, &Menu::OnHDRButtonClick ));

			mpTextureFiltering = new ScrollList( x, y + 2.0 * mDistBetweenButtons, mButtonImage, mLocalization.GetString( "filtering" ) );
			mpTextureFiltering->AttachTo( mGUIOptionsGraphicsCanvas );
			mpTextureFiltering->AddValue( mLocalization.GetString( "trilinear" ));
			mpTextureFiltering->AddValue( mLocalization.GetString( "anisotropic" ));    
		}

		float authorsWidth = 500, authorsHeight = 400;
		mGUIAuthorsBackground = ruCreateGUIRect( (g_resW - authorsWidth) / 2, (g_resH - authorsHeight) / 2, 500, 400, ruGetTexture( "data/textures/generic/loadingScreen.jpg" ) );
		ruAttachGUINode( mGUIAuthorsBackground, mGUICanvas );
		{
			mGUIAuthorsText = ruCreateGUIText( mLocalization.GetString( "authorsText" ), 50, 50, authorsWidth - 100, authorsHeight - 100, pGUI->mFont, ruVector3( 0, 0, 0 ), 0 );
			ruAttachGUINode( mGUIAuthorsText, mGUIAuthorsBackground );
		}

		mGUISaveGameCanvas = ruCreateGUIRect( 200, g_resH - 4.0 * mDistBetweenButtons, buttonWidth + buttonXOffset, buttonHeight * 8, ruGetTexture( "data/gui/tab.png" ), canvasColor );    
		ruAttachGUINode( mGUISaveGameCanvas, mGUICanvas );
		{
			float y = 10;
			for( int i = 0; i < mSaveLoadSlotCount; i++ ) {
				mGUISaveGameSlot[i] = ruCreateGUIButton( 0, y, buttonWidth, buttonHeight, mButtonImage, "Empty slot", pGUI->mFont, buttonColor, 1 );
				ruAttachGUINode( mGUISaveGameSlot[i], mGUISaveGameCanvas );
				ruAddGUINodeAction( mGUISaveGameSlot[i], ruGUIAction::OnClick, ruDelegate::Bind( this, &Menu::OnSaveClick ));
				y += 1.1f * buttonHeight;
			}
		}

		mGUILoadGameCanvas = ruCreateGUIRect( 200, g_resH - 4.0 * mDistBetweenButtons, buttonWidth + buttonXOffset, buttonHeight * 8, ruGetTexture( "data/gui/tab.png" ), canvasColor );    
		ruAttachGUINode( mGUILoadGameCanvas, mGUICanvas );
		{
			float y = 10;
			for( int i = 0; i < mSaveLoadSlotCount; i++ ) {
				mGUILoadGameSlot[i] = ruCreateGUIButton( 0, y, buttonWidth, buttonHeight, mButtonImage, "Empty slot", pGUI->mFont, buttonColor, 1 );
				ruAttachGUINode( mGUILoadGameSlot[i], mGUILoadGameCanvas );
				ruAddGUINodeAction( mGUILoadGameSlot[i], ruGUIAction::OnClick, ruDelegate::Bind( this, &Menu::OnLoadSaveClick ));
				y += 1.1f * buttonHeight;
			}
		}

		mGUIOptionsCommonCanvas = ruCreateGUIRect( 200, g_resH - 2.5 * mDistBetweenButtons, buttonWidth * 2 + buttonXOffset, buttonHeight * 4, ruGetTexture( "data/gui/tab.png" ), canvasColor );    
		ruAttachGUINode( mGUIOptionsCommonCanvas, mGUICanvas );
		ruSetGUINodeVisible( mGUIOptionsCommonCanvas, false );
		{
			float x = 0;
			float y = 0;
			mpMasterVolume = new Slider( x, y, 0, 100, 2.5f, ruGetTexture( "data/gui/smallbuttonthick.png" ), mLocalization.GetString( "masterVolume" ) );
			mpMasterVolume->AttachTo( mGUIOptionsCommonCanvas );
			mpMasterVolume->SetChangeAction( ruDelegate::Bind( this, &Menu::OnSoundVolumeChange ));

			mpMusicVolume = new Slider( x, y + 0.5f * mDistBetweenButtons, 0, 100, 2.5f, ruGetTexture( "data/gui/smallbuttonthick.png" ), mLocalization.GetString(  "musicVolume" ) );
			mpMusicVolume->AttachTo( mGUIOptionsCommonCanvas );
			mpMusicVolume->SetChangeAction( ruDelegate::Bind( this, &Menu::OnMusicVolumeChange ));

			mpMouseSensivity = new Slider( x, y + 1.0f * mDistBetweenButtons, 0, 100, 2.5f, ruGetTexture( "data/gui/smallbuttonthick.png" ), mLocalization.GetString( "mouseSens" ) );
			mpMouseSensivity->AttachTo( mGUIOptionsCommonCanvas );
			mpMouseSensivity->SetChangeAction( ruDelegate::Bind( this, &Menu::OnMouseSensivityChange ));
		}
	}


	
    ruEngine::SetHDREnabled( mpHDRButton->IsEnabled() );
    SetOptionsPageVisible( false );
    SetAuthorsPageVisible( false );
    SetPage( Page::Main );
	mMainButtonsAlpha = 255.0f;
    LoadConfig();
}

void Menu::Show() {
    mpCamera->MakeCurrent();
    mpCamera->FadeIn();
    mScene.Show();
    if( pCurrentLevel ) {
        pCurrentLevel->Hide();
    }
    if( pPlayer ) {
		pPlayer->SetBodyVisible( false );
        pPlayer->SetHUDVisible( false );
    }
    mVisible = true;
    ruEngine::ShowCursor();
    ruSetGUINodeVisible( mGUICanvas, true );
	mMainButtonsAlpha = 255.0f;
}

void Menu::Hide( ) {
    if( pPlayer ) {
        pPlayer->mpCamera->MakeCurrent();
    }
    if( pPlayer ) {
		pPlayer->SetBodyVisible( true );
        pPlayer->SetHUDVisible( true );
    }
    
    mScene.Hide();
    if( pCurrentLevel ) {
        pCurrentLevel->Show();
    }
    mVisible = false;
    mPage = Page::Main;
    ruEngine::HideCursor();
    mMusic.Pause();
    ruSetGUINodeVisible( mGUICanvas, false );
}

void Menu::OnStartNewGameClick() {
	mpModalWindow->Ask( mLocalization.GetString( "newGameQuestion" ) );
	mpModalWindow->SetYesAction( ruDelegate::Bind( this, &Menu::StartNewGame ));
	SetPage( Page::Main );
}

void Menu::OnExitGameClick() {
	mpModalWindow->Ask( mLocalization.GetString( "endGameQuestion" ) );	
	mpModalWindow->SetYesAction( ruDelegate::Bind( this, &Menu::StartExitGame ));
	SetPage( Page::Main );
}

void Menu::StartNewGame() {
	CameraStartFadeOut( ruDelegate::Bind( this, &Menu::DoStartNewGame ));
}

void Menu::StartExitGame() {
	CameraStartFadeOut( ruDelegate::Bind( this, &Menu::DoExitGame ));
}

void Menu::OnContinueGameClick() {
	SetPage( Page::Main );
	if( !pCurrentLevel ) {
		mpModalWindow->Ask( mLocalization.GetString( "continueLastGameQuestion" ) );	
		mpModalWindow->SetYesAction( ruDelegate::Bind( this, &Menu::StartContinueGameFromLast ));
	} else {
		CameraStartFadeOut( ruDelegate::Bind( this, &Menu::DoContinueGameCurrent ));
	}	
}

void Menu::DoContinueGameCurrent() {
	Hide();
}

void Menu::DoStartNewGame() {
	Level::Change( g_initialLevel );						
	Hide();
}

void Menu::DoExitGame() {
	if( pPlayer && pCurrentLevel ) {
		if( !pPlayer->mDead ) {
			SaveWriter( "lastGame.save" ).SaveWorldState();
		}
	}
	g_running = false;
	WriteConfig();
}

void Menu::OnOptionsClick() {
	SetPage( Page::Options );
}

void Menu::OnOptionsGraphicsClick() {
	SetPage( Page::OptionsGraphics );
}

void Menu::OnOptionsCommonClick() {
	SetPage( Page::OptionsCommon );
}

void Menu::OnOptionsControlsClick() {
	SetPage( Page::OptionsKeys );
}

void Menu::StartContinueGameFromLast() {
	CameraStartFadeOut( ruDelegate::Bind( this, &Menu::DoContinueGameFromLast ));
}

void Menu::DoContinueGameFromLast() {
	SaveLoader( "lastGame.save" ).RestoreWorldState();
	Hide();
}

void Menu::UpdateCamera() {
	mpCamera->Update();
	if( !mCameraFadeActionDone ) {
		mMainButtonsAlpha -= 10;
		if( mMainButtonsAlpha < 0 ) {
			mMainButtonsAlpha = 0;
		}
	}
	if( mpCamera->FadeComplete() ) {
		if( !mCameraFadeActionDone ) {
			mCameraFadeDoneAction.Call();
			mCameraFadeActionDone = true;
		}
	}
}

void Menu::StartLoadFromSave() {
	CameraStartFadeOut( ruDelegate::Bind( this, &Menu::DoLoadFromSave ));
}

void Menu::OnLoadSaveClick() {	
	mpModalWindow->Ask( mLocalization.GetString( "loadSaveQuestion" ) );	
	mpModalWindow->SetYesAction( ruDelegate::Bind( this, &Menu::StartLoadFromSave ));
}

void Menu::DoLoadFromSave() {
	SaveLoader( mLoadSaveGameName ).RestoreWorldState(); 
	Hide();
}

void Menu::OnSaveClick() {
	mpModalWindow->Ask( mLocalization.GetString( "rewriteSaveQuestion" ) );	
	mpModalWindow->SetYesAction( ruDelegate::Bind( this, &Menu::DoSaveCurrentGame ));
}

void Menu::OnMouseSensivityChange() {
	mouseSens = mpMouseSensivity->GetValue() / 100.0f;
}

void Menu::OnMusicVolumeChange() {
	mMusic.SetVolume( mpMusicVolume->GetValue() / 100.0f );
	g_musicVolume = mpMusicVolume->GetValue() / 100.0f;
	if( pCurrentLevel ) {
		pCurrentLevel->mMusic.SetVolume( g_musicVolume );
	}
}
void Menu::OnSoundVolumeChange() {
	ruSetMasterVolume( mpMasterVolume->GetValue() / 100.0f );
}

void Menu::DoSaveCurrentGame() {
	SaveWriter( mSaveGameSlotName ).SaveWorldState();
	SetPage( Page::Main );
}

void Menu::CameraStartFadeOut( const ruDelegate & onFadeDoneAction ) {
	mpCamera->FadeOut();
	mCameraFadeActionDone = false;
	mCameraFadeDoneAction = onFadeDoneAction;
}

void Menu::Update( ) {
    ruEngine::SetAmbientColor( ruVector3( 25 / 255.0f, 25 / 255.0f, 25  / 255.0f));

	WaitKeyButton::UpdateAll();
    UpdateCamera();

	SyncPlayerControls();

    if( mVisible ) {

        if( ruIsKeyHit( KEY_Esc ) ) {
			if( pCurrentLevel ) {
				CameraStartFadeOut( ruDelegate::Bind( this, &Menu::DoContinueGameCurrent ));
			}
        }

        CameraFloating();

        int mainButtonsX = 20;
        int saveGamePosX = ( mPage == Page::SaveGame ) ? 20 : 0;
        int loadGamePosX = ( mPage == Page::LoadGame ) ? 20 : 0;
        int optionsPosX = ( mPage == Page::Options || mPage == Page::OptionsGraphics || mPage == Page::OptionsKeys || mPage == Page::OptionsCommon ) ? 20 : 0;

		if( pPlayer ) {
			if( pPlayer->IsDead() ) {
				ruSetGUIButtonActive( mGUIContinueGameButton, false );
				ruSetGUIButtonActive( mGUISaveGameButton, false );
			} else {
				ruSetGUIButtonActive( mGUIContinueGameButton, true );
				ruSetGUIButtonActive( mGUISaveGameButton, true );
			}
		} else {
			ruSetGUIButtonActive( mGUISaveGameButton, false );
		}       

        ruSetGUINodePosition( mGUIContinueGameButton, 0, 0 );
        ruSetGUINodePosition( mGUIStartButton, 0, 0.5f * mDistBetweenButtons );
        ruSetGUINodePosition( mGUISaveGameButton, saveGamePosX, mDistBetweenButtons);
        ruSetGUINodePosition( mGUILoadGameButton, loadGamePosX, 1.5 * mDistBetweenButtons );
        ruSetGUINodePosition( mGUIOptionsButton, optionsPosX, 2.0 * mDistBetweenButtons );
        ruSetGUINodePosition( mGUIAuthorsButton, 0, 2.5 * mDistBetweenButtons );
        ruSetGUINodePosition( mGUIExitButton, 0, 3.0 * mDistBetweenButtons );

		ruSetGUINodeAlpha( mGUIMainButtonsCanvas, mMainButtonsAlpha );

        if( mPage == Page::Authors ) {
			mpModalWindow->CloseNoAction();
            SetAuthorsPageVisible( true );
        } else {
            SetAuthorsPageVisible( false );
        }

        if( mPage == Page::Options ) {
            SetOptionsPageVisible( true );
        } else {
            SetOptionsPageVisible( false );
        }

        if( mPage == Page::OptionsCommon ) {
            ruSetGUINodeVisible( mGUIOptionsCommonCanvas, true );			
        } else {
            ruSetGUINodeVisible( mGUIOptionsCommonCanvas, false );
        }


        if( mPage == Page::LoadGame ) {
            ruSetGUINodeVisible( mGUILoadGameCanvas, true );
            vector< string > nameList;
            GetFilesWithExtension( "*.save", nameList );
            int count = nameList.size();
            if( count >= mSaveLoadSlotCount ) {
                count = mSaveLoadSlotCount;
            }
			// inactivate all buttons
			for( int i = 0; i < mSaveLoadSlotCount; i++ ) {
				ruSetGUIButtonActive( mGUILoadGameSlot[i], false );
			}
            for( int i = 0; i < count; i++ ) {
				// activate button associated with file
				ruSetGUIButtonActive( mGUILoadGameSlot[i], true );
                ruSetGUINodeText( ruGetButtonText( mGUILoadGameSlot[i] ), nameList[i] );
                if( ruIsButtonHit( mGUILoadGameSlot[i] ) ) {
                    mLoadSaveGameName = nameList[i];
                    SetPage( Page::Main );
                }
            }
        } else {
            ruSetGUINodeVisible( mGUILoadGameCanvas, false );
        }

        if( mPage == Page::SaveGame ) {
            ruSetGUINodeVisible( mGUISaveGameCanvas, true );

            vector< string > nameList;
            GetFilesWithExtension( "*.save", nameList );
            for( int iName = nameList.size() - 1; iName < 6; iName++ ) {
                string saveName = "Slot";
                saveName += ( (char)iName + (char)'0' );
                saveName += ".save";
                nameList.push_back( saveName );
            }
            int count = nameList.size();
            if( count >= mSaveLoadSlotCount ) {
                count = mSaveLoadSlotCount;
            }
            for( int iName = 0; iName < count; iName++ ) {
                ruSetGUINodeText( ruGetButtonText( mGUISaveGameSlot[iName] ), nameList[iName] );
                if( ruIsButtonHit( mGUISaveGameSlot[iName] ) ) {
                    mSaveGameSlotName = nameList[iName];
                }
            }
        } else {
            ruSetGUINodeVisible( mGUISaveGameCanvas, false );
        }

        if( mPage == Page::OptionsGraphics ) {
            ruSetGUINodeVisible( mGUIOptionsGraphicsCanvas, true );

            // FXAA Options
            mpFXAAButton->Update();
            if( mpFXAAButton->IsEnabled() ) {
                ruEngine::EnableFXAA();
            } else {
                ruEngine::DisableFXAA();
            }

            // show fps
            mpFPSButton->Update();
            g_showFPS = mpFPSButton->IsEnabled();

            // texture filtering
            mpTextureFiltering->Update( );
            if( mpTextureFiltering->GetCurrentValue() == 0 ) {
                ruEngine::SetAnisotropicTextureFiltration( false );
            } else {
                ruEngine::SetAnisotropicTextureFiltration( true );
            }

            mpSpotShadowsButton->Update();
            ruEngine::EnableSpotLightShadows( mpSpotShadowsButton->IsEnabled() );

            mpHDRButton->Update();
            
        } else {
            ruSetGUINodeVisible( mGUIOptionsGraphicsCanvas, false );
        }

        if( mPage == Page::OptionsKeys ) {
            ruSetGUINodeVisible( mGUIOptionsKeysCanvas, true );
        } else {
            ruSetGUINodeVisible( mGUIOptionsKeysCanvas, false );
        }

		mMusic.Play();

		if( ruIsButtonHit( mGUILoadGameButton ) ) {
			SetPage( Page::LoadGame );
		}
		if( ruIsButtonHit( mGUISaveGameButton ) ) {
			SetPage( Page::SaveGame );
		}
		if( ruIsButtonHit( mGUIAuthorsButton ) ) {
			SetPage( Page::Authors );
		}
        
    } else {
        if( ruIsKeyHit( KEY_Esc ) ) {
            pMainMenu->Show();
        }
    }
}

void Menu::SetPage( Page page ) {
    if( mPage == page ) {
        mPage = Page::Main;
    } else {
        mPage = page;
    }
}

void Menu::LoadTextures() {
    mButtonImage = ruGetTexture( "data/gui/menubutton.png" );
    mSmallButtonImage = ruGetTexture( "data/gui/smallbutton.png" );
}

void Menu::CreateCamera() {
    mpCamera = new GameCamera;
	mCameraFadeActionDone = false;
    mCameraInitialPosition = ruFindByName( "Camera").GetPosition();
    mCameraAnimationNewOffset = ruVector3( 0.5, 0.5, 0.5 );
}

void Menu::LoadSounds() {
    mPickSound = ruSound::Load2D( "data/sounds/menupick.ogg" );
    mMusic = ruSound::LoadMusic( "data/music/menu.ogg" );
}

void Menu::LoadConfig() {
    // Load config
    Parser config;
    config.ParseFile( "config.cfg" );

    if( !config.Empty() ) {
        mpMasterVolume->SetValue( config.GetNumber( "masterVolume" ));

        mpMusicVolume->SetValue( config.GetNumber( "musicVolume" ));
        g_musicVolume = mpMusicVolume->GetValue();

        mpMouseSensivity->SetValue( config.GetNumber( "mouseSens" ));
        mpFXAAButton->SetEnabled( config.GetNumber( "fxaaEnabled" ) != 0 );
        if( mpFXAAButton->IsEnabled() ) {
            ruEngine::EnableFXAA();
        } else {
            ruEngine::DisableFXAA();
        };
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
        ruEngine::EnableSpotLightShadows( mpSpotShadowsButton->IsEnabled() );

        ruSetMasterVolume( mpMasterVolume->GetValue() / 100.0f );
        mMusic.SetVolume( mpMusicVolume->GetValue() / 100.0f );

        mpHDRButton->SetEnabled( config.GetNumber( "hdrEnabled" ) != 0 );
        ruEngine::SetHDREnabled( mpHDRButton->IsEnabled() );

        mpTextureFiltering->SetCurrentValue( config.GetNumber( "textureFiltering" ));
        ruEngine::SetAnisotropicTextureFiltration( mpTextureFiltering->GetCurrentValue() );

		mpLookLeftKey->SetSelected( config.GetNumber( "keyLookLeft" ) );
		mpLookRightKey->SetSelected( config.GetNumber( "keyLookRight" ) );

		mpFPSButton->SetEnabled( config.GetNumber( "showFPS" ) != 0.0f );
		g_showFPS = mpFPSButton->IsEnabled();
    }
}

void Menu::SyncPlayerControls() {
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
		pPlayer->mKeyLookLeft = mpLookLeftKey->GetSelectedKey();
		pPlayer->mKeyLookRight = mpLookRightKey->GetSelectedKey();
    }
}

void Menu::WriteConfig() {
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
    WriteInteger( config, "spotShadowsEnabled", ruEngine::IsSpotLightShadowsEnabled() ? 1 : 0 );
    WriteInteger( config, "hdrEnabled", ruEngine::IsHDREnabled() ? 1 : 0  );
    WriteInteger( config, "keyStealth", mpStealthKey->GetSelectedKey() );
    WriteInteger( config, "textureFiltering", mpTextureFiltering->GetCurrentValue() );
	WriteInteger( config, "keyLookLeft", mpLookLeftKey->GetSelectedKey() );
	WriteInteger( config, "keyLookRight", mpLookRightKey->GetSelectedKey() );
	WriteInteger( config, "showFPS", mpFPSButton->IsEnabled() ? 1 : 0 );
    config.close();
}

void Menu::WriteString( ofstream & stream, string name, string value ) {
    stream << name << "=\"" << value << "\";\n";
}

void Menu::WriteInteger( ofstream & stream, string name, int value ) {
    stream << name << "=\"" << value << "\";\n";
}

void Menu::WriteFloat( ofstream & stream, string name, float value ) {
    stream << name << "=\"" << value << "\";\n";
}


Menu::~Menu() {
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
    delete mpHDRButton;
    delete mpSpotShadowsButton;
    delete mpStealthKey;
    delete mpFPSButton;
	delete mpLookLeftKey;
	delete mpLookRightKey;
}

void Menu::SetMainPageVisible( bool state ) {
	ruSetGUINodeVisible( mGUIMainButtonsCanvas, state );
}

void Menu::SetAuthorsPageVisible( bool state ) {
    ruSetGUINodeVisible( mGUIAuthorsBackground, state );
}

void Menu::SetOptionsPageVisible( bool state ) {
    ruSetGUINodeVisible( mGUIOptionsCanvas, state );
}

Parser * Menu::GetLocalization() {
    return &mLocalization;
}

bool Menu::IsVisible() {
    return mVisible;
}

void Menu::CameraFloating() {
    mCameraAnimationOffset = mCameraAnimationOffset.Lerp( mCameraAnimationNewOffset, 0.0085f );

    if( ( mCameraAnimationOffset - mCameraAnimationNewOffset ).Length2() < 0.025 ) {
        mCameraAnimationNewOffset = ruVector3( frandom( -1.5, 1.5 ), frandom( -1.5, 1.5 ), frandom( -1.5, 1.5 ) );
    }

    mpCamera->mNode.SetPosition( mCameraInitialPosition + mCameraAnimationOffset );
}

void Menu::OnHDRButtonClick()
{
	ruEngine::SetHDREnabled( mpHDRButton->IsEnabled() );
}
