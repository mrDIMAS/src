#include "Menu.h"
#include "Level.h"
#include "GUI.h"
#include "Player.h"
#include "LevelArrival.h"
#include "LevelMine.h"
#include "SaveWriter.h"
#include "SaveLoader.h"
#include "Utils.h"

Menu * menu = 0;
bool g_continueGame = false;

Menu::Menu( ) {
    loc.ParseFile( localizationPath + "menu.loc" );

    scene = ruLoadScene( "data/maps/menu/menu.scene" );

    distBetweenButtons = 72;
    startPressed = false;
    fadeSpeed = 0.1f;
    exitPressed = false;
    buttonsXOffset = 0;
    visible = true;
    page = Page::Main;
    continuePressed = false;
    exitingGame = false;
    returnToGameByEsc = false;
    autosaveNotify = false; // FIXED: set it to true when build release version
	loadSaveGameName = "";
	loadFromSave = false;
    CreateCamera();
    LoadSounds();
    LoadTextures();
    CreateSliders();
    CreateRadioButtons();
    CreateWaitKeys();
    CreateLists();
    LoadConfig();

    ifstream file( "lastGame.save" );
    canContinueGameFromLast = file.good();
    file.close();

    for( int i = 0; i < ruGetWorldPointLightCount(); i++ ) {
        ruSetLightFloatingEnabled( ruGetWorldPointLight( i ), true );
        ruSetLightFloatingLimits( ruGetWorldPointLight( i ), ruVector3( -.25, -.25, -.25 ), ruVector3( .25, .25, .25 ) );
    }
}

void Menu::Show() {
    camera->MakeCurrent();
    camera->FadeIn();
    ruShowNode( scene );
    if( currentLevel ) {
        currentLevel->Hide();
    }
    visible = true;
    ruShowCursor();
    ruPausePhysics();
}

void Menu::Hide( ) {
    if( player ) {
        player->camera->MakeCurrent();
    }
    SetPlayerControls();
    ruHideNode( scene );
    if( currentLevel ) {
        currentLevel->Show();
    }
    visible = false;
    page = Page::Main;
    buttonsXOffset = 0;
    ruHideCursor();
    ruPauseSound( music );
    ruResumePhysics();
}

