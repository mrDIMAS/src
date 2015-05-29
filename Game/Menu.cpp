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
    mStartPressed = false;
    mExitPressed = false;
    mVisible = true;
    mPage = Page::Main;
    mContinuePressed = false;
    mExitingGame = false;
    mReturnToGameByEsc = false;
    mLoadSaveGameName = "";
    mLoadFromSave = false;
	mDoFade = false;
	mCurrentAction = Action::None;

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
			mGUIOptionsKeysButton = ruCreateGUIButton( 0, 0.5 * mDistBetweenButtons, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString( "controls" ), pGUI->mFont, buttonColor, 1 );
			mGUIOptionsGraphicsButton = ruCreateGUIButton( 0, mDistBetweenButtons, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString( "graphics" ), pGUI->mFont, buttonColor, 1 );

			ruAttachGUINode( mGUIOptionsCommonButton, mGUIOptionsCanvas );
			ruAttachGUINode( mGUIOptionsKeysButton, mGUIOptionsCanvas );
			ruAttachGUINode( mGUIOptionsGraphicsButton, mGUIOptionsCanvas );
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
			mpMusicVolume = new Slider( x, y + 0.5f * mDistBetweenButtons, 0, 100, 2.5f, ruGetTexture( "data/gui/smallbuttonthick.png" ), mLocalization.GetString(  "musicVolume" ) );
			mpMusicVolume->AttachTo( mGUIOptionsCommonCanvas );
			mpMouseSensivity = new Slider( x, y + 1.0f * mDistBetweenButtons, 0, 100, 2.5f, ruGetTexture( "data/gui/smallbuttonthick.png" ), mLocalization.GetString( "mouseSens" ) );
			mpMouseSensivity->AttachTo( mGUIOptionsCommonCanvas );
		}
	}


	
    ruSetHDREnabled( mpHDRButton->IsEnabled() );
    SetOptionsPageVisible( false );
    SetAuthorsPageVisible( false );
    SetPage( Page::Main );
	mMainButtonsAlpha = 255.0f;
    LoadConfig();
}

void Menu::Show() {
    mpCamera->MakeCurrent();
    mpCamera->FadeIn();
    ruShowNode( mScene );
    if( pCurrentLevel ) {
        pCurrentLevel->Hide();
    }
    if( pPlayer ) {
		pPlayer->SetBodyVisible( false );
        pPlayer->SetHUDVisible( false );
    }
    mVisible = true;
    ruShowCursor();
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
    
    ruHideNode( mScene );
    if( pCurrentLevel ) {
        pCurrentLevel->Show();
    }
    mVisible = false;
    mPage = Page::Main;
    ruHideCursor();
    ruPauseSound( mMusic );
    ruSetGUINodeVisible( mGUICanvas, false );
}

void Menu::OnStartNewGameClick() {
	mCurrentAction = Action::StartNewGame;
	mpModalWindow->Ask( mLocalization.GetString( "newGameQuestion" ) );
	mpModalWindow->SetYesAction( ruDelegate::Bind( this, &Menu::StartNewGame ));
	SetPage( Page::Main );
}

void Menu::OnExitGameClick() {
	mCurrentAction = Action::ExitGame;
	mpModalWindow->Ask( mLocalization.GetString( "endGameQuestion" ) );	
	mpModalWindow->SetYesAction( ruDelegate::Bind( this, &Menu::ExitGame ));
	SetPage( Page::Main );
}

void Menu::StartNewGame() {
	Level::Change( g_initialLevel );						
	Hide();
}

void Menu::ExitGame() {
	if( pPlayer && pCurrentLevel ) {
		if( !pPlayer->mDead ) {
			SaveWriter( "lastGame.save" ).SaveWorldState();
		}
	}
	g_running = false;
	WriteConfig();
}

void Menu::OnContinueGameClick() {
	if( !pCurrentLevel ) {
		mpModalWindow->Ask( mLocalization.GetString( "continueLastGameQuestion" ) );	
		mpModalWindow->SetYesAction( ruDelegate::Bind( this, &Menu::ContinueGameFromLast ));
	} else {
		Hide();
	}	
}

void Menu::ContinueGameFromLast() {
	SaveLoader( "lastGame.save" ).RestoreWorldState();
	Hide();
}

