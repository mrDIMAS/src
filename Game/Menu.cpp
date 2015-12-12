#include "Precompiled.h"

#include "Menu.h"
#include "Level.h"
#include "GUIProperties.h"
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

    mScene = ruSceneNode::LoadFromFile( "data/maps/menu/menu.scene" );

    mDistBetweenButtons = 72;
    mVisible = true;
    mPage = Page::Main;
    mLoadSaveGameName = "";

    CreateCamera();
    LoadSounds();
    LoadTextures();
		
    const float buttonHeight = 32;
    const float buttonWidth = 128;
	const float buttonXOffset = 10;

	ruTextureHandle tabTexture = ruGetTexture( "data/gui/menu/tab.tga" );

	// Setup
	mGUICanvas = ruCreateGUIRect( 0, 0, 0, 0, ruTextureHandle::Empty() );
	ruSetGUINodeChildAlphaControl( mGUICanvas, true );
	{
		mGUIMainButtonsCanvas = ruCreateGUIRect( 20, g_resH - 4.0 * mDistBetweenButtons, buttonWidth + 2 * buttonXOffset, buttonHeight * 8, tabTexture, pGUIProp->mBackColor );    
		ruSetGUINodeChildAlphaControl( mGUIMainButtonsCanvas, true );
		ruAttachGUINode( mGUIMainButtonsCanvas, mGUICanvas );
		{
			mGUIContinueGameButton = ruCreateGUIButton( buttonXOffset, 5, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString( "continueButton" ), pGUIProp->mFont, pGUIProp->mForeColor, 1 );
			ruAttachGUINode( mGUIContinueGameButton, mGUIMainButtonsCanvas );
			ruAddGUINodeAction( mGUIContinueGameButton, ruGUIAction::OnClick, ruDelegate::Bind( this, &Menu::OnContinueGameClick ));

			mGUIStartButton = ruCreateGUIButton( buttonXOffset, 5 + 0.5f * mDistBetweenButtons, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString( "startButton" ), pGUIProp->mFont, pGUIProp->mForeColor, 1 );
			ruAttachGUINode( mGUIStartButton, mGUIMainButtonsCanvas );
			ruAddGUINodeAction( mGUIStartButton, ruGUIAction::OnClick, ruDelegate::Bind( this, &Menu::OnStartNewGameClick ));

			mGUISaveGameButton = ruCreateGUIButton( buttonXOffset, 5 + 1.0f * mDistBetweenButtons, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString( "saveButton" ), pGUIProp->mFont, pGUIProp->mForeColor, 1 );
			ruAttachGUINode( mGUISaveGameButton, mGUIMainButtonsCanvas );

			mGUILoadGameButton = ruCreateGUIButton( buttonXOffset, 5 + 1.5f * mDistBetweenButtons, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString( "loadButton" ), pGUIProp->mFont, pGUIProp->mForeColor, 1 );
			ruAttachGUINode( mGUILoadGameButton, mGUIMainButtonsCanvas );

			mGUIOptionsButton = ruCreateGUIButton( buttonXOffset, 5 + 2.0f * mDistBetweenButtons, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString( "optionsButton" ), pGUIProp->mFont, pGUIProp->mForeColor, 1 );
			ruAttachGUINode( mGUIOptionsButton, mGUIMainButtonsCanvas );
			ruAddGUINodeAction( mGUIOptionsButton, ruGUIAction::OnClick, ruDelegate::Bind( this, &Menu::OnOptionsClick ));

			mGUIAuthorsButton = ruCreateGUIButton( buttonXOffset, 5 + 2.5f * mDistBetweenButtons, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString( "authorsButton" ), pGUIProp->mFont, pGUIProp->mForeColor, 1 );
			ruAttachGUINode( mGUIAuthorsButton, mGUIMainButtonsCanvas );

			mGUIExitButton = ruCreateGUIButton( buttonXOffset, 5 + 3.0f * mDistBetweenButtons, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString( "exitButton" ), pGUIProp->mFont, pGUIProp->mForeColor, 1 );
			ruAttachGUINode( mGUIExitButton, mGUIMainButtonsCanvas );
			ruAddGUINodeAction( mGUIExitButton, ruGUIAction::OnClick, ruDelegate::Bind( this, &Menu::OnExitGameClick ));
		}

		int aTabX = 200;
		int aTabY = g_resH - 4.0 * mDistBetweenButtons;
		int aTabWidth = buttonWidth * 3;
		int aTabHeight = buttonHeight * 8;

		mpModalWindow = new ModalWindow( aTabX, aTabY, aTabWidth, aTabHeight, ruGetTexture( "data/gui/menu/tab.tga" ), mButtonImage, pGUIProp->mBackColor );
		mpModalWindow->AttachTo( mGUICanvas );

		mGUIWindowText = ruCreateGUIText( " ", aTabX, aTabY - 17, aTabWidth, 32, pGUIProp->mFont, ruVector3( 255, 255, 255 ), 0 );
		ruAttachGUINode( mGUIWindowText, mGUICanvas );

		mGUICaption = ruCreateGUIText( "THE MINE", 20, aTabY - 17, aTabWidth, 32, pGUIProp->mFont, ruVector3( 255, 255, 255 ), 0 );
		ruAttachGUINode( mGUICaption, mGUICanvas );

		mGUIOptionsCanvas = ruCreateGUIRect( aTabX, aTabY, aTabWidth, aTabHeight, tabTexture, pGUIProp->mBackColor );  
		ruAttachGUINode( mGUIOptionsCanvas, mGUICanvas );
		{
			int yOffset = (aTabHeight - 2 * mDistBetweenButtons ) / 2;

			mGUIOptionsCommonButton = ruCreateGUIButton(  ( aTabWidth - buttonWidth ) / 2, yOffset, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString( "commonSettings" ), pGUIProp->mFont, pGUIProp->mForeColor, 1 );
			ruAttachGUINode( mGUIOptionsCommonButton, mGUIOptionsCanvas );
			ruAddGUINodeAction( mGUIOptionsCommonButton, ruGUIAction::OnClick, ruDelegate::Bind( this, &Menu::OnOptionsCommonClick ));

			mGUIOptionsControlsButton = ruCreateGUIButton(  ( aTabWidth - buttonWidth ) / 2, yOffset + 0.5 * mDistBetweenButtons, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString( "controls" ), pGUIProp->mFont, pGUIProp->mForeColor, 1 );
			ruAttachGUINode( mGUIOptionsControlsButton, mGUIOptionsCanvas );
			ruAddGUINodeAction( mGUIOptionsControlsButton, ruGUIAction::OnClick, ruDelegate::Bind( this, &Menu::OnOptionsControlsClick ));

			mGUIOptionsGraphicsButton = ruCreateGUIButton(  ( aTabWidth - buttonWidth ) / 2, yOffset + mDistBetweenButtons, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString( "graphics" ), pGUIProp->mFont, pGUIProp->mForeColor, 1 );
			ruAttachGUINode( mGUIOptionsGraphicsButton, mGUIOptionsCanvas );
			ruAddGUINodeAction( mGUIOptionsGraphicsButton, ruGUIAction::OnClick, ruDelegate::Bind( this, &Menu::OnOptionsGraphicsClick ));
		}

		mGUIOptionsKeysCanvas = ruCreateGUIRect( aTabX, aTabY, aTabWidth, aTabHeight, tabTexture, pGUIProp->mBackColor );    
		ruAttachGUINode( mGUIOptionsCanvas, mGUICanvas );
		ruSetGUINodeVisible( mGUIOptionsKeysCanvas, false );
		{
			float x = 40, y = 10;
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
			y += 32 * 1.1f;
			mpJumpKey = new WaitKeyButton( x, y, mSmallButtonImage, mLocalization.GetString( "jump" ));
			mpJumpKey->AttachTo( mGUIOptionsKeysCanvas );
			y += 32 * 1.1f;
			mpFlashLightKey = new WaitKeyButton( x, y, mSmallButtonImage, mLocalization.GetString( "flashLight" ));
			mpFlashLightKey->AttachTo( mGUIOptionsKeysCanvas );
			y += 32 * 1.1f;
			mpRunKey = new WaitKeyButton( x, y, mSmallButtonImage, mLocalization.GetString( "run" ));
			mpRunKey->AttachTo( mGUIOptionsKeysCanvas );
			// Second column
			x += 150;
			y = 10;
			mpInventoryKey = new WaitKeyButton( x, y, mSmallButtonImage, mLocalization.GetString( "inventory" ));
			mpInventoryKey->AttachTo( mGUIOptionsKeysCanvas );
			y += 32 * 1.1f;
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
			y += 32 * 1.1f;
			mpLookLeftKey = new WaitKeyButton( x, y, mSmallButtonImage, mLocalization.GetString( "lookLeft" ));
			mpLookLeftKey->AttachTo( mGUIOptionsKeysCanvas );
			y += 32 * 1.1f;
			mpLookRightKey = new WaitKeyButton( x, y, mSmallButtonImage, mLocalization.GetString( "lookRight" ));
			mpLookRightKey->AttachTo( mGUIOptionsKeysCanvas );
		}

		mGUIOptionsGraphicsCanvas = ruCreateGUIRect( aTabX, aTabY, aTabWidth, aTabHeight, tabTexture, pGUIProp->mBackColor );    
		ruAttachGUINode( mGUIOptionsGraphicsCanvas, mGUICanvas );
		ruSetGUINodeVisible( mGUIOptionsGraphicsCanvas, false );
		{
			float x = 30, y = 10;
			mpFXAAButton = new RadioButton( x, y, mButtonImage, mLocalization.GetString( "fxaa" ) );
			mpFXAAButton->AttachTo( mGUIOptionsGraphicsCanvas );

			mpFPSButton = new RadioButton( x, y + 0.5 * mDistBetweenButtons, mButtonImage, mLocalization.GetString( "showFPS" ));
			mpFPSButton->AttachTo( mGUIOptionsGraphicsCanvas );

			mpSpotShadowsButton = new RadioButton( x, y + mDistBetweenButtons, mButtonImage, mLocalization.GetString( "spotLightShadows" ));
			mpSpotShadowsButton->AttachTo( mGUIOptionsGraphicsCanvas );

			mpHDRButton = new RadioButton( x, y + 1.5 * mDistBetweenButtons, mButtonImage, mLocalization.GetString( "hdr" ));
			mpHDRButton->AttachTo( mGUIOptionsGraphicsCanvas );
			mpHDRButton->SetChangeAction( ruDelegate::Bind( this, &Menu::OnHDRButtonClick ));

			mpParallaxButton = new RadioButton( x, y + 2.0 * mDistBetweenButtons, mButtonImage, mLocalization.GetString( "parallax" ));
			mpParallaxButton->AttachTo( mGUIOptionsGraphicsCanvas );
			mpParallaxButton->SetChangeAction( ruDelegate::Bind( this, &Menu::OnParallaxButtonClick ));

			mpTextureFiltering = new ScrollList( x, y + 2.5 * mDistBetweenButtons, mButtonImage, mLocalization.GetString( "filtering" ) );
			mpTextureFiltering->AttachTo( mGUIOptionsGraphicsCanvas );
			mpTextureFiltering->AddValue( mLocalization.GetString( "trilinear" ));
			mpTextureFiltering->AddValue( mLocalization.GetString( "anisotropic" ));   
		}

		mGUIAuthorsBackground = ruCreateGUIRect( aTabX, aTabY, aTabWidth, aTabHeight, tabTexture, pGUIProp->mBackColor );  
		ruAttachGUINode( mGUIAuthorsBackground, mGUICanvas );
		{
			mGUIAuthorsText = ruCreateGUIText( mLocalization.GetString( "authorsText" ), 15, 15, aTabWidth - 30, aTabHeight - 30, pGUIProp->mFont, ruVector3( 255, 255, 255 ), 0 );
			ruAttachGUINode( mGUIAuthorsText, mGUIAuthorsBackground );
		}

		mGUISaveGameCanvas = ruCreateGUIRect( aTabX, aTabY, aTabWidth, aTabHeight, tabTexture, pGUIProp->mBackColor );      
		ruAttachGUINode( mGUISaveGameCanvas, mGUICanvas );
		{
			float y = 10;
			for( int i = 0; i < mSaveLoadSlotCount; i++ ) {
				mGUISaveGameSlot[i] = ruCreateGUIButton( ( aTabWidth - buttonWidth ) / 2, y, buttonWidth, buttonHeight, mButtonImage, "Empty slot", pGUIProp->mFont, pGUIProp->mForeColor, 1 );
				ruAttachGUINode( mGUISaveGameSlot[i], mGUISaveGameCanvas );
				ruAddGUINodeAction( mGUISaveGameSlot[i], ruGUIAction::OnClick, ruDelegate::Bind( this, &Menu::OnSaveClick ));
				y += 1.1f * buttonHeight;
			}
		}

		mGUILoadGameCanvas = ruCreateGUIRect( aTabX, aTabY, aTabWidth, aTabHeight, tabTexture, pGUIProp->mBackColor );  
		ruAttachGUINode( mGUILoadGameCanvas, mGUICanvas );
		{
			float y = 10;
			for( int i = 0; i < mSaveLoadSlotCount; i++ ) {
				mGUILoadGameSlot[i] = ruCreateGUIButton( ( aTabWidth - buttonWidth ) / 2, y, buttonWidth, buttonHeight, mButtonImage, "Empty slot", pGUIProp->mFont, pGUIProp->mForeColor, 1 );
				ruAttachGUINode( mGUILoadGameSlot[i], mGUILoadGameCanvas );
				ruAddGUINodeAction( mGUILoadGameSlot[i], ruGUIAction::OnClick, ruDelegate::Bind( this, &Menu::OnLoadSaveClick ));
				y += 1.1f * buttonHeight;
			}
		}

		mGUIOptionsCommonCanvas = ruCreateGUIRect( aTabX, aTabY, aTabWidth, aTabHeight, tabTexture, pGUIProp->mBackColor );  
		ruAttachGUINode( mGUIOptionsCommonCanvas, mGUICanvas );
		ruSetGUINodeVisible( mGUIOptionsCommonCanvas, false );
		{
			int yOffset = (aTabHeight - 1.5 * mDistBetweenButtons ) / 2;
			int xOffset = aTabWidth / 6.5;

			mpMasterVolume = new Slider( xOffset, yOffset, 0, 100, 2.5f, ruGetTexture( "data/gui/menu/smallbutton.tga" ), mLocalization.GetString( "masterVolume" ) );
			mpMasterVolume->AttachTo( mGUIOptionsCommonCanvas );
			mpMasterVolume->SetChangeAction( ruDelegate::Bind( this, &Menu::OnSoundVolumeChange ));

			mpMusicVolume = new Slider( xOffset, yOffset + 0.5f * mDistBetweenButtons, 0, 100, 2.5f, ruGetTexture( "data/gui/menu/smallbutton.tga"), mLocalization.GetString(  "musicVolume" ) );
			mpMusicVolume->AttachTo( mGUIOptionsCommonCanvas );
			mpMusicVolume->SetChangeAction( ruDelegate::Bind( this, &Menu::OnMusicVolumeChange ));

			mpMouseSensivity = new Slider( xOffset, yOffset + 1.0f * mDistBetweenButtons, 0, 100, 2.5f, ruGetTexture( "data/gui/menu/smallbutton.tga" ), mLocalization.GetString( "mouseSens" ) );
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

float Menu::GetMusicVolume() {
	return mpMusicVolume->GetValue() / 100.0f;
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
	ruSetGUINodeVisible( mGUICaption, true );
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
	ruSetGUINodeVisible( mGUICaption, false );
}

void Menu::OnStartNewGameClick() {
	SetPage( Page::Main );
	mpModalWindow->Ask( mLocalization.GetString( "newGameQuestion" ) );
	mpModalWindow->SetYesAction( ruDelegate::Bind( this, &Menu::StartNewGame ));
	
}

void Menu::OnExitGameClick() {
	SetPage( Page::Main );
	mpModalWindow->Ask( mLocalization.GetString( "endGameQuestion" ) );	
	mpModalWindow->SetYesAction( ruDelegate::Bind( this, &Menu::StartExitGame ));	
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
	SaveLoader( "quickSave.save" ).RestoreWorldState();
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
    ruEngine::SetAmbientColor( ruVector3( 14 / 255.0f, 14 / 255.0f, 14 / 255.0f));

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
            ruEngine::SetFXAAEnabled( mpFXAAButton->IsEnabled() );

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

			mpParallaxButton->Update();
            
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
	mpModalWindow->Close();
    if( mPage == page ) {
        mPage = Page::Main;
    } else {
        mPage = page;
    }
	ruSetGUINodeText( mGUIWindowText, " " );
	if( mPage == Page::Options ) {
		ruSetGUINodeText( mGUIWindowText, mLocalization.GetString( "captionOptions" ));
	} 
	if( mPage == Page::OptionsGraphics ) {
		ruSetGUINodeText( mGUIWindowText, mLocalization.GetString( "captionOptionsGraphics" ));
	}
	if( mPage == Page::OptionsKeys ) {
		ruSetGUINodeText( mGUIWindowText, mLocalization.GetString( "captionOptionsControls" ));
	}
	if( mPage == Page::OptionsCommon ) {
		ruSetGUINodeText( mGUIWindowText, mLocalization.GetString( "captionOptionsCommon" ));
	}
	if( mPage == Page::SaveGame ) {
		ruSetGUINodeText( mGUIWindowText, mLocalization.GetString( "captionSaveGame" ));
	}
	if( mPage == Page::LoadGame ) {
		ruSetGUINodeText( mGUIWindowText, mLocalization.GetString( "captionLoadGame" ));
	}
	if( mPage == Page::Authors ) {
		ruSetGUINodeText( mGUIWindowText, mLocalization.GetString( "captionAuthors" ));
	}
}

void Menu::LoadTextures() {
    mButtonImage = ruGetTexture( "data/gui/menu/button.tga" );
    mSmallButtonImage = ruGetTexture( "data/gui/menu/button.tga" );
}

void Menu::CreateCamera() {
    mpCamera = new GameCamera;
	mCameraFadeActionDone = false;
    mCameraInitialPosition = mScene.FindChild( "Camera" ).GetPosition();
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
        ruEngine::SetFXAAEnabled( mpFXAAButton->IsEnabled() );
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

		mpParallaxButton->SetEnabled( config.GetNumber( "parallax" ) != 0 );
		ruEngine::SetParallaxEnabled( mpParallaxButton->IsEnabled() );

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
	WriteInteger( config, "parallax", mpParallaxButton->IsEnabled() ? 1 : 0 );
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
	delete mpParallaxButton;
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
