#include "Menu.h"
#include "Level.h"
#include "GUI.h"
#include "Player.h"
#include "LevelArrival.h"
#include "LevelMine.h"
#include "SaveWriter.h"
#include "SaveLoader.h"

Menu * menu = 0;
bool g_continueGame = false;

Menu::Menu( ) {
    loc.ParseFile( localizationPath + "menu.loc" );

    scene = LoadScene( "data/maps/menu/menu.scene" );

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
    textFont = CreateGUIFont( 16, "Arial", false, false );
    textBackgroundFont = CreateGUIFont( 21, "Arial", false, false );

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

    for( int i = 0; i < GetWorldPointLightCount(); i++ ) {
        SetLightFloatingEnabled( GetWorldPointLight( i ), true );
        SetLightFloatingLimits( GetWorldPointLight( i ), Vector3( -.25, -.25, -.25 ), Vector3( .25, .25, .25 ) );
    }
}

void Menu::Show() {
    camera->MakeCurrent();
    camera->FadeIn();
    ShowNode( scene );
    if( currentLevel ) {
        currentLevel->Hide();
    }
    visible = true;
    ShowCursor();
    PausePhysics();
}

void Menu::Hide( ) {
    if( player ) {
        player->camera->MakeCurrent();
    }
    SetPlayerControls();
    HideNode( scene );
    if( currentLevel ) {
        currentLevel->Show();
    }
    visible = false;
    page = Page::Main;
    buttonsXOffset = 0;
    HideCursor();
    PauseSoundSource( music );
    ResumePhysics();
}

