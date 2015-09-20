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
#include "SaveFile.h"
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

    ruSceneNode mHead;
    ruSceneNode mPickPoint;
    ruSceneNode mItemPoint;
    ruSceneNode mNodeInHands;
    ruSceneNode mNearestPickedNode;
    ruSceneNode mPickedNode;

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
	float mStepLength;

    ruVector3 mSpeed;
    ruVector3 mSpeedTo;
    ruVector3 mGravity;
    ruVector3 mJumpTo;
    ruVector3 mCameraOffset;
    ruVector3 mCameraShakeOffset;
    ruVector3 mFrameColor;

	float mCameraTrembleTime;
	ruVector3 mCameraTrembleOffset;

    Way * mpCurrentWay;

    ruSound mLighterCloseSound;
    ruSound mLighterOpenSound;
    ruSound mItemPickupSound;
    ruSound mHeartBeatSound;
    ruSound mBreathSound;

    vector< ruSound > mFootstepList;

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
	bool mInAir;
	bool mFlashlightLocked;


    Inventory mInventory;

	void SwitchToWeapon() {
		mCurrentWeapon->SetVisible( true );
	};

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
	ruTextHandle mGUIYouDied;
	ruFontHandle mGUIYouDiedFont;
	ruRectHandle mGUIDamageBackground;
	int mDamageBackgroundAlpha;

	ruSound mDeadSound;
	ruVector3 mAirPosition;

	ruTimerHandle mAutoSaveTimer;
public:
    explicit Player();
    virtual ~Player();
    void FreeHands();
	void LockFlashlight( bool state ) {
		mFlashlightLocked = state;
	}
	void SetHealth( float health ) {
		mHealth = health;
	}
	float GetHealth() {
		return mHealth;
	}
	void TrembleCamera( float time );
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
    bool IsObjectHasNormalMass( ruSceneNode node );
	bool IsDead();
	void Resurrect();
    void DoFright();
    void ComputeStealth();
	void RepairInventory();
	virtual void SetPosition( ruVector3 position );
    Inventory * GetInventory();
    Flashlight * GetFlashLight();
    Parser * GetLocalization();
    void SetTip( const string & text );
    virtual void Serialize( SaveFile & out ) final;
    virtual void Deserialize( SaveFile & in ) final;
    void SetActionText( const string & text );
    void SetHUDVisible( bool state );
	virtual void ManageEnvironmentDamaging() final;
};