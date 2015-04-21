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
#include "Weapon.h"

class Player : public Actor {
public:
    void LoadSounds();
    void CreateCamera();
    void CreateFlashLight();
    void LoadGUIElements();
    void UpdateJumping();
    void UpdateCameraShake();
    void UpdateFlashLight();
    void UpdatePicking();
    void UpdateItemsHandling();
    void UpdateCursor();
    void DrawSheetInHands();
    void CloseCurrentSheet();
    void SetRockFootsteps();
    void SetDirtFootsteps();
    void SetMetalFootsteps();
    void UpdateFright();
    Parser mLocalization;
    Item * mpFlashLightItem;

    FootstepsType mFootstepsType;

    GameCamera * mpCamera;

    ruNodeHandle mHead;
    ruNodeHandle mPickPoint;
    ruNodeHandle mItemPoint;
    ruNodeHandle mNodeInHands;
    ruNodeHandle mNearestPickedNode;
    ruNodeHandle mPickedNode;

    ruTextureHandle mStatusBar;

	ruRectHandle mGUICursorPickUp;
	ruRectHandle mGUICursorPut;
	ruRectHandle mGUICrosshair;

    SmoothFloat mPitch;
    SmoothFloat mYaw;
    SmoothFloat mStaminaAlpha;
    SmoothFloat mHealthAlpha;
    SmoothFloat mBreathVolume;
    SmoothFloat mHeartBeatVolume;
    SmoothFloat mHeartBeatPitch;
    SmoothFloat mBreathPitch;
    SmoothFloat mFov;
    SmoothFloat mHeadAngle;

    float mStamina;

    float mMaxStamina;
    float mRunSpeedMult;
    float mHeadHeight;
    float mCameraBobCoeff;
    float mRunCameraShakeCoeff;
    float mStealthFactor;

    ruVector3 mSpeed;
    ruVector3 mSpeedTo;
    ruVector3 mGravity;
    ruVector3 mJumpTo;
    ruVector3 mCameraOffset;
    ruVector3 mCameraShakeOffset;
    ruVector3 mFrameColor;

    Way * mpCurrentWay;

    ruSoundHandle mLighterCloseSound;
    ruSoundHandle mLighterOpenSound;
    ruSoundHandle mItemPickupSound;
    ruSoundHandle mHeartBeatSound;
    ruSoundHandle mBreathSound;

    vector< ruSoundHandle > mFootstepList;

	Weapon * mCurrentWeapon;
	vector<Weapon*> mWeaponList;

    bool mObjectThrown;
    bool mLanded;
    bool mDead;
    bool mObjectiveDone;
    bool mMoved;
    bool mSmoothCamera;
    bool mStealthMode;
    bool mRunning;

    Inventory mInventory;

    int mKeyMoveForward;
    int mKeyMoveBackward;
    int mKeyStrafeLeft;
    int mKeyStrafeRight;
    int mKeyJump;
    int mKeyFlashLight;
    int mKeyRun;
    int mKeyInventory;
    int mKeyUse;
    int mKeyStealth;
    int mKeyLookLeft;
    int mKeyLookRight;

    Goal mGoal; 

    Tip mTip;

    Sheet * mpSheetInHands;

    Flashlight * mpFlashlight;

    ruTextHandle mGUIActionText;

    static const int mGUISegmentCount = 20;
    ruRectHandle mGUIHealthBarSegment[mGUISegmentCount];
    ruRectHandle mGUIStaminaBarSegment[mGUISegmentCount];
    ruRectHandle mGUIBackground;
	ruRectHandle mGUIStealthSign;
public:
    explicit Player();
    virtual ~Player();
    void FreeHands();
    bool IsCanJump( );
    bool UseStamina( float st );
    virtual void Damage( float dmg );
    void AddItem( Item * itm );
    void UpdateInventory();
    void Update( );
    void UpdateMouseLook();
    void UpdateMoving();
	Weapon * AddWeapon( Weapon::Type type );
	void UpdateWeapons();
    void DrawStatusBar();
    void SetObjective( string text );
    void CompleteObjective();
    void SetFootsteps( FootstepsType ft );
    void ChargeFlashLight( );
    bool IsUseButtonHit();
    bool IsObjectHasNormalMass( ruNodeHandle node );
	bool IsDead();
	void Resurrect();
    void DoFright();
    void ComputeStealth();
	void RepairInventory();
    Inventory * GetInventory();
    Flashlight * GetFlashLight();
    Parser * GetLocalization();
    void SetTip( const string & text );
    virtual void SerializeWith( TextFileStream & out ) final;
    virtual void DeserializeWith( TextFileStream & in ) final;
    void SetActionText( const string & text );
    void SetHUDVisible( bool state );
	virtual void ManageEnvironmentDamaging() final;
};