void Menu::Update( ) {
    if( visible ) {
        camera->Update();

        if( mi::KeyHit( mi::Esc ) ) {
            returnToGameByEsc = currentLevel != nullptr;
        }

        if( startPressed || continuePressed || returnToGameByEsc ) {
            camera->FadeOut();

            buttonsXOffset -= 6;

            if( camera->FadeComplete() ) {
                if( !currentLevel && continuePressed ) {
                    Level::Change( continueLevelName, true );
                }

                if( !currentLevel && startPressed ) {
                    Level::Change( g_initialLevel );
					//Level::Change( LXTestingChamber );
                }

                startPressed = false;
                continuePressed = false;
                returnToGameByEsc = false;

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
            cameraAnimationNewOffset = Vector3( frandom( -1.5, 1.5 ), frandom( -1.5, 1.5 ), frandom( -1.5, 1.5 ) );
        }

        SetPosition( camera->cameraNode, cameraInitialPosition + cameraAnimationOffset );

        int mainButtonsX = buttonsXOffset + 20;
        int startOffsetIfInGame = currentLevel ? 0.5 * distBetweenButtons : 0;

        GUIState continueGame;
        GUIState start;
        GUIState options;
        GUIState authors;
        GUIState exit;

        if( !autosaveNotify ) {
            if( currentLevel || canContinueGameFromLast ) {
                continueGame = DrawGUIButton( mainButtonsX, g_resH - 3.0 * distBetweenButtons + startOffsetIfInGame, 128, 32, buttonImage, loc.GetString( "continueButton" ), gui->font, Vector3( 0, 255, 0 ), 1 );
            }
            if( !currentLevel ) {
                start = DrawGUIButton( mainButtonsX, g_resH - 2.5 * distBetweenButtons, 128, 32, buttonImage, loc.GetString( "startButton" ), gui->font, Vector3( 0, 255, 0 ), 1 );
            }
            options = DrawGUIButton( mainButtonsX, g_resH - 2.0 * distBetweenButtons, 128, 32, buttonImage, loc.GetString( "optionsButton" ), gui->font, Vector3( 0, 255, 0 ), 1 );
            authors = DrawGUIButton( mainButtonsX, g_resH - 1.5 * distBetweenButtons, 128, 32, buttonImage, loc.GetString( "authorsButton" ), gui->font, Vector3( 0, 255, 0 ), 1 );
            exit = DrawGUIButton( mainButtonsX, g_resH - 1.0 * distBetweenButtons, 128, 32, buttonImage, loc.GetString( "exitButton" ), gui->font, Vector3( 0, 255, 0 ), 1 );
        } else {
            int ntfX = 400;
            int ntfY = g_resH / 2 - 64;
            int ntfW = g_resW - 800;
            DrawGUIRect( ntfX, ntfY, ntfW, 32, TextureHandle::Empty(), Vector3( 50, 0, 0 ), 200 );
            DrawGUIText( loc.GetString( "autosaveNotify" ), ntfX, ntfY, ntfW , 32, textFont, Vector3( 200, 0, 0 ), 1 );
            GUIState ok = DrawGUIButton( g_resW / 2 - 64, g_resH / 2 - 16, 128, 32, buttonImage, loc.GetString( "okButton" ), gui->font, Vector3( 0, 255, 0 ), 1 );
            if( ok.mouseLeftClicked ) {
                autosaveNotify = false;
            }
        }

        if( page == Page::Authors ) {
            int w = 500;
            int h = 400;
            int x = ( g_resW - w ) / 2;
            int y = ( g_resH - h ) / 2;
            DrawGUIRect( x, y, w, h, TextureHandle::Empty() );
            DrawGUIText( loc.GetString( "authorsText" ), x, y, w, h, gui->font, Vector3( 255, 127, 127 ), 1 );
        }

        if( page == Page::Options ) { // options window
            GUIState optionsCommon = DrawGUIButton( buttonsXOffset + 200, g_resH - 2.5 * distBetweenButtons, 128, 32, buttonImage, loc.GetString( "commonSettings" ), gui->font, Vector3( 0, 255, 0 ), 1 );
            GUIState optionsKeys = DrawGUIButton( buttonsXOffset + 200, g_resH - 2.0 * distBetweenButtons, 128, 32, buttonImage, loc.GetString( "controls" ), gui->font, Vector3( 0, 255, 0 ), 1 );
            GUIState optionsGraphics = DrawGUIButton( buttonsXOffset + 200, g_resH - 1.5 * distBetweenButtons, 128, 32, buttonImage, loc.GetString( "graphics" ), gui->font, Vector3( 0, 255, 0 ), 1 );

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
            SetMasterVolume( masterVolume->value / 100.0f );

            y = y + 1.5f * buttonHeight;

            musicVolume->Draw( x, y, buttonImage, loc.GetString(  "musicVolume" ));
            SetVolume( music, musicVolume->value / 100.0f );
			g_musicVolume = musicVolume->value / 100.0f;
			if( currentLevel ) {
				SetVolume( currentLevel->music, g_musicVolume );
			}
        }

        if( page == Page::OptionsGraphics ) {
            float x = buttonsXOffset + 200;
            float y = g_resH - 2.5 * distBetweenButtons;

            // FXAA Options
            fxaaButton->Draw( x, y, buttonImage, loc.GetString( "fxaa" ));
            if( fxaaButton->on ) {
                EnableFXAA();
            } else {
                DisableFXAA();
            }

            // show fps
            y = g_resH - 2.0 * distBetweenButtons;
            fpsButton->Draw( x, y, buttonImage, loc.GetString( "showFPS" ));
            g_showFPS = fpsButton->on;

            // texture filtering
            y = g_resH - 2.0 * distBetweenButtons;
            textureFiltering->Draw( x, y, buttonImage, loc.GetString( "filtering" ));
            if( textureFiltering->GetCurrentValue() == 0 ) {
                SetTextureFiltering( TextureFilter::Anisotropic, GetMaxAnisotropy() );
            } else {
                SetTextureFiltering( TextureFilter::Linear, 0 );
            }

            // shadows
            x += 250;

            y = g_resH - 2.5 * distBetweenButtons;
            pointShadowsButton->Draw( x, y, buttonImage, loc.GetString( "pointLightShadows" ));
            EnablePointLightShadows( pointShadowsButton->on );

            y = g_resH - 2.0 * distBetweenButtons;
            spotShadowsButton->Draw( x, y, buttonImage, loc.GetString( "spotLightShadows" ));
            EnableSpotLightShadows( spotShadowsButton->on );

			// hdr
			y = g_resH - 1.5 * distBetweenButtons;
			hdrButton->Draw( x, y, buttonImage, loc.GetString( "hdr" ));
			SetHDREnabled( hdrButton->on );
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
        }

        if( !startPressed && !returnToGameByEsc ) {
            PlaySoundSource( music );
            if( start.mouseLeftClicked ) {
                startPressed = true;
                SetPage( Page::Main );
            }
            if( exit.mouseLeftClicked ) {
                exitPressed = true;
                if( player && currentLevel ) {
                    if( !player->dead ) {
                        WriteProgressConfig();
                        SaveWriter( "lastGame.save" ).SaveWorldState();
                    }
                }
                SetPage( Page::Main );
            }
            if( continueGame.mouseLeftClicked ) {
                continuePressed = true;
                SetPage( Page::Main );
                if( !currentLevel ) {
                    Parser progress;
                    progress.ParseFile( "progress.cfg" );
                    continueLevelName = progress.GetNumber( "lastLevel" );
                }
            }
            if( options.mouseLeftClicked ) {
                SetPage( Page::Options );
            }
            if( authors.mouseLeftClicked ) {
                SetPage( Page::Authors );
            }
        }
    } else {
        if( mi::KeyHit( mi::Esc ) ) {
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
    buttonImage = GetTexture( "data/gui/button.png" );
    smallButtonImage = GetTexture( "data/gui/smallbutton.png" );
}

void Menu::CreateCamera() {
    camera = new GameCamera;
    cameraInitialPosition = GetPosition( FindByName( "Camera") );
    cameraAnimationNewOffset = Vector3( 0.5, 0.5, 0.5 );
}

void Menu::LoadSounds() {
    pickSound = CreateSound2D( "data/sounds/menupick.ogg" );
    music = CreateMusic( "data/music/menu.ogg" );
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
        fxaaButton->on = config.GetNumber( "masterVolume" );
		if( fxaaButton->on ) {
			EnableFXAA(); 
		} else {
			DisableFXAA();
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
        spotShadowsButton->on = config.GetNumber( "spotShadowsEnabled" );
        EnableSpotLightShadows( spotShadowsButton->on );
        pointShadowsButton->on = config.GetNumber( "pointShadowsEnabled" );
        EnablePointLightShadows( pointShadowsButton->on );
		SetMasterVolume( masterVolume->value / 100.0f );
		SetVolume( music, musicVolume->value / 100.0f );
		SetHDREnabled( hdrButton->on = (int)config.GetNumber( "hdrEnabled" ));
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
    WriteInteger( config, "spotShadowsEnabled", IsSpotLightShadowsEnabled() ? 1 : 0  );
    WriteInteger( config, "pointShadowsEnabled", IsPointLightShadowsEnabled() ? 1 : 0  );
	WriteInteger( config, "hdrEnabled", IsHDREnabled() ? 1 : 0  );
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
}

void Menu::WriteProgressConfig() {
    ofstream out( "progress.cfg" );
    WriteInteger( out, "lastLevel", (int)Level::curLevelID );
    out.close();
}
