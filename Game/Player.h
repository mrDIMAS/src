#pragma once

#include "Game.h"
#include "Inventory.h"
#include "GUIProperties.h"
#include "LightAnimator.h"
#include "GameCamera.h"
#include "Ladder.h"
#include "SaveFile.h"
#include "Parser.h"
#include "SmoothFloat.h"
#include "Actor.h"
#include "SoundMaterial.h"
#include "HUD.h"

class Player : public Actor {
private:
	friend class Level;
	explicit Player();
public:
	Parser mLocalization;	
	// animations
	ruAnimation mWalkAnimation;
	ruAnimation mCrouchAnimation;
	ruAnimation mCrouchWalkAnimation;
	ruAnimation mRunAnimation;
	ruAnimation mStayAnimation;
	ruAnimation mGrabAnimation;
	ruAnimation mPushDoorAnimation;
	ruAnimation mPickUpAnimation;
	ruAnimation mJumpAnimation;
	ruAnimation mLookRightAnimation;
	ruAnimation mLookLeftAnimation;
	ruAnimation mLadderCrawlInAnimation;
	ruAnimation mLadderCrawlAnimation;	
	ruAnimation mSwitchFlashlightAnimation;
	// items additional animations
	float mLadderClimbDelta;
	int mHealthRegenTimer;
	shared_ptr<ruSceneNode> mBodyModelRoot;
	shared_ptr<ruSceneNode> mBodyModel;
	shared_ptr<ruSceneNode> mNeck;
	shared_ptr<ruSceneNode> mCameraPivot;
	vector<shared_ptr<ruSceneNode>> mLeftArm;
	vector<shared_ptr<ruSceneNode>> mRightArm;
	vector<shared_ptr<ruSceneNode>> mLeftLeg;
	vector<shared_ptr<ruSceneNode>> mRightLeg;
	vector<shared_ptr<ruSceneNode>> mSpine;
	unique_ptr<GameCamera> mpCamera;
	shared_ptr<ruSceneNode> mPickPoint;
	shared_ptr<ruSceneNode> mItemPoint;
	shared_ptr<ruSceneNode> mNodeInHands;
	shared_ptr<ruSceneNode> mNearestPickedNode;
	shared_ptr<ruSceneNode> mPickedNode;
	shared_ptr<ruPointLight> mFakeLight;
	shared_ptr<ruSpotLight> mFlashlight;
	shared_ptr<ruSound> mFlashlightSwitchSound;
	SmoothFloat mPitch;
	SmoothFloat mYaw;
	SmoothFloat mFov;
	bool mFlashlightEnabled;
	float mStamina;
	float mMaxStamina;
	float mRunSpeedMult;
	float mStealthFactor;
	float mLastHealth;
	float mDeadRotation;
	float mDestDeadRotation;
	SmoothFloat mYawWalkOffset;
	ruVector3 mSpeed;
	ruVector3 mSpeedTo;
	ruVector3 mGravity;
	ruVector3 mJumpTo;
	ruVector3 mFrameColor;
	ruVector3 mPickCenterOffset;
	weak_ptr<Ladder> mLadder;
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
	ruInput::Key mKeyRun;
	ruInput::Key mKeyInventory;
	ruInput::Key mKeyUse;
	ruInput::Key mKeyStealth;
	ruInput::Key mKeyLookLeft;
	ruInput::Key mKeyLookRight;
	ruInput::Key mKeyFlashlightHotkey;
	unique_ptr<HUD> mHUD;
	shared_ptr<ruSound> mDeadSound;
	ruVector3 mAirPosition;
	shared_ptr<ruTimer> mAutoSaveTimer;
	vector<unique_ptr<SoundMaterial>> mSoundMaterialList;

	void UpdateJumping();
	void UpdatePicking();
	void UpdateItemsHandling();
	void SetLeftLegAnimation(ruAnimation * anim);
	void SetRightLegAnimation(ruAnimation * anim);
	void SetLeftArmAnimation(ruAnimation * anim);
	void SetRightArmAnimation(ruAnimation * anim);
	void SetSpineAnimation(ruAnimation * anim);
	void SetLegsAnimation(ruAnimation * anim);
	void SetArmsAnimation(ruAnimation * anim);
	void SetBodyAnimation(ruAnimation * anim);
	void EmitStepSound();
	void LadderEmitStepSound();
	void SwitchFlashlight() {
		mFlashlightEnabled = !mFlashlightEnabled;
		mFlashlightSwitchSound->Play();
	}
public:
	virtual ~Player();
	void Step(ruVector3 direction, float speed);
	void FreeHands();
	void LockFlashlight(bool state);
	void SetHealth(float health);
	float GetHealth();
	void TurnOffFakeLight();
	bool IsCanJump();
	bool UseStamina(float st);
	virtual void Damage(float dmg, bool headJitter = true);
	void AddItem(Item::Type type);
	void UpdateInventory();
	void Update();
	void UpdateMouseLook();
	void UpdateMoving();
	void DrawStatusBar();
	bool IsUseButtonHit();
	bool IsObjectHasNormalMass(shared_ptr<ruSceneNode> node);
	bool IsDead();
	void ComputeStealth();
	virtual void SetPosition(ruVector3 position);
	unique_ptr<Inventory> & GetInventory();
	Parser * GetLocalization();
	virtual void Serialize(SaveFile & out) final;
	void SetHUDVisible(bool state);
	void Interact();
	unique_ptr<HUD> & GetHUD() {
		return mHUD;
	}
	void Crouch(bool state);
};