void Menu::Update( ) {
	ruSetAmbientColor( ruVector3( 0, 0, 0 ));

    if( visible ) {
        camera->Update();

        if( ruIsKeyHit( KEY_Esc ) ) {
            returnToGameByEsc = currentLevel != nullptr;
        }

        if( startPressed || continuePressed || returnToGameByEsc || loadFromSave ) {
            camera->FadeOut();

            buttonsXOffset -= 6;

            if( camera->FadeComplete() ) {
                if( !currentLevel && continuePressed ) {
					SaveLoader( "lastGame.save" ).RestoreWorldState();
                }
				if( loadFromSave ) {
					SaveLoader( loadSaveGameName ).RestoreWorldState();
				}
                if( !currentLevel && startPressed ) {
                    Level::Change( g_initialLevel );
                }
                startPressed = false;
                continuePressed = false;
                returnToGameByEsc = false;
				loadFromSave = false;
                Hide();
                return;
            }
        }

        if( menu->exitPressed ) {
            exitingGame = true;
            menu->camera->FadeOut();
        }

        if( exitingGame ) {
            menu->buttonsXOffset -= 12;

            if( menu->camera->FadeComplete() ) {
                g_running = false;

                WriteConfig();
            }
        }

        cameraAnimationOffset = cameraAnimationOffset.Lerp( cameraAnimationNewOffset, 0.0085f );

        if( ( cameraAnimationOffset - cameraAnimationNewOffset ).Length2() < 0.025 ) {
            cameraAnimationNewOffset = ruVector3( frandom( -1.5, 1.5 ), frandom( -1.5, 1.5 ), frandom( -1.5, 1.5 ) );
        }

        ruSetNodePosition( camera->cameraNode, cameraInitialPosition + cameraAnimationOffset );

        int mainButtonsX = buttonsXOffset + 20;
        int startOffsetIfInGame = currentLevel ? 0.5 * distBetweenButtons : 0;

        ruGUIState continueGame;
        ruGUIState start;
		ruGUIState saveGame;
		ruGUIState loadGame;
        ruGUIState options;
        ruGUIState authors;
        ruGUIState exit;

        if( !autosaveNotify ) {
			int saveGamePosX = ( page == Page::SaveGame ) ? mainButtonsX + 20 : mainButtonsX;
			int loadGamePosX = ( page == Page::LoadGame ) ? mainButtonsX + 20 : mainButtonsX;
			int optionsPosX = ( page == Page::Options || page == Page::OptionsGraphics || page == Page::OptionsKeys || page == Page::OptionsCommon ) ? mainButtonsX + 20 : mainButtonsX;
            if( currentLevel || canContinueGameFromLast ) {
                continueGame = ruDrawGUIButton( mainButtonsX, g_resH - 4.0 * distBetweenButtons + startOffsetIfInGame, 128, 32, buttonImage, loc.GetString( "continueButton" ), gui->font, ruVector3( 0, 255, 0 ), 1 );
            }
            if( !currentLevel ) {
                start = ruDrawGUIButton( mainButtonsX, g_resH - 3.5 * distBetweenButtons, 128, 32, buttonImage, loc.GetString( "startButton" ), gui->font, ruVector3( 0, 255, 0 ), 1 );
            }
			saveGame = ruDrawGUIButton( saveGamePosX, g_resH - 3.0 * distBetweenButtons, 128, 32, buttonImage, loc.GetString( "saveButton" ), gui->font, ruVector3( 0, 255, 0 ), 1 );
			loadGame = ruDrawGUIButton( loadGamePosX, g_resH - 2.5 * distBetweenButtons, 128, 32, buttonImage, loc.GetString( "loadButton" ), gui->font, ruVector3( 0, 255, 0 ), 1 );
            options = ruDrawGUIButton( optionsPosX, g_resH - 2.0 * distBetweenButtons, 128, 32, buttonImage, loc.GetString( "optionsButton" ), gui->font, ruVector3( 0, 255, 0 ), 1 );
            authors = ruDrawGUIButton( mainButtonsX, g_resH - 1.5 * distBetweenButtons, 128, 32, buttonImage, loc.GetString( "authorsButton" ), gui->font, ruVector3( 0, 255, 0 ), 1 );
            exit = ruDrawGUIButton( mainButtonsX, g_resH - 1.0 * distBetweenButtons, 128, 32, buttonImage, loc.GetString( "exitButton" ), gui->font, ruVector3( 0, 255, 0 ), 1 );
        } else {
            int ntfX = 400;
            int ntfY = g_resH / 2 - 64;
            int ntfW = g_resW - 800;
            ruDrawGUIRect( ntfX, ntfY, ntfW, 32, ruTextureHandle::Empty(), ruVector3( 50, 0, 0 ), 200 );
            ruDrawGUIText( loc.GetString( "autosaveNotify" ), ntfX, ntfY, ntfW , 32, gui->font, ruVector3( 200, 0, 0 ), 1 );
            ruGUIState ok = ruDrawGUIButton( g_resW / 2 - 64, g_resH / 2 - 16, 128, 32, buttonImage, loc.GetString( "okButton" ), gui->font, ruVector3( 0, 255, 0 ), 1 );
            if( ok.mouseLeftClicked ) {
                autosaveNotify = false;
            }
        }

        if( page == Page::Authors ) {
            int w = 500;
            int h = 400;
            int x = ( g_resW - w ) / 2;
            int y = ( g_resH - h ) / 2;
            ruDrawGUIRect( x, y, w, h, ruTextureHandle::Empty() );
            ruDrawGUIText( loc.GetString( "authorsText" ), x + 50, y + 50, w - 100, h - 100, gui->font, ruVector3( 255, 127, 127 ), 0 );
        }

        if( page == Page::Options ) { // options window
            ruGUIState optionsCommon = ruDrawGUIButton( buttonsXOffset + 200, g_resH - 2.5 * distBetweenButtons, 128, 32, buttonImage, loc.GetString( "commonSettings" ), gui->font, ruVector3( 0, 255, 0 ), 1 );
            ruGUIState optionsKeys = ruDrawGUIButton( buttonsXOffset + 200, g_resH - 2.0 * distBetweenButtons, 128, 32, buttonImage, loc.GetString( "controls" ), gui->font, ruVector3( 0, 255, 0 ), 1 );
            ruGUIState optionsGraphics = ruDrawGUIButton( buttonsXOffset + 200, g_resH - 1.5 * distBetweenButtons, 128, 32, buttonImage, loc.GetString( "graphics" ), gui->font, ruVector3( 0, 255, 0 ), 1 );

            if( optionsCommon.mouseLeftClicked ) {
                SetPage( Page::OptionsCommon );
            }

            if( optionsKeys.mouseLeftClicked ) {
                SetPage( Page::OptionsKeys );
            }

            if( optionsGraphics.mouseLeftClicked ) {
                SetPage( Page::OptionsGraphics );
            }
        }

        if( page == Page::OptionsCommon ) {
            float x = buttonsXOffset + 200;
            float y = g_resH - 2.5 * distBetweenButtons;

            float buttonWidth = 80;
            float buttonHeight = 32;
            float textX = x + buttonWidth * 1.5f;

            mouseSensivity->Draw( x, y - 3.0f * buttonHeight, buttonImage, loc.GetString( "mouseSens" ));
            mouseSens = mouseSensivity->value / 100.0f;

            buttonWidth = 32;

            masterVolume->Draw( x, y, buttonImage, loc.GetString( "masterVolume" ));
            ruSetMasterVolume( masterVolume->value / 100.0f );

            y = y + 1.5f * buttonHeight;

            musicVolume->Draw( x, y, buttonImage, loc.GetString(  "musicVolume" ));
            ruSetSoundVolume( music, musicVolume->value / 100.0f );
			g_musicVolume = musicVolume->value / 100.0f;
			if( currentLevel ) {
				ruSetSoundVolume( currentLevel->music, g_musicVolume );
			}
        }

		if( page == Page::LoadGame ) {
			float x = buttonsXOffset + 200;
			float y = g_resH - 4.0 * distBetweenButtons;

			float buttonWidth = 80;
			float buttonHeight = 32;
			float textX = x + buttonWidth * 1.5f;

			vector< string > names;
			GetFilesWithDefExt( "*.save", names );

			for( int i = 0; i < names.size(); i++ ) {
				ruGUIState save = ruDrawGUIButton( x, y, 128, 32, buttonImage, names[i].c_str(), gui->font, ruVector3( 0, 255, 0 ), 1 );
				if( save.mouseLeftClicked ) {
					loadSaveGameName = names[i];
					loadFromSave = true;
					SetPage( Page::Main );
				}
				y += 1.1f * buttonHeight;
			}
		}
		
		if( page == Page::SaveGame ) {
			float x = buttonsXOffset + 200;
			float y = g_resH - 4.0 * distBetweenButtons;

			float buttonWidth = 80;
			float buttonHeight = 32;
			float textX = x + buttonWidth * 1.5f;

			vector< string > names;
			GetFilesWithDefExt( "*.save", names );

			for( int i = names.size() - 1; i < 6; i++ ) {
				string saveName = "Slot";
				saveName += ( (char)i + (char)'0' );
				saveName += ".save";
				names.push_back( saveName );
			}

			for( int i = 0; i < names.size(); i++ ) {
				ruGUIState save = ruDrawGUIButton( x, y, 128, 32, buttonImage, names[i].c_str(), gui->font, ruVector3( 0, 255, 0 ), 1 );
				if( save.mouseLeftClicked ) {
					if( currentLevel ) {
						SaveWriter( names[i] ).SaveWorldState();
						SetPage( Page::Main );
						break;
					}
				}
				y += 1.1f * buttonHeight;
			}
		}

        if( page == Page::OptionsGraphics ) {
			// first column
            float x = buttonsXOffset + 200;
            float y = g_resH - 2.5 * distBetweenButtons;

            // FXAA Options
            fxaaButton->Draw( x + 60, y, buttonImage, loc.GetString( "fxaa" ));
            if( fxaaButton->on ) {
                ruEnableFXAA();
            } else {
                ruDisableFXAA();
            }

            // show fps
            y = g_resH - 2.0 * distBetweenButtons;
            fpsButton->Draw( x + 60, y, buttonImage, loc.GetString( "showFPS" ));
            g_showFPS = fpsButton->on;

            // texture filtering
            y = g_resH - 2.0 * distBetweenButtons;
            textureFiltering->Draw( x, y, buttonImage, loc.GetString( "filtering" ));
            if( textureFiltering->GetCurrentValue() == 0 ) {
                ruSetRendererTextureFiltering( ruTextureFilter::Anisotropic, ruGetRendererMaxAnisotropy() );
            } else {
                ruSetRendererTextureFiltering( ruTextureFilter::Linear, 0 );
            }

			y = g_resH - 1.5 * distBetweenButtons;
			graphicsQuality->Draw( x, y, buttonImage, loc.GetString( "graphQuality"));
			if( graphicsQuality->GetCurrentValue() == 0 ) {// low 
				ruSetRenderQuality( 0 );
			} else { // high
				ruSetRenderQuality( 1 );
			}
			// second column
            x += 280;

            y = g_resH - 2.5 * distBetweenButtons;
            pointShadowsButton->Draw( x, y, buttonImage, loc.GetString( "pointLightShadows" ));
            ruEnablePointLightShadows( pointShadowsButton->on );

            y = g_resH - 2.0 * distBetweenButtons;
            spotShadowsButton->Draw( x, y, buttonImage, loc.GetString( "spotLightShadows" ));
            ruEnableSpotLightShadows( spotShadowsButton->on );

			// hdr
			y = g_resH - 1.5 * distBetweenButtons;
			hdrButton->Draw( x, y, buttonImage, loc.GetString( "hdr" ));
			ruSetHDREnabled( hdrButton->on );
        }

        if( page == Page::OptionsKeys ) {
            float x = buttonsXOffset + 200;
            float y = g_resH - 2.5 * distBetweenButtons;

            // First column
            wkMoveForward->Draw( x, y, smallButtonImage, loc.GetString( "forward" ) );

            y += 32 * 1.1f;
            wkMoveBackward->Draw( x, y, smallButtonImage, loc.GetString( "backward" ) );

            y += 32 * 1.1f;
            wkStrafeLeft->Draw( x, y, smallButtonImage, loc.GetString( "strafeLeft" ) );

            y += 32 * 1.1f;
            wkStrafeRight->Draw( x, y, smallButtonImage, loc.GetString( "strafeRight" ) );

            // Second column
            x += 150;
            y = g_resH - 2.5 * distBetweenButtons;

            wkJump->Draw( x, y, smallButtonImage, loc.GetString( "jump" ) );

            y += 32 * 1.1f;
            wkFlashLight->Draw( x, y, smallButtonImage, loc.GetString( "flashLight" ) );

            y += 32 * 1.1f;
            wkRun->Draw( x, y, smallButtonImage, loc.GetString( "run" ) );

            y += 32 * 1.1f;
            wkInventory->Draw( x, y, smallButtonImage, loc.GetString( "inventory" ) );

            // Third column
            x += 150;
            y = g_resH - 2.5 * distBetweenButtons;

            wkUse->Draw( x, y, smallButtonImage, loc.GetString( "use" ) );

            y += 32 * 1.1f;
            wkQuickSave->Draw( x, y, smallButtonImage, loc.GetString( "quickSave" ));

            y += 32 * 1.1f;
            wkQuickLoad->Draw( x, y, smallButtonImage, loc.GetString( "quickLoad" ) );

			y += 32 * 1.1f;
			wkStealth->Draw( x, y, smallButtonImage, loc.GetString( "stealth" ) );
        }

        if( !startPressed && !returnToGameByEsc ) {
            ruPlaySound( music );
            if( start.mouseLeftClicked ) {
                startPressed = true;
                SetPage( Page::Main );
            }
            if( exit.mouseLeftClicked ) {
                exitPressed = true;
                if( player && currentLevel ) {
                    if( !player->dead ) {
                        SaveWriter( "lastGame.save" ).SaveWorldState();
                    }
                }
                SetPage( Page::Main );
            }
            if( continueGame.mouseLeftClicked ) {
                continuePressed = true;
                SetPage( Page::Main );
            }
			if( loadGame.mouseLeftClicked ) {
				SetPage( Page::LoadGame );
			}
			if( saveGame.mouseLeftClicked ) {
				SetPage( Page::SaveGame );
			}
            if( options.mouseLeftClicked ) {
                SetPage( Page::Options );
            }
            if( authors.mouseLeftClicked ) {
                SetPage( Page::Authors );
            }
        }
    } else {
        if( ruIsKeyHit( KEY_Esc ) ) {
            menu->Show();
        }
    }
}

