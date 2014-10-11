#pragma once

#include "Game.h"
#include "Inventory.h"
#include "GUI.h"
#include "LightAnimator.h"
#include "Sheet.h"
#include "GameCamera.h"
#include "Flashlight.h"
#include "Goal.h"
#include "Way.h"
#include "Locale.h"

class Player : public Locale
{
private:
  void LoadSounds();
  void CreateBody();
  void CreateCamera();
  void CreateFlashLight();
  void LoadGUIElements();
  void UpdateJumping();
  void UpdateCameraBob();
  void DrawGUIElements();
  void UpdateFlashLight();
  void UpdatePicking();
  void UpdateItemsHandling();
  void UpdateEnvironmentDamaging();
  void DescribePickedObject();
  void DrawSheetInHands();
  void SetRockFootsteps();
  void SetDirtFootsteps();
  void SetMetalFootsteps();
public:
  explicit Player();
  virtual ~Player();
  void DrawTip( string text );
  void FreeHands();
  bool CanJump( );
  bool UseStamina( float st );
  void Damage( float dmg );
  void AddItem( Item * itm );
  void UpdateInventory();
  void Update( );
  void UpdateMouseLook();
  void UpdateMoving();
  void DrawStatusBar();
  void SetObjective( string text );
  void CompleteObjective();
  int GotItemAnyOfType( int type );
  void SetPlaceDescription( string desc );
  void SetFootsteps( FootstepsType ft );
  void ChargeFlashLight( Item * fuel );
  bool IsUseButtonHit();
  bool IsObjectHasNormalMass( NodeHandle node );
  vector< SoundHandle > footsteps;

  bool locked;
  bool smoothCamera;

  SoundHandle lighterCloseSound;
  SoundHandle lighterOpenSound;
  float runBobCoeff;
 
  float damagePitchOffset;
  float damagePitchOffsetTo;

  Item * flashLightItem;

  FootstepsType footstepsType;
  NodeHandle body;
  GameCamera * camera;
  NodeHandle pickPoint;
  NodeHandle itemPoint;
  NodeHandle objectInHands;
  float pitch, yaw;
  float pitchTo, yawTo;
  Vector3 speed;
  Vector3 speedTo;
  Vector3 gravity;
  Vector3 jumpTo;
  TextureHandle upCursor;
  TextureHandle downCursor;
  Way * currentWay;
  bool landed;
  NodeHandle picked;
  float stamina;
  float life;
  NodeHandle nearestPicked;
  float maxLife;
  float maxStamina;
  float runSpeedMult;
  float fov;
  float runFOV;
  float normalFOV;
  
  SoundHandle pickupSound;
  float fovTo;
  float cameraBobCoeff;
  Vector3 cameraOffset;
  Vector3 cameraBob;
  float headHeight;
  bool objectThrown;

  bool dead;
  Inventory inventory;
  Vector3 frameColor;
  string placeDesc;
  bool moved;
  int placeDescTimer;
  TextureHandle statusBar;
  float staminaAlpha;
  float healthAlpha;
  float staminaAlphaTo;
  float healthAlphaTo;

  bool objectiveDone;

  string pickedObjectDesc;

  int breathParticleSystem;
  Goal goal;

  Sheet * sheetInHands;

  // Control keys
  int keyMoveForward;
  int keyMoveBackward;
  int keyStrafeLeft;
  int keyStrafeRight;
  int keyJump;
  int keyFlashLight;
  int keyRun;
  int keyInventory;
  int keyUse;

  Flashlight * flashlight;
};