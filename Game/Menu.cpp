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

	shared_ptr<ruTexture> tabTexture = ruTexture::Request( "data/gui/menu/tab.tga" );

	// Setup
	mGUICanvas = ruRect::Create( 0, 0, 0, 0, nullptr );
	mGUICanvas->SetChildAlphaControl( true );
	{
		mGUIMainButtonsCanvas = ruRect::Create( 20, g_resH - 4.0 * mDistBetweenButtons, buttonWidth + 2 * buttonXOffset, buttonHeight * 8, tabTexture, pGUIProp->mBackColor );    
		mGUIMainButtonsCanvas->SetChildAlphaControl( true );
		mGUIMainButtonsCanvas->Attach( mGUICanvas );
		{
			mGUIContinueGameButton = ruButton::Create( buttonXOffset, 5, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString( "continueButton" ), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center );
			mGUIContinueGameButton->Attach( mGUIMainButtonsCanvas );
			mGUIContinueGameButton->AddAction( ruGUIAction::OnClick, ruDelegate::Bind( this, &Menu::OnContinueGameClick ));

			mGUIStartButton = ruButton::Create( buttonXOffset, 5 + 0.5f * mDistBetweenButtons, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString( "startButton" ), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center );
			mGUIStartButton->Attach( mGUIMainButtonsCanvas );
			mGUIStartButton->AddAction( ruGUIAction::OnClick, ruDelegate::Bind( this, &Menu::OnStartNewGameClick ));

			mGUISaveGameButton = ruButton::Create( buttonXOffset, 5 + 1.0f * mDistBetweenButtons, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString( "saveButton" ), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center );
			mGUISaveGameButton->Attach( mGUIMainButtonsCanvas );

			mGUILoadGameButton = ruButton::Create( buttonXOffset, 5 + 1.5f * mDistBetweenButtons, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString( "loadButton" ), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center );
			mGUILoadGameButton->Attach( mGUIMainButtonsCanvas );

			mGUIOptionsButton = ruButton::Create( buttonXOffset, 5 + 2.0f * mDistBetweenButtons, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString( "optionsButton" ), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center );
			mGUIOptionsButton->Attach( mGUIMainButtonsCanvas );
			mGUIOptionsButton->AddAction( ruGUIAction::OnClick, ruDelegate::Bind( this, &Menu::OnOptionsClick ));

			mGUIAuthorsButton = ruButton::Create( buttonXOffset, 5 + 2.5f * mDistBetweenButtons, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString( "authorsButton" ), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center );
			mGUIAuthorsButton->Attach( mGUIMainButtonsCanvas );

			mGUIExitButton = ruButton::Create( buttonXOffset, 5 + 3.0f * mDistBetweenButtons, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString( "exitButton" ), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center );
			mGUIExitButton->Attach( mGUIMainButtonsCanvas );
			mGUIExitButton->AddAction( ruGUIAction::OnClick, ruDelegate::Bind( this, &Menu::OnExitGameClick ));
		}

		int aTabX = 200;
		int aTabY = g_resH - 4.0 * mDistBetweenButtons;
		int aTabWidth = buttonWidth * 3;
		int aTabHeight = buttonHeight * 8;

		mpModalWindow = new ModalWindow( aTabX, aTabY, aTabWidth, aTabHeight, ruTexture::Request( "data/gui/menu/tab.tga" ), mButtonImage, pGUIProp->mBackColor );
		mpModalWindow->AttachTo( mGUICanvas );

		mGUIWindowText = ruText::Create( " ", aTabX, aTabY - 17, aTabWidth, 32, pGUIProp->mFont, ruVector3( 255, 255, 255 ), ruTextAlignment::Left );
		mGUIWindowText->Attach( mGUICanvas );

		mGUICaption = ruText::Create( "THE MINE", 20, aTabY - 17, aTabWidth, 32, pGUIProp->mFont, ruVector3( 255, 255, 255 ), ruTextAlignment::Left );
		mGUICaption->Attach( mGUICanvas );

		mGUIOptionsCanvas = ruRect::Create( aTabX, aTabY, aTabWidth, aTabHeight, tabTexture, pGUIProp->mBackColor );  
		mGUIOptionsCanvas->Attach( mGUICanvas );
		{
			int yOffset = (aTabHeight - 2 * mDistBetweenButtons ) / 2;

			mGUIOptionsCommonButton = ruButton::Create(  ( aTabWidth - buttonWidth ) / 2, yOffset, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString( "commonSettings" ), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center );
			mGUIOptionsCommonButton->Attach( mGUIOptionsCanvas );
			mGUIOptionsCommonButton->AddAction( ruGUIAction::OnClick, ruDelegate::Bind( this, &Menu::OnOptionsCommonClick ));

			mGUIOptionsControlsButton = ruButton::Create( ( aTabWidth - buttonWidth ) / 2, yOffset + 0.5 * mDistBetweenButtons, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString( "controls" ), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center );
			mGUIOptionsControlsButton->Attach( mGUIOptionsCanvas );
			mGUIOptionsControlsButton->AddAction( ruGUIAction::OnClick, ruDelegate::Bind( this, &Menu::OnOptionsControlsClick ));

			mGUIOptionsGraphicsButton = ruButton::Create(  ( aTabWidth - buttonWidth ) / 2, yOffset + mDistBetweenButtons, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString( "graphics" ), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center );
			mGUIOptionsGraphicsButton->Attach( mGUIOptionsCanvas );
			mGUIOptionsGraphicsButton->AddAction( ruGUIAction::OnClick, ruDelegate::Bind( this, &Menu::OnOptionsGraphicsClick ));
		}

		mGUIOptionsKeysCanvas = ruRect::Create( aTabX, aTabY, aTabWidth, aTabHeight, tabTexture, pGUIProp->mBackColor );    
		mGUIOptionsCanvas->Attach( mGUICanvas );
		mGUIOptionsKeysCanvas->SetVisible( false );
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

		mGUIOptionsGraphicsCanvas = ruRect::Create( aTabX, aTabY, aTabWidth, aTabHeight, tabTexture, pGUIProp->mBackColor );    
		mGUIOptionsGraphicsCanvas->Attach( mGUICanvas );
		mGUIOptionsGraphicsCanvas->SetVisible( false );
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

		mGUIAuthorsBackground = ruRect::Create( aTabX, aTabY, aTabWidth, aTabHeight, tabTexture, pGUIProp->mBackColor );  
		mGUIAuthorsBackground->Attach( mGUICanvas );
		{
			mGUIAuthorsText = ruText::Create( mLocalization.GetString( "authorsText" ), 15, 15, aTabWidth - 30, aTabHeight - 30, pGUIProp->mFont, ruVector3( 255, 255, 255 ), ruTextAlignment::Left );
			mGUIAuthorsText->Attach( mGUIAuthorsBackground );
		}

		mGUISaveGameCanvas = ruRect::Create( aTabX, aTabY, aTabWidth, aTabHeight, tabTexture, pGUIProp->mBackColor );      
		mGUISaveGameCanvas->Attach( mGUICanvas );
		{
			float y = 10;
			for( int i = 0; i < mSaveLoadSlotCount; i++ ) {
				mGUISaveGameSlot[i] = ruButton::Create( ( aTabWidth - buttonWidth ) / 2, y, buttonWidth, buttonHeight, mButtonImage, "Empty slot", pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center );
				mGUISaveGameSlot[i]->Attach( mGUISaveGameCanvas );
				mGUISaveGameSlot[i]->AddAction( ruGUIAction::OnClick, ruDelegate::Bind( this, &Menu::OnSaveClick ));
				y += 1.1f * buttonHeight;
			}
		}

		mGUILoadGameCanvas = ruRect::Create( aTabX, aTabY, aTabWidth, aTabHeight, tabTexture, pGUIProp->mBackColor );  
		mGUILoadGameCanvas->Attach( mGUICanvas );
		{
			float y = 10;
			for( int i = 0; i < mSaveLoadSlotCount; i++ ) {
				mGUILoadGameSlot[i] = ruButton::Create( ( aTabWidth - buttonWidth ) / 2, y, buttonWidth, buttonHeight, mButtonImage, "Empty slot", pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center );
				mGUILoadGameSlot[i]->Attach( mGUILoadGameCanvas );
				mGUILoadGameSlot[i]->AddAction( ruGUIAction::OnClick, ruDelegate::Bind( this, &Menu::OnLoadSaveClick ));
				y += 1.1f * buttonHeight;
			}
		}

		mGUIOptionsCommonCanvas = ruRect::Create( aTabX, aTabY, aTabWidth, aTabHeight, tabTexture, pGUIProp->mBackColor );  
		mGUIOptionsCommonCanvas->Attach( mGUICanvas );
		mGUIOptionsCommonCanvas->SetVisible( false );
		{
			int yOffset = (aTabHeight - 1.5 * mDistBetweenButtons ) / 2;
			int xOffset = aTabWidth / 6.5;

			mpMasterVolume = new Slider( xOffset, yOffset, 0, 100, 2.5f, ruTexture::Request( "data/gui/menu/smallbutton.tga" ), mLocalization.GetString( "masterVolume" ) );
			mpMasterVolume->AttachTo( mGUIOptionsCommonCanvas );
			mpMasterVolume->SetChangeAction( ruDelegate::Bind( this, &Menu::OnSoundVolumeChange ));

			mpMusicVolume = new Slider( xOffset, yOffset + 0.5f * mDistBetweenButtons, 0, 100, 2.5f, ruTexture::Request( "data/gui/menu/smallbutton.tga"), mLocalization.GetString(  "musicVolume" ) );
			mpMusicVolume->AttachTo( mGUIOptionsCommonCanvas );
			mpMusicVolume->SetChangeAction( ruDelegate::Bind( this, &Menu::OnMusicVolumeChange ));

			mpMouseSensivity = new Slider( xOffset, yOffset + 1.0f * mDistBetweenButtons, 0, 100, 2.5f, ruTexture::Request( "data/gui/menu/smallbutton.tga" ), mLocalization.GetString( "mouseSens" ) );
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
    mScene->Show();
    if( pCurrentLevel ) {
        pCurrentLevel->Hide();
    }
    if( pPlayer ) {
		pPlayer->SetBodyVisible( false );
        pPlayer->SetHUDVisible( false );
    }
    mVisible = true;
    ruEngine::ShowCursor();
    mGUICanvas->SetVisible( true );
	mGUICaption->SetVisible( true );
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
    
    mScene->Hide();
    if( pCurrentLevel ) {
        pCurrentLevel->Show();
    }
    mVisible = false;
    mPage = Page::Main;
    ruEngine::HideCursor();
    mMusic->Pause();
    mGUICanvas->SetVisible( false );
	mGUICaption->SetVisible( false );
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
	mMusic->SetVolume( mpMusicVolume->GetValue() / 100.0f );
	g_musicVolume = mpMusicVolume->GetValue() / 100.0f;
	if( pCurrentLevel ) {
		pCurrentLevel->mMusic->SetVolume( g_musicVolume );
	}
}
void Menu::OnSoundVolumeChange() {
	ruSound::SetMasterVolume( mpMasterVolume->GetValue() / 100.0f );
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
        if( ruInput::IsKeyHit( ruInput::Key::Esc ) ) {
			if( pCurrentLevel ) {
				CameraStartFadeOut( ruDelegate::Bind( this, &Menu::DoContinueGameCurrent ));
			}
        }

        CameraFloating();

		if( pPlayer ) {
			if( pPlayer->IsDead() ) {
				mGUIContinueGameButton->SetActive( false );
				mGUISaveGameButton->SetActive( false );
			} else {
				mGUIContinueGameButton->SetActive( true );
				mGUISaveGameButton->SetActive( true );
			}
		} else {
			mGUISaveGameButton->SetActive( false );
		}       

		mGUIMainButtonsCanvas->SetAlpha( mMainButtonsAlpha );

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
            mGUIOptionsCommonCanvas->SetVisible( true );			
        } else {
            mGUIOptionsCommonCanvas->SetVisible( false );
        }
		
        if( mPage == Page::LoadGame ) {
            mGUILoadGameCanvas->SetVisible( true );
            vector< string > nameList;
            GetFilesWithExtension( "*.save", nameList );
            int count = nameList.size();
            if( count >= mSaveLoadSlotCount ) {
                count = mSaveLoadSlotCount;
            }
			// inactivate all buttons
			for( int i = 0; i < mSaveLoadSlotCount; i++ ) {
				mGUILoadGameSlot[i]->SetActive( false );
			}
            for( int i = 0; i < count; i++ ) {
				// activate button associated with file
				mGUILoadGameSlot[i]->SetActive( true );
                mGUILoadGameSlot[i]->GetText()->SetText( nameList[i] );
                if( mGUILoadGameSlot[i]->IsHit() ) {
                    mLoadSaveGameName = nameList[i];
                    SetPage( Page::Main );
                }
            }
        } else {
            mGUILoadGameCanvas->SetVisible( false );
        }

        if( mPage == Page::SaveGame ) {
            mGUISaveGameCanvas->SetVisible( true );

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
                mGUISaveGameSlot[iName]->GetText()->SetText( nameList[iName] );
                if( mGUISaveGameSlot[iName]->IsHit() ) {
                    mSaveGameSlotName = nameList[iName];
                }
            }
        } else {
            mGUISaveGameCanvas->SetVisible( false );
        }

        if( mPage == Page::OptionsGraphics ) {
            mGUIOptionsGraphicsCanvas->SetVisible( true );

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
            mGUIOptionsGraphicsCanvas->SetVisible( false );
        }

        if( mPage == Page::OptionsKeys ) {
            mGUIOptionsKeysCanvas->SetVisible( true );
        } else {
            mGUIOptionsKeysCanvas->SetVisible( false );
        }

		mMusic->Play();

		if( mGUILoadGameButton->IsHit() ) {
			SetPage( Page::LoadGame );
		}
		if( mGUISaveGameButton->IsHit() ) {
			SetPage( Page::SaveGame );
		}
		if( mGUIAuthorsButton->IsHit() ) {
			SetPage( Page::Authors );
		}
        
    } else {
        if( ruInput::IsKeyHit( ruInput::Key::Esc ) ) {
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
	mGUIWindowText->SetText( " " );
	if( mPage == Page::Options ) {
		mGUIWindowText->SetText( mLocalization.GetString( "captionOptions" ));
	} 
	if( mPage == Page::OptionsGraphics ) {
		mGUIWindowText->SetText( mLocalization.GetString( "captionOptionsGraphics" ));
	}
	if( mPage == Page::OptionsKeys ) {
		mGUIWindowText->SetText( mLocalization.GetString( "captionOptionsControls" ));
	}
	if( mPage == Page::OptionsCommon ) {
		mGUIWindowText->SetText( mLocalization.GetString( "captionOptionsCommon" ));
	}
	if( mPage == Page::SaveGame ) {
		mGUIWindowText->SetText( mLocalization.GetString( "captionSaveGame" ));
	}
	if( mPage == Page::LoadGame ) {
		mGUIWindowText->SetText( mLocalization.GetString( "captionLoadGame" ));
	}
	if( mPage == Page::Authors ) {
		mGUIWindowText->SetText( mLocalization.GetString( "captionAuthors" ));
	}
}

void Menu::LoadTextures() {
    mButtonImage = ruTexture::Request( "data/gui/menu/button.tga" );
    mSmallButtonImage = ruTexture::Request( "data/gui/menu/button.tga" );
}

void Menu::CreateCamera() {
    mpCamera = new GameCamera;
	mCameraFadeActionDone = false;
    mCameraInitialPosition = mScene->FindChild( "Camera" )->GetPosition();
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
        mpMoveForwardKey->SetSelected( static_cast<ruInput::Key>( static_cast<int>( config.GetNumber( "keyMoveForward" ))));
        mpMoveBackwardKey->SetSelected( static_cast<ruInput::Key>( static_cast<int>( config.GetNumber( "keyMoveBackward" ))));
        mpStrafeLeftKey->SetSelected( static_cast<ruInput::Key>( static_cast<int>( config.GetNumber( "keyStrafeLeft" ))));
        mpStrafeRightKey->SetSelected( static_cast<ruInput::Key>( static_cast<int>( config.GetNumber( "keyStrafeRight" ))));
        mpJumpKey->SetSelected( static_cast<ruInput::Key>( static_cast<int>( config.GetNumber( "keyJump" ))));
        mpRunKey->SetSelected( static_cast<ruInput::Key>( static_cast<int>( config.GetNumber( "keyRun" ))));
        mpFlashLightKey->SetSelected( static_cast<ruInput::Key>( static_cast<int>( config.GetNumber( "keyFlashLight" ))));
        mpInventoryKey->SetSelected( static_cast<ruInput::Key>( static_cast<int>( config.GetNumber( "keyInventory" ))));
        mpUseKey->SetSelected( static_cast<ruInput::Key>( static_cast<int>( config.GetNumber( "keyUse" ))));

        mpQuickSaveKey->SetSelected( static_cast<ruInput::Key>( static_cast<int>( config.GetNumber( "keyQuickSave" ))));
        g_keyQuickSave = mpQuickSaveKey->GetSelectedKey();

        mpQuickLoadKey->SetSelected( static_cast<ruInput::Key>( static_cast<int>( config.GetNumber( "keyQuickLoad" ))));
        g_keyQuickLoad = mpQuickLoadKey->GetSelectedKey();

        mpStealthKey->SetSelected( static_cast<ruInput::Key>( static_cast<int>( config.GetNumber( "keyStealth" ))));

        mpSpotShadowsButton->SetEnabled( config.GetNumber( "spotShadowsEnabled" ) != 0 );
        ruEngine::EnableSpotLightShadows( mpSpotShadowsButton->IsEnabled() );

        ruSound::SetMasterVolume( mpMasterVolume->GetValue() / 100.0f );
        mMusic->SetVolume( mpMusicVolume->GetValue() / 100.0f );

        mpHDRButton->SetEnabled( config.GetNumber( "hdrEnabled" ) != 0 );
        ruEngine::SetHDREnabled( mpHDRButton->IsEnabled() );

		mpParallaxButton->SetEnabled( config.GetNumber( "parallax" ) != 0 );
		ruEngine::SetParallaxEnabled( mpParallaxButton->IsEnabled() );

        mpTextureFiltering->SetCurrentValue( config.GetNumber( "textureFiltering" ));
        ruEngine::SetAnisotropicTextureFiltration( mpTextureFiltering->GetCurrentValue() );

		mpLookLeftKey->SetSelected( static_cast<ruInput::Key>( static_cast<int>( config.GetNumber( "keyLookLeft" ))));
		mpLookRightKey->SetSelected( static_cast<ruInput::Key>( static_cast<int>( config.GetNumber( "keyLookRight" ))));

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
    WriteInteger( config, "keyMoveForward", static_cast<int>( mpMoveForwardKey->GetSelectedKey()));
    WriteInteger( config, "keyMoveBackward", static_cast<int>( mpMoveBackwardKey->GetSelectedKey()));
    WriteInteger( config, "keyStrafeLeft", static_cast<int>( mpStrafeLeftKey->GetSelectedKey()));
    WriteInteger( config, "keyStrafeRight", static_cast<int>( mpStrafeRightKey->GetSelectedKey()));
    WriteInteger( config, "keyJump", static_cast<int>( mpJumpKey->GetSelectedKey()));
    WriteInteger( config, "keyFlashLight", static_cast<int>( mpFlashLightKey->GetSelectedKey()));
    WriteInteger( config, "keyRun", static_cast<int>( mpRunKey->GetSelectedKey()));
    WriteInteger( config, "keyInventory", static_cast<int>( mpInventoryKey->GetSelectedKey()));
    WriteInteger( config, "keyUse", static_cast<int>( mpUseKey->GetSelectedKey()));
    WriteInteger( config, "keyQuickSave", static_cast<int>( mpQuickSaveKey->GetSelectedKey()));
    WriteInteger( config, "keyQuickLoad", static_cast<int>( mpQuickLoadKey->GetSelectedKey()));
    WriteInteger( config, "spotShadowsEnabled", ruEngine::IsSpotLightShadowsEnabled() ? 1 : 0 );
    WriteInteger( config, "hdrEnabled", ruEngine::IsHDREnabled() ? 1 : 0  );
    WriteInteger( config, "keyStealth", static_cast<int>( mpStealthKey->GetSelectedKey()));
    WriteInteger( config, "textureFiltering", mpTextureFiltering->GetCurrentValue() );
	WriteInteger( config, "keyLookLeft", static_cast<int>( mpLookLeftKey->GetSelectedKey()));
	WriteInteger( config, "keyLookRight", static_cast<int>( mpLookRightKey->GetSelectedKey()));
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
	mGUIMainButtonsCanvas->SetVisible( state );
}

void Menu::SetAuthorsPageVisible( bool state ) {
    mGUIAuthorsBackground->SetVisible( state );
}

void Menu::SetOptionsPageVisible( bool state ) {
    mGUIOptionsCanvas->SetVisible( state );
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

    mpCamera->mCamera->SetPosition( mCameraInitialPosition + mCameraAnimationOffset );
}

void Menu::OnHDRButtonClick()
{
	ruEngine::SetHDREnabled( mpHDRButton->IsEnabled() );
}