void Menu::SetPage( Page page ) {
    if( this->page == page ) {
        this->page = Page::Main;
    } else {
        this->page = page;
    }
}

void Menu::LoadTextures() {
    buttonImage = ruGetTexture( "data/gui/button.png" );
    smallButtonImage = ruGetTexture( "data/gui/smallbutton.png" );
}

void Menu::CreateCamera() {
    camera = new GameCamera;
    cameraInitialPosition = ruGetNodePosition( ruFindByName( "Camera") );
    cameraAnimationNewOffset = ruVector3( 0.5, 0.5, 0.5 );
}

void Menu::LoadSounds() {
    pickSound = ruLoadSound2D( "data/sounds/menupick.ogg" );
    music = ruLoadMusic( "data/music/menu.ogg" );
}

void Menu::CreateWaitKeys() {
    wkMoveForward = new WaitKeyButton();
    wkMoveBackward = new WaitKeyButton();
    wkStrafeLeft = new WaitKeyButton();
    wkStrafeRight = new WaitKeyButton();
    wkJump = new WaitKeyButton();
    wkFlashLight = new WaitKeyButton();
    wkRun = new WaitKeyButton();
    wkInventory = new WaitKeyButton();
    wkUse = new WaitKeyButton();
    wkQuickLoad = new WaitKeyButton();
    wkQuickSave = new WaitKeyButton();
	wkStealth = new WaitKeyButton();
}

