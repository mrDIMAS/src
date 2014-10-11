#ifndef _MENU_
#define _MENU_

#include "GameCamera.h"
#include "GUI.h"
#include "NumericSlider.h"
#include "RadioButton.h"
#include "WaitKeyButton.h"
#include "Player.h"
#include <Windows.h>
#include "List.h"
#include "Locale.h"

class Menu : public Locale
{
public:
  enum class Page
  {
      Main,
      Options,
      Authors,
      OptionsKeys,
      OptionsCommon,
      OptionsGraphics,
  };

  explicit Menu( );
  virtual ~Menu();

  void WriteFloat( ofstream & stream, string name, float value );
  void WriteInteger( ofstream & stream, string name, int value );
  void WriteString( ofstream & stream, string name, string value );
  void WriteProgressConfig();
  void WriteConfig();
  void SetPlayerControls();
  void LoadConfig();
  void CreateSliders();
  void CreateRadioButtons();
  void CreateWaitKeys();
  void CreateLists();
  void LoadSounds();
  void CreateCamera();
  void LoadTextures();  
  void Show();
  void Hide( );
  void Update( );
  void SetPage( Page page );

  FontHandle textFont;
  FontHandle textBackgroundFont;
  NodeHandle scene;
  bool visible;
  TextureHandle buttonImage;
  TextureHandle smallButtonImage;
  int distBetweenButtons;

  float fadeSpeed;
  bool canContinueGameFromLast;
  bool startPressed;
  bool exitPressed;
  bool continuePressed;
  bool returnToGameByEsc;
  bool autosaveNotify;
  float buttonsXOffset;
  LevelName continueLevelName;
  SoundHandle pickSound;
  Page page;
  SoundHandle music;

  NumericSlider * masterVolume;
  NumericSlider * musicVolume;
  NumericSlider * mouseSensivity;
  RadioButton * fxaaButton;
  RadioButton * fpsButton;
  List * textureFiltering;

  // Keys
  WaitKeyButton * wkMoveForward;
  WaitKeyButton * wkMoveBackward;
  WaitKeyButton * wkStrafeLeft;
  WaitKeyButton * wkStrafeRight;
  WaitKeyButton * wkJump;
  WaitKeyButton * wkFlashLight;
  WaitKeyButton * wkRun;
  WaitKeyButton * wkInventory;
  WaitKeyButton * wkUse;

  bool exitingGame;

  Vector3 cameraInitialPosition;
  Vector3 cameraAnimationOffset;
  Vector3 cameraAnimationNewOffset;

  GameCamera * camera;
};


#endif