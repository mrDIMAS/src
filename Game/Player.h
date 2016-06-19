#pragma once

#include "Game.h"
#include "Inventory.h"
#include "GUIProperties.h"
#include "LightAnimator.h"
#include "Sheet.h"
#include "GameCamera.h"
#include "Flashlight.h"
#include "Goal.h"
#include "Ladder.h"
#include "SaveFile.h"
#include "Parser.h"
#include "SmoothFloat.h"
#include "Tip.h"
#include "Actor.h"
#include "Weapon.h"
#include "SoundMaterial.h"
#include "Syringe.h"

class Player : public Actor {
private:
	friend class Level;
	explicit Player();
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

    unique_ptr<GameCamera> mpCamera;

    shared_ptr<ruSceneNode> mHead;
    shared_ptr<ruSceneNode> mPickPoint;
    shared_ptr<ruSceneNode> mItemPoint;
    shared_ptr<ruSceneNode> mNodeInHands;
    shared_ptr<ruSceneNode> mNearestPickedNode;
    shared_ptr<ruSceneNode> mPickedNode;
	shared_ptr<ruPointLight>mFakeLight;

    shared_ptr<ruTexture> mStatusBar;

    SmoothFloat mPitch;
    SmoothFloat mYaw;
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

    weak_ptr<Ladder> mLadder;

    shared_ptr<ruSound> mLighterCloseSound;
    shared_ptr<ruSound> mLighterOpenSound;
    shared_ptr<ruSound> mItemPickupSound;
    shared_ptr<ruSound> mHeartBeatSound;
    shared_ptr<ruSound> mBreathSound;

	float mWhispersSoundVolume;
	float mWhispersSoundVolumeTo;
	shared_ptr<ruSound> mWhispersSound;

	vector<shared_ptr<ruSound>> mPainSound;

	
	bool mInLight;
    bool mObjectThrown;
    bool mLanded;
    bool mDead;
    bool mObjectiveDone;
    bool mMoved;
    bool mStealthMode;
    bool mRunning;
	bool mInAir;
	bool mFlashlightLocked;
	bool mLandedSoundEmitted;

    unique_ptr<Inventory> mInventory;

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

    unique_ptr<Goal> mGoal; 
	unique_ptr<Tip> mTip;

    weak_ptr<Sheet> mSheetInHands;

	shared_ptr<ruGUIScene> mGUIScene;

    shared_ptr<ruText> mGUIActionText;

	shared_ptr<ruRect> mGUICursorPickUp;
	shared_ptr<ruRect> mGUICursorPut;
	shared_ptr<ruRect> mGUICrosshair;

    static const int mGUISegmentCount = 20;
    shared_ptr<ruRect> mGUIHealthBarSegment[mGUISegmentCount];
    shared_ptr<ruRect> mGUIStaminaBarSegment[mGUISegmentCount];
    shared_ptr<ruRect> mGUIBackground;
	shared_ptr<ruRect> mGUIStealthSign;
	shared_ptr<ruText> mGUIYouDied;
	shared_ptr<ruFont> mGUIYouDiedFont;
	shared_ptr<ruRect> mGUIDamageBackground;
	int mDamageBackgroundAlpha;

	shared_ptr<ruSound> mDeadSound;
	ruVector3 mAirPosition;

	shared_ptr<ruTimer> mAutoSaveTimer;
	vector<unique_ptr<SoundMaterial>> mSoundMaterialList;

	vector<UsableObject*> mUsableObjectList;
	UsableObject * mCurrentUsableObject;

	void EmitStepSound();

public:
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
	unique_ptr<Inventory> & GetInventory();
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
	void Interact();
};