void Menu::CreateRadioButtons() {
    fxaaButton = new RadioButton;
    fpsButton = new RadioButton;
    pointShadowsButton = new RadioButton;
    spotShadowsButton = new RadioButton;
	hdrButton = new RadioButton;
}

void Menu::CreateSliders() {
    masterVolume = new NumericSlider( 0, 100, 2.5f );
    musicVolume = new NumericSlider( 0, 100, 2.5f );
    mouseSensivity = new NumericSlider( 0, 100, 2.5f );
}

void Menu::LoadConfig() {
    // Load config
    Parser config;
    config.ParseFile( "config.cfg" );

    if( !config.Empty() ) {
        masterVolume->value = config.GetNumber( "masterVolume" );
        g_musicVolume = musicVolume->value = config.GetNumber( "musicVolume" );
        mouseSensivity->value = config.GetNumber( "mouseSens" );
        fxaaButton->on = (int)config.GetNumber( "fxaaEnabled" );
		if( fxaaButton->on ) {
			ruEnableFXAA(); 
		} else {
			ruDisableFXAA();
		};
        wkMoveForward->SetSelected( config.GetNumber( "keyMoveForward" ) );
        wkMoveBackward->SetSelected( config.GetNumber( "keyMoveBackward" ) );
        wkStrafeLeft->SetSelected( config.GetNumber( "keyStrafeLeft" ) );
        wkStrafeRight->SetSelected( config.GetNumber( "keyStrafeRight" ) );
        wkJump->SetSelected( config.GetNumber( "keyJump" ) );
        wkRun->SetSelected( config.GetNumber( "keyRun" ) );
        wkFlashLight->SetSelected( config.GetNumber( "keyFlashLight" ) );
        wkInventory->SetSelected( config.GetNumber( "keyInventory" ) );
        wkUse->SetSelected( config.GetNumber( "keyUse" ) );
        wkQuickSave->SetSelected( g_keyQuickSave = config.GetNumber( "keyQuickSave" ) );
        wkQuickLoad->SetSelected( g_keyQuickLoad = config.GetNumber( "keyQuickLoad" ) );
		wkStealth->SetSelected(  config.GetNumber( "keyStealth" ) );
        spotShadowsButton->on = (int)config.GetNumber( "spotShadowsEnabled" );
        ruEnableSpotLightShadows( spotShadowsButton->on );
        pointShadowsButton->on = (int)config.GetNumber( "pointShadowsEnabled" );
        ruEnablePointLightShadows( pointShadowsButton->on );
		ruSetMasterVolume( masterVolume->value / 100.0f );
		ruSetSoundVolume( music, musicVolume->value / 100.0f );
		ruSetHDREnabled( hdrButton->on = (int)config.GetNumber( "hdrEnabled" ));
    }
}

