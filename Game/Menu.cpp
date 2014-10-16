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

Menu::Menu( )
{
  LoadLocalizationFromFile( localizationPath + "menu.loc" );

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
  autosaveNotify = true;
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
}

void Menu::Show() 
{
  camera->MakeCurrent();
  camera->FadeIn();
  ShowNode( scene );
  if( currentLevel )
    currentLevel->Hide();
  visible = true;
  ShowCursor();
  PausePhysics();
}

void Menu::Hide( )
{
  if( player )
    player->camera->MakeCurrent();
  SetPlayerControls();
  HideNode( scene );
  if( currentLevel )
    currentLevel->Show();
  visible = false;
  page = Page::Main;
  buttonsXOffset = 0;
  HideCursor();
  PauseSoundSource( music );
  ResumePhysics();
}

void Menu::Update( )
{
  if( visible )
  {
    camera->Update();

    if( mi::KeyHit( mi::Esc ) )
      returnToGameByEsc = currentLevel != nullptr;

    if( startPressed || continuePressed || returnToGameByEsc )
    {
      camera->FadeOut();

      buttonsXOffset -= 6;

      if( camera->FadeComplete() )
      { 
        if( !currentLevel && continuePressed )
          Level::Change( continueLevelName, true );

        if( !currentLevel && startPressed )
          Level::Change( g_initialLevel );

        startPressed = false;
        continuePressed = false;
        returnToGameByEsc = false;

        Hide();

        return;
      }    
    }
    
    if( menu->exitPressed )
    {
      exitingGame = true;
      menu->camera->FadeOut();
    }

    if( exitingGame )
    {
      menu->buttonsXOffset -= 12;

      if( menu->camera->FadeComplete() )
      {
        g_running = false;

        WriteConfig();
      }
    }

    cameraAnimationOffset = cameraAnimationOffset.Lerp( cameraAnimationNewOffset, 0.0085f );  

    if( ( cameraAnimationOffset - cameraAnimationNewOffset ).Length2() < 0.025 )
      cameraAnimationNewOffset = Vector3( frandom( -1.5, 1.5 ), frandom( -1.5, 1.5 ), frandom( -1.5, 1.5 ) );

    SetPosition( camera->cameraNode, cameraInitialPosition + cameraAnimationOffset );

    int mainButtonsX = buttonsXOffset + 20;
    int startOffsetIfInGame = currentLevel ? 0.5 * distBetweenButtons : 0;

    GUIState continueGame; 
    GUIState start; 
    GUIState options;
    GUIState authors;
    GUIState exit;

    if( !autosaveNotify )
    {
      if( currentLevel || canContinueGameFromLast )
        continueGame = DrawGUIButton( mainButtonsX, GetResolutionHeight() - 3.0 * distBetweenButtons + startOffsetIfInGame, 128, 32, buttonImage, loc[ "continueButton" ].c_str(), gui->font, Vector3( 0, 255, 0 ), 1 );   
      if( !currentLevel )
        start = DrawGUIButton( mainButtonsX, GetResolutionHeight() - 2.5 * distBetweenButtons, 128, 32, buttonImage, loc[ "startButton" ].c_str(), gui->font, Vector3( 0, 255, 0 ), 1 );
      options = DrawGUIButton( mainButtonsX, GetResolutionHeight() - 2.0 * distBetweenButtons, 128, 32, buttonImage, loc[ "optionsButton" ].c_str(), gui->font, Vector3( 0, 255, 0 ), 1 );
      authors = DrawGUIButton( mainButtonsX, GetResolutionHeight() - 1.5 * distBetweenButtons, 128, 32, buttonImage, loc[ "authorsButton" ].c_str(), gui->font, Vector3( 0, 255, 0 ), 1 );
      exit = DrawGUIButton( mainButtonsX, GetResolutionHeight() - 1.0 * distBetweenButtons, 128, 32, buttonImage, loc[ "exitButton" ].c_str(), gui->font, Vector3( 0, 255, 0 ), 1 );
    }
    else
    {
      int ntfX = 400; 
      int ntfY = GetResolutionHeight() / 2 - 64;
      int ntfW = GetResolutionWidth() - 800;
      DrawGUIRect( ntfX, ntfY, ntfW, 32, TextureHandle::Empty(), Vector3( 50, 0, 0 ), 200 );
      DrawGUIText( loc["autosaveNotify"].c_str(), ntfX, ntfY, ntfW , 32, textFont, Vector3( 200, 0, 0 ), 1 );
      GUIState ok = DrawGUIButton( GetResolutionWidth() / 2 - 64, GetResolutionHeight() / 2 - 16, 128, 32, buttonImage, loc[ "okButton" ].c_str(), gui->font, Vector3( 0, 255, 0 ), 1 );
      if( ok.mouseLeftClicked )
        autosaveNotify = false;
    }

    if( page == Page::Authors )
    {
      int w = 500;
      int h = 400;
      int x = ( GetResolutionWidth() - w ) / 2;
      int y = ( GetResolutionHeight() - h ) / 2;
      DrawGUIRect( x, y, w, h, TextureHandle::Empty() );
      DrawGUIText( loc["authorsText"].c_str(), x, y, w, h, gui->font, Vector3( 255, 127, 127 ), 1 );
    }

    if( page == Page::Options ) // options window
    {
      GUIState optionsCommon = DrawGUIButton( buttonsXOffset + 200, GetResolutionHeight() - 2.5 * distBetweenButtons, 128, 32, buttonImage, loc[ "commonSettings" ].c_str(), gui->font, Vector3( 0, 255, 0 ), 1 );
      GUIState optionsKeys = DrawGUIButton( buttonsXOffset + 200, GetResolutionHeight() - 2.0 * distBetweenButtons, 128, 32, buttonImage, loc[ "controls" ].c_str(), gui->font, Vector3( 0, 255, 0 ), 1 );
      GUIState optionsGraphics = DrawGUIButton( buttonsXOffset + 200, GetResolutionHeight() - 1.5 * distBetweenButtons, 128, 32, buttonImage, loc[ "graphics" ].c_str(), gui->font, Vector3( 0, 255, 0 ), 1 );

      if( optionsCommon.mouseLeftClicked )
        SetPage( Page::OptionsCommon );

      if( optionsKeys.mouseLeftClicked )
        SetPage( Page::OptionsKeys );

      if( optionsGraphics.mouseLeftClicked )
        SetPage( Page::OptionsGraphics );
    }

    if( page == Page::OptionsCommon )
    {
      float x = buttonsXOffset + 200;
      float y = GetResolutionHeight() - 2.5 * distBetweenButtons;

      float buttonWidth = 80;
      float buttonHeight = 32;
      float textX = x + buttonWidth * 1.5f;

      mouseSensivity->Draw( x, y - 3.0f * buttonHeight, buttonImage, loc[ "mouseSens" ].c_str() );
      mouseSens = mouseSensivity->value / 100.0f;

      buttonWidth = 32;

      masterVolume->Draw( x, y, buttonImage, loc["masterVolume"].c_str() );
      SetMasterVolume( masterVolume->value / 100.0f );

      y = y + 1.5f * buttonHeight;

      musicVolume->Draw( x, y, buttonImage, loc["musicVolume"].c_str() );
      SetVolume( music, musicVolume->value / 100.0f );
    }

    if( page == Page::OptionsGraphics )
    {
      float x = buttonsXOffset + 200;
      float y = GetResolutionHeight() - 2.5 * distBetweenButtons;

      // FXAA Options
      fxaaButton->Draw( x, y, buttonImage, loc["fxaa"].c_str() );
      if( fxaaButton->on )
        EnableFXAA();
      else
        DisableFXAA();

      y = GetResolutionHeight() - 2.0 * distBetweenButtons;

      fpsButton->Draw( x, y, buttonImage, loc["showFPS"].c_str() );

      g_showFPS = fpsButton->on;

      y = GetResolutionHeight() - 2.0 * distBetweenButtons;

      textureFiltering->Draw( x, y, buttonImage, loc["filtering"].c_str() );

      if( textureFiltering->GetCurrentValue() == 0 ) 
        SetTextureFiltering( TextureFilter::Anisotropic, GetMaxAnisotropy() );
      else
        SetTextureFiltering( TextureFilter::Linear, 0 );
    }

    if( page == Page::OptionsKeys )
    {
      float x = buttonsXOffset + 200;
      float y = GetResolutionHeight() - 2.5 * distBetweenButtons;

      // First column
      wkMoveForward->Draw( x, y, smallButtonImage, loc["forward"].c_str() );

      y += 32 * 1.1f;
      wkMoveBackward->Draw( x, y, smallButtonImage, loc["backward"].c_str() );

      y += 32 * 1.1f;
      wkStrafeLeft->Draw( x, y, smallButtonImage, loc["strafeLeft"].c_str() );

      y += 32 * 1.1f;
      wkStrafeRight->Draw( x, y, smallButtonImage, loc["strafeRight"].c_str() );

      // Second column
      x += 150;
      y = GetResolutionHeight() - 2.5 * distBetweenButtons;

      wkJump->Draw( x, y, smallButtonImage, loc["jump"].c_str() );

      y += 32 * 1.1f;
      wkFlashLight->Draw( x, y, smallButtonImage, loc["flashLight"].c_str() );

      y += 32 * 1.1f;
      wkRun->Draw( x, y, smallButtonImage, loc["run"].c_str() );

      y += 32 * 1.1f;
      wkInventory->Draw( x, y, smallButtonImage, loc["inventory"].c_str() );

      // Third column
      x += 150;
      y = GetResolutionHeight() - 2.5 * distBetweenButtons;

      wkUse->Draw( x, y, smallButtonImage, loc["use"].c_str() );

      y += 32 * 1.1f;
      wkQuickSave->Draw( x, y, smallButtonImage, loc["quickSave"].c_str() );

      y += 32 * 1.1f;
      wkQuickLoad->Draw( x, y, smallButtonImage, loc["quickLoad"].c_str() );
    }

    if( !startPressed && !returnToGameByEsc )
    {
      PlaySoundSource( music );

      if( start.mouseLeftClicked )
      {
        startPressed = true;

        SetPage( Page::Main );
      }

      if( exit.mouseLeftClicked )
      {
        exitPressed = true;
        
        if( player && currentLevel )
        {
          if( !player->dead )
          {
            WriteProgressConfig();
            SaveWriter save( "lastGame.save" );
            save.SaveWorldState();
          }
        }

        SetPage( Page::Main );
      }

      if( continueGame.mouseLeftClicked )
      {
        continuePressed = true;

        SetPage( Page::Main );

        if( !currentLevel )
        {
          // load last played game
          map< string, string > values;
          ParseFile( "progress.cfg", values );

          continueLevelName = (LevelName)( atoi( values[ "lastLevel" ].c_str()) );
        }
      }

      if( options.mouseLeftClicked )
        SetPage( Page::Options );

      if( authors.mouseLeftClicked )
        SetPage( Page::Authors );
    }  
  }
  else
  {
    if( mi::KeyHit( mi::Esc ) )  
      menu->Show();
  }
}

