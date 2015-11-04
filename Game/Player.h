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

    ruSceneNode mHead;
    ruSceneNode mPickPoint;
    ruSceneNode mItemPoint;
    ruSceneNode mNodeInHands;
    ruSceneNode mNearestPickedNode;
    ruSceneNode mPickedNode;
	ruSceneNode mFakeLight;

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
	float mLastHealth;

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

	vector< ruSound > mPainSound;

	

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
	vector<SoundMaterial*> mSoundMaterialList;

	vector<UsableObject*> mUsableObjectList;
	UsableObject * mCurrentUsableObject;
public:
    explicit Player();
    virtual ~Player();
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
    void AddItem( Item * itm );
    void UpdateInventory();
    void Update( );
    void UpdateMouseLook();
    void UpdateMoving();
	void UpdateUsableObjects();
    void DrawStatusBar();
    void SetObjective( string text );
    void CompleteObjective();
    bool IsUseButtonHit();
    bool IsObjectHasNormalMass( ruSceneNode node );
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
	void DumpUsableObjects( vector<UsableObject*> & otherPlace ) {
		// this method is useful to transfer usable objects between levels
		for( auto uo : mUsableObjectList ) {
			uo->GetModel().Detach();
		}
		otherPlace = mUsableObjectList;
		mUsableObjectList.clear();
	}
};