void Menu::SetPlayerControls() {
    if( player ) {
        player->keyMoveForward = wkMoveForward->selectedKey;
        player->keyMoveBackward = wkMoveBackward->selectedKey;
        player->keyStrafeLeft = wkStrafeLeft->selectedKey;
        player->keyStrafeRight = wkStrafeRight->selectedKey;
        player->keyJump = wkJump->selectedKey;
        player->keyRun = wkRun->selectedKey;
        player->keyFlashLight = wkFlashLight->selectedKey;
        player->keyInventory = wkInventory->selectedKey;
        player->keyUse = wkUse->selectedKey;
		player->keyStealth = wkStealth->selectedKey;
    }
}

void Menu::WriteConfig() {
    ofstream config( "config.cfg" );
    WriteFloat( config, "mouseSens", mouseSensivity->value );
    WriteFloat( config, "masterVolume", masterVolume->value );
    WriteFloat( config, "musicVolume", musicVolume->value );
    WriteFloat( config, "fxaaEnabled", fxaaButton->on );
    WriteInteger( config, "keyMoveForward", wkMoveForward->selectedKey );
    WriteInteger( config, "keyMoveBackward", wkMoveBackward->selectedKey );
    WriteInteger( config, "keyStrafeLeft", wkStrafeLeft->selectedKey );
    WriteInteger( config, "keyStrafeRight", wkStrafeRight->selectedKey );
    WriteInteger( config, "keyJump", wkJump->selectedKey );
    WriteInteger( config, "keyFlashLight", wkFlashLight->selectedKey );
    WriteInteger( config, "keyRun", wkRun->selectedKey );
    WriteInteger( config, "keyInventory", wkInventory->selectedKey );
    WriteInteger( config, "keyUse", wkUse->selectedKey );
    WriteInteger( config, "keyQuickSave", wkQuickSave->selectedKey );
    WriteInteger( config, "keyQuickLoad", wkQuickLoad->selectedKey );    
    WriteInteger( config, "spotShadowsEnabled", ruIsSpotLightShadowsEnabled() ? 1 : 0  );
    WriteInteger( config, "pointShadowsEnabled", ruIsPointLightShadowsEnabled() ? 1 : 0  );
	WriteInteger( config, "hdrEnabled", ruIsHDREnabled() ? 1 : 0  );
	WriteInteger( config, "keyStealth", wkStealth->selectedKey );
    config.close();
}

