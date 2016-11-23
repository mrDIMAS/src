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
public:
	Config mLocalization;
	// animations
	Animation mWalkAnimation;
	Animation mCrouchAnimation;
	Animation mCrouchWalkAnimation;
	Animation mRunAnimation;
	Animation mStayAnimation;
	Animation mGrabAnimation;
	Animation mPushDoorAnimation;
	Animation mPickUpAnimation;
	Animation mJumpAnimation;
	Animation mLookRightAnimation;
	Animation mLookLeftAnimation;
	Animation mLadderCrawlInAnimation;
	Animation mLadderCrawlAnimation;
	Animation mSwitchFlashlightAnimation;
	// items additional animations
	float mLadderClimbDelta;
	int mHealthRegenTimer;
	shared_ptr<ISceneNode> mBodyModelRoot;
	shared_ptr<ISceneNode> mBodyModel;
	shared_ptr<ISceneNode> mNeck;
	shared_ptr<ISceneNode> mCameraPivot;
	vector<shared_ptr<ISceneNode>> mLeftArm;
	vector<shared_ptr<ISceneNode>> mRightArm;
	vector<shared_ptr<ISceneNode>> mLeftLeg;
	vector<shared_ptr<ISceneNode>> mRightLeg;
	vector<shared_ptr<ISceneNode>> mSpine;
	unique_ptr<GameCamera> mpCamera;
	shared_ptr<ISceneNode> mPickPoint;
	shared_ptr<ISceneNode> mItemPoint;
	shared_ptr<ISceneNode> mNodeInHands;
	shared_ptr<ISceneNode> mNearestPickedNode;
	shared_ptr<ISceneNode> mPickedNode;
	shared_ptr<IPointLight> mFakeLight;
	shared_ptr<ISpotLight> mFlashlight;
	shared_ptr<ISound> mFlashlightSwitchSound;
	SmoothFloat mPitch;
	SmoothFloat mYaw;
	SmoothFloat mFov;
	bool mFlashlightEnabled;
	float mNoiseFactor;
	float mStamina;
	float mMaxStamina;
	float mRunSpeedMult;
	float mStealthFactor;
	float mLastHealth;
	float mDeadRotation;
	float mDestDeadRotation;
	SmoothFloat mYawWalkOffset;
	Vector3 mSpeed;
	Vector3 mSpeedTo;
	Vector3 mGravity;
	Vector3 mJumpTo;
	Vector3 mFrameColor;
	Vector3 mPickCenterOffset;
	weak_ptr<Ladder> mLadder;
	float mWhispersSoundVolume;
	float mWhispersSoundVolumeTo;
	shared_ptr<ISound> mWhispersSound;
	vector<shared_ptr<ISound>> mPainSound;
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
	IInput::Key mKeyMoveForward;
	IInput::Key mKeyMoveBackward;
	IInput::Key mKeyStrafeLeft;
	IInput::Key mKeyStrafeRight;
	IInput::Key mKeyJump;
	IInput::Key mKeyRun;
	IInput::Key mKeyInventory;
	IInput::Key mKeyUse;
	IInput::Key mKeyStealth;
	IInput::Key mKeyLookLeft;
	IInput::Key mKeyLookRight;
	IInput::Key mKeyFlashlightHotkey;
	unique_ptr<HUD> mHUD;
	shared_ptr<ISound> mDeadSound;
	Vector3 mAirPosition;
	shared_ptr<ITimer> mAutoSaveTimer;
	vector<unique_ptr<SoundMaterial>> mSoundMaterialList;

	void UpdateJumping();
	void UpdatePicking();
	void UpdateItemsHandling();
	void SetLeftLegAnimation(Animation * anim);
	void SetRightLegAnimation(Animation * anim);
	void SetLeftArmAnimation(Animation * anim);
	void SetRightArmAnimation(Animation * anim);
	void SetSpineAnimation(Animation * anim);
	void SetLegsAnimation(Animation * anim);
	void SetArmsAnimation(Animation * anim);
	void SetBodyAnimation(Animation * anim);
	void EmitStepSound();
	void LadderEmitStepSound();
	void SwitchFlashlight() {
		mFlashlightEnabled = !mFlashlightEnabled;
		mFlashlightSwitchSound->Play();
	}

public:
	explicit Player(unique_ptr<Game> & game);
	virtual ~Player();

	void Step(Vector3 direction, float speed);
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
	bool IsUseButtonDown() {
		return mGame->GetEngine()->GetInput()->IsKeyDown(mKeyUse);
	}
	bool IsObjectHasNormalMass(shared_ptr<ISceneNode> node);
	bool IsDead();
	void ComputeStealth();
	virtual void SetPosition(Vector3 position);
	unique_ptr<Inventory> & GetInventory();
	Config * GetLocalization();
	virtual void Serialize(SaveFile & out) final;
	void SetHUDVisible(bool state);
	void Interact();
	unique_ptr<HUD> & GetHUD() {
		return mHUD;
	}
	void Crouch(bool state);
};