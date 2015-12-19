#pragma once

#include "Game.h"
#include "Inventory.h"
#include "GUIProperties.h"
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
#include "SoundMaterial.h"
#include "Syringe.h"

class Player : public Actor {
public:
    void LoadSounds();
    void CreateCamera();
    void LoadGUIElements();
    void UpdateJumping();
    void UpdateCameraShake();
    void UpdatePicking();
    void UpdateItemsHandling();
    void UpdateCursor();
    void DrawSheetInHands();
    void CloseCurrentSheet();
    void UpdateFright();
    Parser mLocalization;

    GameCamera * mpCamera;

    shared_ptr<ruSceneNode> mHead;
    shared_ptr<ruSceneNode> mPickPoint;
    shared_ptr<ruSceneNode> mItemPoint;
    shared_ptr<ruSceneNode> mNodeInHands;
    shared_ptr<ruSceneNode> mNearestPickedNode;
    shared_ptr<ruSceneNode> mPickedNode;
	shared_ptr<ruPointLight>mFakeLight;

    shared_ptr<ruTexture> mStatusBar;

	ruRect * mGUICursorPickUp;
	ruRect * mGUICursorPut;
	ruRect * mGUICrosshair;

    SmoothFloat mPitch;
    SmoothFloat mYaw;
   // SmoothFloat mStaminaAlpha;
    //SmoothFloat mHealthAlpha;
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
	float mLastHealth;
	

    ruVector3 mSpeed;
    ruVector3 mSpeedTo;
    ruVector3 mGravity;
    ruVector3 mJumpTo;
    ruVector3 mCameraOffset;
    ruVector3 mCameraShakeOffset;
    ruVector3 mFrameColor;
	ruVector3 mPickCenterOffset;

	float mCameraTrembleTime;
	ruVector3 mCameraTrembleOffset;

    Way * mpCurrentWay;

    ruSound mLighterCloseSound;
    ruSound mLighterOpenSound;
    ruSound mItemPickupSound;
    ruSound mHeartBeatSound;
    ruSound mBreathSound;

	float mWhispersSoundVolume;
	float mWhispersSoundVolumeTo;
	ruSound mWhispersSound;

	vector< ruSound > mPainSound;

	
	bool mInLight;
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
	bool mLandedSoundEmitted;

    Inventory mInventory;

    ruInput::Key mKeyMoveForward;
    ruInput::Key mKeyMoveBackward;
    ruInput::Key mKeyStrafeLeft;
    ruInput::Key mKeyStrafeRight;
    ruInput::Key mKeyJump;
    ruInput::Key mKeyFlashLight;
    ruInput::Key mKeyRun;
    ruInput::Key mKeyInventory;
    ruInput::Key mKeyUse;
    ruInput::Key mKeyStealth;
    ruInput::Key mKeyLookLeft;
    ruInput::Key mKeyLookRight;

    Goal mGoal; 

    Tip mTip;

    Sheet * mpSheetInHands;

    ruText * mGUIActionText;

    static const int mGUISegmentCount = 20;
    ruRect * mGUIHealthBarSegment[mGUISegmentCount];
    ruRect * mGUIStaminaBarSegment[mGUISegmentCount];
    ruRect * mGUIBackground;
	ruRect * mGUIStealthSign;
	ruText * mGUIYouDied;
	ruFont * mGUIYouDiedFont;
	ruRect * mGUIDamageBackground;
	int mDamageBackgroundAlpha;

	ruSound mDeadSound;
	ruVector3 mAirPosition;

	ruTimer * mAutoSaveTimer;
	vector<SoundMaterial*> mSoundMaterialList;

	vector<UsableObject*> mUsableObjectList;
	UsableObject * mCurrentUsableObject;

	void EmitStepSound();

public:
    explicit Player();
    virtual ~Player();
	void Step( ruVector3 direction, float speed );
    void FreeHands();
	void LockFlashlight( bool state );
	void SetHealth( float health );
	float GetHealth();
	void TurnOffFakeLight( );
	void TrembleCamera( float time );
    bool IsCanJump( );
    bool UseStamina( float st );
	bool AddUsableObject( UsableObject * usObj );
    virtual void Damage( float dmg, bool headJitter = true );
    void AddItem( Item::Type type );
    void UpdateInventory();
    void Update( );
    void UpdateMouseLook();
    void UpdateMoving();
	void UpdateUsableObjects();
    void DrawStatusBar();
    void SetObjective( string text );
    void CompleteObjective();
    bool IsUseButtonHit();
    bool IsObjectHasNormalMass( shared_ptr<ruSceneNode> node );
	bool IsDead();
	void Resurrect();
    void DoFright();
    void ComputeStealth();
	virtual void SetPosition( ruVector3 position );
    Inventory * GetInventory();
    Flashlight * GetFlashLight();
	Weapon * GetWeapon();
    Parser * GetLocalization();
    void SetTip( const string & text );
    virtual void Serialize( SaveFile & out ) final;
    virtual void Deserialize( SaveFile & in ) final;
    void SetActionText( const string & text );
    void SetHUDVisible( bool state );
	virtual void ManageEnvironmentDamaging() final;
	void DumpUsableObjects( vector<UsableObject*> & otherPlace );
};