void Menu::Writestring( ofstream & stream, string name, string value ) {
    stream << name << "=\"" << value << "\";\n";
}

void Menu::WriteInteger( ofstream & stream, string name, int value ) {
    stream << name << "=\"" << value << "\";\n";
}

void Menu::WriteFloat( ofstream & stream, string name, float value ) {
    stream << name << "=\"" << value << "\";\n";
}

void Menu::CreateLists() {
    textureFiltering = new List();

    textureFiltering->AddValue( loc.GetString( "anisotropic" ));
    textureFiltering->AddValue( loc.GetString( "trilinear" ));

	graphicsQuality = new List();
	graphicsQuality->AddValue( loc.GetString( "gqLow" ));
	graphicsQuality->AddValue( loc.GetString( "gqHigh" ));
}

Menu::~Menu() {
    delete masterVolume;
    delete mouseSensivity;
    delete musicVolume;
    delete fxaaButton;
    delete textureFiltering;
    delete wkMoveForward;
    delete wkMoveBackward;
    delete wkStrafeLeft;
    delete wkStrafeRight;
    delete wkJump;
    delete wkInventory;
    delete wkRun;
    delete wkFlashLight;
    delete camera;
    delete wkUse;
    delete wkQuickLoad;
    delete wkQuickSave;
    delete pointShadowsButton;
	delete hdrButton;
    delete spotShadowsButton;
	delete wkStealth;
	delete fpsButton;
	delete graphicsQuality;
}
