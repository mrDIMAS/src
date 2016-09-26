#pragma once

#include "Game.h"
#include "Inventory.h"
#include "GUIProperties.h"
#include "LightAnimator.h"
#include "Sheet.h"
#include "GameCamera.h"
#include "Flashlight.h"
#include "Ladder.h"
#include "SaveFile.h"
#include "Parser.h"
#include "SmoothFloat.h"
#include "Actor.h"
#include "Weapon.h"
#include "SoundMaterial.h"
#include "Syringe.h"
#include "HUD.h"

class Player : public Actor {
private:
	friend class Level;
	explicit Player();
public:
    void LoadSounds();
    void CreateCamera();
    void UpdateJumping();
    void UpdateCameraShake();
    void UpdatePicking();
    void UpdateItemsHandling();
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
	shared_ptr<ruPointLight> mFakeLight;

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
	float mDeadRotation;
	float mDestDeadRotation;

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

	

    weak_ptr<Sheet> mSheetInHands;
	unique_ptr<HUD> mHUD;


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
    bool IsUseButtonHit();
    bool IsObjectHasNormalMass( shared_ptr<ruSceneNode> node );
	bool IsDead();
	void DoFright();
    void ComputeStealth();
	virtual void SetPosition( ruVector3 position );
	unique_ptr<Inventory> & GetInventory();
    Flashlight * GetFlashLight();
	Weapon * GetWeapon();
    Parser * GetLocalization();
    virtual void Serialize( SaveFile & out ) final;
    void SetHUDVisible( bool state );
	void DumpUsableObjects( vector<UsableObject*> & otherPlace );
	void Interact();
	unique_ptr<HUD> & GetHUD() {
		return mHUD;
	}
};