void Menu::SetPage( Page page )
{
  if( this->page == page )
    this->page = Page::Main;
  else
    this->page = page;
}

void Menu::LoadTextures()
{
  buttonImage = GetTexture( "data/gui/button.png" );
  smallButtonImage = GetTexture( "data/gui/smallbutton.png" );
}

void Menu::CreateCamera()
{
  camera = new GameCamera;
  cameraInitialPosition = GetPosition( FindByName( "Camera") );
  cameraAnimationNewOffset = Vector3( 0.5, 0.5, 0.5 );
}

void Menu::LoadSounds()
{
  pickSound = CreateSound2D( "data/sounds/menupick.ogg" );
  music = CreateMusic( "data/music/menu.ogg" );
}

void Menu::CreateWaitKeys()
{
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

void Menu::CreateRadioButtons()
{
  fxaaButton = new RadioButton;  
  fpsButton = new RadioButton;
}

void Menu::CreateSliders()
{
  masterVolume = new NumericSlider( 0, 100, 2.5f );
  musicVolume = new NumericSlider( 0, 100, 2.5f );
  mouseSensivity = new NumericSlider( 0, 100, 2.5f );
}

void Menu::LoadConfig()
{
  // Load config
  map< string, string > config;
  ParseFile( "config.cfg", config );

  if( config.size() )
  {
    masterVolume->value = atof( config[ "masterVolume" ].c_str() );
    musicVolume->value = atof( config[ "musicVolume" ].c_str() );
    mouseSensivity->value = atof( config[ "mouseSens" ].c_str() );
    fxaaButton->on = atoi( config[ "masterVolume" ].c_str() );

    wkMoveForward->SetSelected( atoi( config[ "keyMoveForward" ].c_str() ));
    wkMoveBackward->SetSelected( atoi( config[ "keyMoveBackward" ].c_str() ));
    wkStrafeLeft->SetSelected( atoi( config[ "keyStrafeLeft" ].c_str() ));
    wkStrafeRight->SetSelected( atoi( config[ "keyStrafeRight" ].c_str() ));
    wkJump->SetSelected( atoi( config[ "keyJump" ].c_str() ));
    wkRun->SetSelected( atoi( config[ "keyRun" ].c_str() ));
    wkFlashLight->SetSelected( atoi( config[ "keyFlashLight" ].c_str() ));
    wkInventory->SetSelected( atoi( config[ "keyInventory" ].c_str() ));
    wkUse->SetSelected( atoi( config[ "keyUse" ].c_str() ));
    wkQuickSave->SetSelected( g_keyQuickSave = atoi( config[ "keyQuickSave" ].c_str() ));
    wkQuickLoad->SetSelected( g_keyQuickLoad = atoi( config[ "keyQuickLoad" ].c_str() ));
  }
}

void Menu::SetPlayerControls()
{
  if( player )
  {
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

void Menu::WriteConfig()
{
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

  config.close();
}

void Menu::WriteString( ofstream & stream, string name, string value )
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
  textureFiltering = new List();

  textureFiltering->AddValue( loc["anisotropic"].c_str() );
  textureFiltering->AddValue( loc["trilinear"].c_str() );
}

Menu::~Menu()
{
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
}

void Menu::WriteProgressConfig()
{
  ofstream out( "progress.cfg" );

  WriteInteger( out, "lastLevel", (int)Level::currentLevelName );
  if( player )
    WriteFloat( out, "playerLife", player->life );

  out.close();
}
