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
#include "TextFileStream.h"
#include "Parser.h"
#include "SmoothFloat.h"
#include "Tip.h"
#include "Actor.h"

class Player : public Actor {
public:
    void LoadSounds();
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

	void CloseCurrentSheet()
	{
		ruShowNode( sheetInHands->object );
		sheetInHands = 0;
		ruPlaySound( Sheet::paperFlip );
	}

	void SetRockFootsteps();
    void SetDirtFootsteps();
    void SetMetalFootsteps();
    void UpdateFright();
    Parser localization;
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
    bool IsObjectHasNormalMass( ruNodeHandle node );
    void DoFright();
	void ComputeStealth();
    Item * flashLightItem;

    FootstepsType footstepsType;

    GameCamera * camera;

    //NodeHandle body;
	ruNodeHandle head;
    ruNodeHandle pickPoint;
    ruNodeHandle itemPoint;
    ruNodeHandle objectInHands;
    ruNodeHandle nearestPicked;
    ruNodeHandle picked;

    ruTextureHandle upCursor;
    ruTextureHandle downCursor;
    ruTextureHandle statusBar;
	ruTextureHandle stealthSign;

    SmoothFloat pitch;
    SmoothFloat yaw;
    SmoothFloat damagePitchOffset;
    SmoothFloat staminaAlpha;
    SmoothFloat healthAlpha;
    SmoothFloat breathVolume;
    SmoothFloat heartBeatVolume;
    SmoothFloat heartBeatPitch;
    SmoothFloat breathPitch;
    SmoothFloat fov;
	SmoothFloat stealthOffset;
	SmoothFloat headAngle;

    float stamina;
    float life;
    float maxLife;
    float maxStamina;
    float runSpeedMult;
    float headHeight;
    float cameraBobCoeff;
    float runBobCoeff;   
	float capsuleRadius;
    float capsuleHeight;
	float stealthFactor;

    ruVector3 speed;
    ruVector3 speedTo;
    ruVector3 gravity;
    ruVector3 jumpTo;
    ruVector3 cameraOffset;
    ruVector3 cameraBob;
    ruVector3 frameColor;
	 

    Way * currentWay;

    ruSoundHandle lighterCloseSound;
    ruSoundHandle lighterOpenSound;
    ruSoundHandle pickupSound;
    ruSoundHandle heartBeatSound;
    ruSoundHandle breathSound;

    vector< ruSoundHandle > footsteps;

    bool objectThrown;
    bool landed;
    bool dead;
    bool objectiveDone;
    bool moved;
    bool locked;
    bool smoothCamera;
	bool stealthMode;
	bool running;

    Inventory inventory;

    int placeDescTimer;
    int keyMoveForward;
    int keyMoveBackward;
    int keyStrafeLeft;
    int keyStrafeRight;
    int keyJump;
    int keyFlashLight;
    int keyRun;
    int keyInventory;
    int keyUse;
	int keyStealth;
	int keyLookLeft;
	int keyLookRight;

    string placeDesc;
    string pickedObjectDesc;

    Goal goal;

    Tip tip;

    Sheet * sheetInHands;

    Flashlight * flashlight;

    virtual void SerializeWith( TextFileStream & out ) final;
    virtual void DeserializeWith( TextFileStream & in ) final;
};