void Menu::Update( ) {
    ruSetAmbientColor( ruVector3( 25 / 255.0f, 25 / 255.0f, 25  / 255.0f));

    mpCamera->Update();

	SyncPlayerControls();

	mpModalWindow->Update();

    if( mVisible ) {

        if( ruIsKeyHit( KEY_Esc ) ) {
            mReturnToGameByEsc = pCurrentLevel != nullptr;
        }


		if( mCurrentAction != Action::None ) {
			if( mpModalWindow->IsAnswered() ) {	
				if( mpModalWindow->GetAnswer() == ModalWindow::Answer::Yes ) {
					mpCamera->FadeOut();
					if( mMainButtonsAlpha > 0 )	{
						mMainButtonsAlpha -= 10;
						if( mMainButtonsAlpha < 0 ) {
							mMainButtonsAlpha = 0;
						}
					}
					// wait until screen goes black
					if( mpCamera->FadeComplete() ) {
						if( mCurrentAction == Action::StartNewGame ) {	
							/*
							ruSetGUIButtonActive( mGUIStartButton, true );
							Level::Change( g_initialLevel );						
							Hide();*/
						}
						if( mCurrentAction == Action::ExitGame ) {

						}
						mpModalWindow->Reset();						
					}	
					mCurrentAction = Action::None;	
				} else {
					if( mCurrentAction == Action::StartNewGame ) {	
						ruSetGUIButtonActive( mGUIStartButton, true );
					}
				}
			}			
		}

	/*
        if( mStartPressed || mContinuePressed || mReturnToGameByEsc || mLoadFromSave ) {


//			if( answer == ModalWindow::Answer::Yes ) {
			
				mpCamera->FadeOut();
				if( mMainButtonsAlpha > 0 )	{
					mMainButtonsAlpha -= 10;
					if( mMainButtonsAlpha < 0 ) {
						mMainButtonsAlpha = 0;
					}
				}
				if( mpCamera->FadeComplete() ) {
					if( !pCurrentLevel && mContinuePressed ) {
						SaveLoader( "lastGame.save" ).RestoreWorldState();
					}
					if( mLoadFromSave ) {
						SaveLoader( mLoadSaveGameName ).RestoreWorldState();
					}

					bool startNewGame = !pCurrentLevel && mStartPressed;
					if( pPlayer && mStartPressed ) {
						if( pPlayer->IsDead() ) {
							startNewGame = true;
						}
					}


					if( startNewGame ) {
						Level::Change( g_initialLevel );
					}

					mStartPressed = false;
					mContinuePressed = false;
					mReturnToGameByEsc = false;
					mLoadFromSave = false;
					Hide();
					return;
				}			
			//}
        }
		*/
		/*
        if( mExitPressed ) {
            mExitingGame = true;
			mMainButtonsAlpha -= 10;
			if( mMainButtonsAlpha < 0 ) {
				mMainButtonsAlpha = 0;
			}
            mpCamera->FadeOut();
        }

        if( mExitingGame ) {
            if( mpCamera->FadeComplete() ) {
                g_running = false;

                WriteConfig();
            }
        }*/

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
            SetAuthorsPageVisible( true );
        } else {
            SetAuthorsPageVisible( false );
        }

        if( mPage == Page::Options ) {
            SetOptionsPageVisible( true );
            if( ruIsButtonHit( mGUIOptionsCommonButton )) {
                SetPage( Page::OptionsCommon );
            }
            if( ruIsButtonHit( mGUIOptionsKeysButton )) {
                SetPage( Page::OptionsKeys );
            }
            if( ruIsButtonHit( mGUIOptionsGraphicsButton )) {
                SetPage( Page::OptionsGraphics );
            }
        } else {
            SetOptionsPageVisible( false );
        }

        if( mPage == Page::OptionsCommon ) {
            ruSetGUINodeVisible( mGUIOptionsCommonCanvas, true );

            mpMouseSensivity->Update();
            mouseSens = mpMouseSensivity->GetValue() / 100.0f;

            mpMasterVolume->Update();
            ruSetMasterVolume( mpMasterVolume->GetValue() / 100.0f );

            mpMusicVolume->Update();
            ruSetSoundVolume( mMusic, mpMusicVolume->GetValue() / 100.0f );
            g_musicVolume = mpMusicVolume->GetValue() / 100.0f;
            if( pCurrentLevel ) {
                ruSetSoundVolume( pCurrentLevel->mMusic, g_musicVolume );
            }
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
            for( int i = 0; i < count; i++ ) {
                ruSetGUINodeText( ruGetButtonText( mGUILoadGameSlot[i] ), nameList[i] );
                if( ruIsButtonHit( mGUILoadGameSlot[i] ) ) {
                    mLoadSaveGameName = nameList[i];
                    mLoadFromSave = true;
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
                    if( pCurrentLevel ) {
                        SaveWriter( nameList[iName] ).SaveWorldState();
                        SetPage( Page::Main );
                        break;
                    }
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
                ruEnableFXAA();
            } else {
                ruDisableFXAA();
            }

            // show fps
            mpFPSButton->Update();
            g_showFPS = mpFPSButton->IsEnabled();

            // texture filtering
            mpTextureFiltering->Update( );
            if( mpTextureFiltering->GetCurrentValue() == 0 ) {
                ruSetAnisotropicTextureFiltration( false );
            } else {
                ruSetAnisotropicTextureFiltration( true );
            }

            mpSpotShadowsButton->Update();
            ruEnableSpotLightShadows( mpSpotShadowsButton->IsEnabled() );

            mpHDRButton->Update();
            ruSetHDREnabled( mpHDRButton->IsEnabled() );
        } else {
            ruSetGUINodeVisible( mGUIOptionsGraphicsCanvas, false );
        }

        if( mPage == Page::OptionsKeys ) {
            ruSetGUINodeVisible( mGUIOptionsKeysCanvas, true );
            mpMoveForwardKey->Update();
            mpMoveBackwardKey->Update();
            mpStrafeLeftKey->Update();
            mpStrafeRightKey->Update();
            mpJumpKey->Update();
            mpFlashLightKey->Update();
            mpRunKey->Update();
            mpInventoryKey->Update();
            mpUseKey->Update();
            mpQuickSaveKey->Update();
            mpQuickLoadKey->Update();
            mpStealthKey->Update();
			mpLookLeftKey->Update();
			mpLookRightKey->Update();
        } else {
            ruSetGUINodeVisible( mGUIOptionsKeysCanvas, false );
        }

        if( !mStartPressed && !mReturnToGameByEsc ) {
            ruPlaySound( mMusic );

            if( ruIsButtonHit( mGUIContinueGameButton ) ) {
                mContinuePressed = true;
                SetPage( Page::Main );
            }
            if( ruIsButtonHit( mGUILoadGameButton ) ) {
                SetPage( Page::LoadGame );
            }
            if( ruIsButtonHit( mGUISaveGameButton ) ) {
                SetPage( Page::SaveGame );
            }
            if( ruIsButtonHit( mGUIOptionsButton ) ) {
                SetPage( Page::Options );
            }
            if( ruIsButtonHit( mGUIAuthorsButton ) ) {
                SetPage( Page::Authors );
            }
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
    mCameraInitialPosition = ruGetNodePosition( ruFindByName( "Camera") );
    mCameraAnimationNewOffset = ruVector3( 0.5, 0.5, 0.5 );
}

void Menu::LoadSounds() {
    mPickSound = ruLoadSound2D( "data/sounds/menupick.ogg" );
    mMusic = ruLoadMusic( "data/music/menu.ogg" );
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
            ruEnableFXAA();
        } else {
            ruDisableFXAA();
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
        ruEnableSpotLightShadows( mpSpotShadowsButton->IsEnabled() );

        ruSetMasterVolume( mpMasterVolume->GetValue() / 100.0f );
        ruSetSoundVolume( mMusic, mpMusicVolume->GetValue() / 100.0f );

        mpHDRButton->SetEnabled( config.GetNumber( "hdrEnabled" ) != 0 );
        ruSetHDREnabled( mpHDRButton->IsEnabled() );

        mpTextureFiltering->SetCurrentValue( config.GetNumber( "textureFiltering" ));
        ruSetAnisotropicTextureFiltration( mpTextureFiltering->GetCurrentValue() );

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
    WriteInteger( config, "spotShadowsEnabled", ruIsSpotLightShadowsEnabled() ? 1 : 0 );
    WriteInteger( config, "hdrEnabled", ruIsHDREnabled() ? 1 : 0  );
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

    ruSetNodePosition( mpCamera->mNode, mCameraInitialPosition + mCameraAnimationOffset );
}
