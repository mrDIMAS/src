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

class Player {
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
    void UpdateFright();
	void SetupBody() {
		SetAngularFactor( body, Vector3( 0, 0, 0 ));
		SetFriction( body, 0 );
		SetAnisotropicFriction( body, Vector3( 1, 1, 1 ));
		SetDamping( body, 0, 0 );
		SetMass( body, 2 );
		SetGravity( body, Vector3( 0, 0, 0 ));
		//SetLinearFactor( body, Vector3( 0, 0, 0 ));
	}
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
    bool IsObjectHasNormalMass( NodeHandle node );
    void DoFright();
    Item * flashLightItem;

    FootstepsType footstepsType;

    GameCamera * camera;

    NodeHandle body;
    NodeHandle pickPoint;
    NodeHandle itemPoint;
    NodeHandle objectInHands;
    NodeHandle nearestPicked;
    NodeHandle picked;

    TextureHandle upCursor;
    TextureHandle downCursor;
    TextureHandle statusBar;

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

    float stamina;
    float life;
    float maxLife;
    float maxStamina;
    float runSpeedMult;
    float headHeight;
    float cameraBobCoeff;
    float runBobCoeff;    
    
    Vector3 speed;
    Vector3 speedTo;
    Vector3 gravity;
    Vector3 jumpTo;
    Vector3 cameraOffset;
    Vector3 cameraBob;
    Vector3 frameColor;
    Way * currentWay;

    SoundHandle lighterCloseSound;
    SoundHandle lighterOpenSound;
    SoundHandle pickupSound;
    SoundHandle heartBeatSound;
    SoundHandle breathSound;

    vector< SoundHandle > footsteps;

    bool objectThrown;
    bool landed;
    bool dead;
    bool objectiveDone;
    bool moved;
    bool locked;
    bool smoothCamera;

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

    string placeDesc;
    string pickedObjectDesc;

    Goal goal;

    Tip tip;

    Sheet * sheetInHands;

    Flashlight * flashlight;

    virtual void SerializeWith( TextFileStream & out ) final;
    virtual void DeserializeWith( TextFileStream & in ) final;
};