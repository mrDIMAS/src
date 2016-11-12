#include "Precompiled.h"

#include "Player.h"
#include "Menu.h"
#include "Door.h"
#include "utils.h"
#include "Enemy.h"
#include "SaveWriter.h"
#include "Level.h"

#include  <random>
#include  <iterator>

Player::Player() :
	Actor(0.7f, 0.2f),
	mFlashlightLocked(false),
	mLandedSoundEmitted(true),
	mMaxStamina(100.0f),
	mStamina(100.0f),
	mFov(75.0f, 75.0f, 80.0f),
	mRunSpeedMult(2.5f),
	mObjectThrown(false),
	mPitch(0.0f, -89.9f, 89.9f),
	mYaw(0.0f),
	mDead(false),
	mLanded(false),
	mStealthMode(false),
	mWhispersSoundVolume(0.0f),
	mWhispersSoundVolumeTo(0.0f),
	mFrameColor(1.0f, 1.0f, 1.0f),
	mKeyMoveForward(ruInput::Key::W),
	mKeyMoveBackward(ruInput::Key::S),
	mKeyStrafeLeft(ruInput::Key::A),
	mKeyStrafeRight(ruInput::Key::D),
	mKeyJump(ruInput::Key::Space),
	mKeyRun(ruInput::Key::LeftShift),
	mKeyInventory(ruInput::Key::Tab),
	mKeyUse(ruInput::Key::R),
	mKeyStealth(ruInput::Key::C),
	mKeyLookLeft(ruInput::Key::Q),
	mKeyLookRight(ruInput::Key::E),
	mKeyFlashlightHotkey(ruInput::Key::F),
	mInAir(false),
	mDeadRotation(0.0f),
	mDestDeadRotation(0.0f),
	mNodeInHands(nullptr),
	mYawWalkOffset(0.0, -30, 30),
	mHealthRegenTimer(0),
	mFlashlightEnabled(true),
	mNoiseFactor(0.0f)
{
	mLocalization.ParseFile(gLocalizationPath + "player.loc");
	mHUD = unique_ptr<HUD>(new HUD());
	mInventory = unique_ptr<Inventory>(new Inventory());
	gMouseSens = 0.5f;
	mLastHealth = mHealth;

	// load model and configure animations
	mBodyModel = ruSceneNode::LoadFromFile("data/models/character/character.scene");
	mBodyModel->Attach(mBody);

	// find bodyparts - names are standard for MakeHuman
	mCameraPivot = mBodyModel->FindChild("Camera");

	mNeck = mBodyModel->FindChild("neck_01");
	mNeck->SetAnimationOverride(true);

	mBodyModelRoot = mBodyModel->FindChild("Root");
	mFlashlight = dynamic_pointer_cast<ruSpotLight>(mBodyModel->FindChild("Fspot001"));
	mFlashlight->SetShadowCastEnabled(false); 
	mFlashlightSwitchSound = ruSound::Load2D("data/sounds/flashlight_switch.ogg");
	mFlashlightSwitchSound->Attach(mBody);

	// fill left arm
	{
		const char * leftArm[] = { "clavicle_l", "upperarm_l", "lowerarm_l", "hand_l", "thumb_01_l", "thumb_02_l",
			"thumb_03_l", "index_01_l", "index_02_l", "index_03_l", "middle_01_l", "middle_02_l", "middle_03_l", "ring_01_l",
			"ring_02_l", "ring_03_l", "pinky_01_l", "pinky_02_l","pinky_03_l" };
		for (auto name : leftArm) {
			mLeftArm.push_back(mBodyModel->FindChild(name));
		}
	}

	// fill right arm
	{
		const char * rightArm[] = { "clavicle_r", "upperarm_r", "lowerarm_r", "hand_r", "thumb_01_r", "thumb_02_r",
			"thumb_03_r", "index_01_r", "index_02_r", "index_03_r", "middle_01_r", "middle_02_r", "middle_03_r", "ring_01_r",
			"ring_02_r", "ring_03_r", "pinky_01_r", "pinky_02_r","pinky_03_r" };
		for (auto name : rightArm) {
			mRightArm.push_back(mBodyModel->FindChild(name));
		}
	}

	// fill left leg
	{
		const char * leftLeg[] = { "thigh_l", "calf_l", "foot_l", "ball_l" };
		for (auto name : leftLeg) {
			mLeftLeg.push_back(mBodyModel->FindChild(name));
		}
	}

	// fill right leg
	{
		const char * rightLeg[] = { "thigh_r", "calf_r", "foot_r", "ball_r" };
		for (auto name : rightLeg) {
			mRightLeg.push_back(mBodyModel->FindChild(name));
		}
	}

	// fill spine
	{
		const char * spine[] = { "spine_01", "spine_02", "spine_03" };
		for (auto name : spine) {
			mSpine.push_back(mBodyModel->FindChild(name));
		}
	}


	// configure animations
	mWalkAnimation = ruAnimation(0, 60, 0.9f, false);
	mWalkAnimation.AddFrameListener(10, [this] { EmitStepSound(); });
	mWalkAnimation.AddFrameListener(40, [this] { EmitStepSound(); });

	mRunAnimation = ruAnimation(231, 269, 0.4, false);
	mRunAnimation.AddFrameListener(241, [this] { EmitStepSound(); });
	mRunAnimation.AddFrameListener(261, [this] { EmitStepSound(); });

	mStayAnimation = ruAnimation(121, 129, 1, true);
	mStayAnimation.SetEnabled(true);

	mCrouchAnimation = ruAnimation(82, 90, 0.9, false);
	mCrouchWalkAnimation = ruAnimation(91, 120, 1, true);
	mPushDoorAnimation = ruAnimation(151, 160, 0.7, false);
	mPickUpAnimation = ruAnimation(131, 150, 0.7, false);
	mJumpAnimation = ruAnimation(160, 180, 1, false);
	mLookRightAnimation = ruAnimation(61, 70, 1, false);
	mLookLeftAnimation = ruAnimation(71, 80, 1, false);
	mLadderCrawlInAnimation = ruAnimation(180, 200, 1, false);

	mSwitchFlashlightAnimation = ruAnimation(280, 300, 0.7, false);
	mSwitchFlashlightAnimation.AddFrameListener(290, [this] { SwitchFlashlight(); });

	mLadderCrawlAnimation = ruAnimation(200, 230, 0.68, false);
	mLadderCrawlAnimation.AddFrameListener(201, [this] { LadderEmitStepSound(); });
	mLadderCrawlAnimation.AddFrameListener(210, [this] { LadderEmitStepSound(); });
	mLadderCrawlAnimation.AddFrameListener(229, [this] { LadderEmitStepSound(); });
	mLadderClimbDelta = (mBodyModelRoot->GetKeyFrame(mLadderCrawlAnimation.GetEndFrame()).mPosition - mBodyModelRoot->GetKeyFrame(mLadderCrawlAnimation.GetBeginFrame()).mPosition).Length();

	// create camera
	mpCamera = make_unique<GameCamera>(mHUD->GetScene(), mFov);
	mpCamera->mCamera->Attach(mCameraPivot);

	// pick
	mPickPoint = ruSceneNode::Create();
	mPickPoint->Attach(mpCamera->mCamera);
	mPickPoint->SetPosition(ruVector3(0, 0, 0.1));

	mItemPoint = ruSceneNode::Create();
	mItemPoint->Attach(mpCamera->mCamera);
	mItemPoint->SetPosition(ruVector3(0, 0, 1.0f));

	// fake light
	mFakeLight = ruPointLight::Create();
	mFakeLight->Attach(mpCamera->mCamera);
	mFakeLight->SetRange(2);
	mFakeLight->SetColor(ruVector3(25, 25, 25));
	mFakeLight->SetShadowCastEnabled(false);

	mBody->SetName("Player");

	// hack
	pMainMenu->SyncPlayerControls();

	mAutoSaveTimer = ruTimer::Create();

	mPainSound.push_back(ruSound::Load2D("data/sounds/player/grunt1.ogg"));
	mPainSound.push_back(ruSound::Load2D("data/sounds/player/grunt2.ogg"));
	mPainSound.push_back(ruSound::Load2D("data/sounds/player/grunt3.ogg"));
	for (auto & ps : mPainSound) {
		ps->Attach(mBody);
		ps->SetVolume(0.7);
	}

	mWhispersSound = ruSound::Load2D("data/sounds/whispers.ogg");
	mWhispersSound->SetVolume(0.085f);
	mWhispersSound->SetLoop(true);

	mSoundMaterialList.push_back(make_unique<SoundMaterial>("data/materials/stone.smat", mpCamera->mCamera));
	mSoundMaterialList.push_back(make_unique<SoundMaterial>("data/materials/metal.smat", mpCamera->mCamera));
	mSoundMaterialList.push_back(make_unique<SoundMaterial>("data/materials/wood.smat", mpCamera->mCamera));
	mSoundMaterialList.push_back(make_unique<SoundMaterial>("data/materials/gravel.smat", mpCamera->mCamera));
	mSoundMaterialList.push_back(make_unique<SoundMaterial>("data/materials/muddyrock.smat", mpCamera->mCamera));
	mSoundMaterialList.push_back(make_unique<SoundMaterial>("data/materials/rock.smat", mpCamera->mCamera));
	mSoundMaterialList.push_back(make_unique<SoundMaterial>("data/materials/grass.smat", mpCamera->mCamera));
	mSoundMaterialList.push_back(make_unique<SoundMaterial>("data/materials/soil.smat", mpCamera->mCamera));
	mSoundMaterialList.push_back(make_unique<SoundMaterial>("data/materials/chain.smat", mpCamera->mCamera));
	mSoundMaterialList.push_back(make_unique<SoundMaterial>("data/materials/mud.smat", mpCamera->mCamera));
}

Player::~Player() {

}

void Player::DrawStatusBar() {
	mHUD->SetHealth(mHealth);
	mHUD->SetStamina(mStamina);
}

bool Player::IsCanJump() {
	shared_ptr<ruSceneNode> node = ruPhysics::CastRay(mBody->GetPosition() + ruVector3(0, 0.1, 0), mBody->GetPosition() - ruVector3(0, mBodyHeight * 2, 0), 0);
	if (node) {
		if (node == mNodeInHands) {
			mNodeInHands = nullptr;
			return false;
		} else {
			return true;
		}
	}
	return false;
}

bool Player::UseStamina(float required) {
	if (mStamina - required < 0) {
		return false;
	}

	mStamina -= required;

	return true;
}


template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
	std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
	std::advance(start, dis(g));
	return start;
}

template<typename Iter>
Iter select_randomly(Iter start, Iter end) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	return select_randomly(start, end, gen);
}

void Player::Damage(float dmg, bool headJitter) {
	if (mDead) {
		return;
	}

	mHealthRegenTimer = 300; // in frames 

	Actor::Damage(dmg);
	mHUD->ShowDamage();

	if (headJitter) {
		mPitch.SetTarget(mPitch.GetTarget() + frandom(20, 40));
		mYaw.SetTarget(mYaw.GetTarget() + frandom(-40, 40));
	}
	//if (abs(mLastHealth - mHealth) > 5) {
		auto randomSound = *select_randomly(mPainSound.begin(), mPainSound.end());
		randomSound->Play();
		mLastHealth = mHealth;
	//}
	if (mHealth <= 0.0f) {
		if (!mDead) {
			mBody->Freeze();
			mDestDeadRotation = -90;
		}
		mDeadSound = ruSound::Load2D("data/sounds/dead.ogg");
		mDeadSound->Play();
		mDead = true;
		mpCamera->FadePercent(70);
		mpCamera->mCamera->SetFrameColor(ruVector3(70, 0, 0));
	}
}

void Player::AddItem(Item::Type type) {
	mInventory->AddItem(type);
}

void Player::UpdateInventory() {
	if (ruInput::IsKeyHit(mKeyInventory)) {
		mInventory->Open(!mInventory->IsOpened());
	}
	mInventory->Update();
}

void Player::UpdateMouseLook() {
	if (!mInventory->IsOpened()) {
		float mouseSpeed = gMouseSens / 2.0f;
		mPitch.SetTarget(mPitch.GetTarget() + ruInput::GetMouseYSpeed() * mouseSpeed);
		if (mLadder.expired()) {
			mYaw.SetTarget(mYaw.GetTarget() - ruInput::GetMouseXSpeed() * mouseSpeed);
		}
	}

	mPitch.ChaseTarget(0.233f);
	mYaw.ChaseTarget(0.233f);

	mpCamera->mCamera->SetLocalRotation(ruQuaternion(ruVector3(0, 1, 0), -mYawWalkOffset));
	mNeck->SetLocalRotation(ruQuaternion(ruVector3(1, 0, 0), mPitch));

	// lock yaw rotation when climbing on ladders, and also orient player to face ladder
	if (!mLadder.expired()) {
		auto ladder = mLadder.lock();
		if (ladder->mStatus == Ladder::Status::ActorClimbInBottom || ladder->mStatus == Ladder::Status::ActorClimbInTop) {
			ruVector3 delta = (ladder->mEnterZone->GetPosition() - mBody->GetPosition()).Normalize();
			mYaw.SetTarget(atan2(delta.x, delta.z) * 180.0f / M_PI);
		}
	}

	mBody->SetRotation(ruQuaternion(ruVector3(0, 1, 0), mYaw));

	// look left
	if (ruInput::IsKeyDown(mKeyLookLeft)) {
		ruVector3 rayBegin = mBody->GetPosition() + ruVector3(mBodyWidth / 2, 0, 0);
		ruVector3 rayEnd = rayBegin + mBody->GetRightVector() * 10.0f;
		ruVector3 hitPoint;
		shared_ptr<ruSceneNode> leftIntersection = ruPhysics::CastRay(rayBegin, rayEnd, &hitPoint);
		bool canLookLeft = true;
		if (leftIntersection) {
			float dist2 = (hitPoint - mBody->GetPosition()).Length2();
			if (dist2 < 0.4) {
				canLookLeft = false;
			}
		}
		if (canLookLeft) {
			mLookLeftAnimation.SetEnabled(true);
			SetSpineAnimation(&mLookLeftAnimation);
			if (mLookLeftAnimation.GetCurrentFrame() == mLookLeftAnimation.GetEndFrame()) {
				mLookLeftAnimation.SetInterpolator(0.0f);
			}
		} else {
			mLookLeftAnimation.SetDirection(ruAnimation::Direction::Reverse);
		}
	} else {
		mLookLeftAnimation.SetDirection(ruAnimation::Direction::Reverse);
	}

	// look right
	if (ruInput::IsKeyDown(mKeyLookRight)) {
		ruVector3 rayBegin = mBody->GetPosition() - ruVector3(mBodyWidth / 2.0f, 0.0f, 0.0f);
		ruVector3 rayEnd = rayBegin - mBody->GetRightVector() * 10.0f;
		ruVector3 hitPoint;
		shared_ptr<ruSceneNode> rightIntersection = ruPhysics::CastRay(rayBegin, rayEnd, &hitPoint);
		bool canLookRight = true;
		if (rightIntersection) {
			float dist2 = (hitPoint - mBody->GetPosition()).Length2();
			if (dist2 < 0.4f) {
				canLookRight = false;
			}
		}
		if (canLookRight) {
			mLookRightAnimation.SetDirection(ruAnimation::Direction::Forward);
			mLookRightAnimation.SetEnabled(true);
			SetSpineAnimation(&mLookRightAnimation);
			if (mLookRightAnimation.GetCurrentFrame() == mLookRightAnimation.GetEndFrame()) {
				mLookRightAnimation.SetInterpolator(0.0f);
			}
		} else {
			mLookRightAnimation.SetDirection(ruAnimation::Direction::Reverse);
		}
	} else {
		mLookRightAnimation.SetDirection(ruAnimation::Direction::Reverse);
	}
}

void Player::UpdateJumping() {
	// do ray test, to determine collision with objects above camera
	shared_ptr<ruSceneNode> headBumpObject = ruPhysics::CastRay(mBody->GetPosition() + ruVector3(0.0f, mBodyHeight * 0.98f, 0.0f), mBody->GetPosition() + ruVector3(0, 1.02 * mBodyHeight, 0), nullptr);

	if (ruInput::IsKeyHit(mKeyJump)) {
		if (IsCanJump()) {
			mJumpTo = ruVector3(0.0f, 350.0f, 0.0f);
			mLanded = false;
			mLandedSoundEmitted = false;

			mJumpAnimation.SetEnabled(true);
			SetBodyAnimation(&mJumpAnimation);
		}
	}

	mGravity = mGravity.Lerp(mJumpTo, 0.65f);

	if (mGravity.y >= mJumpTo.y) {
		mLanded = true;
	}

	if (!mLandedSoundEmitted) {
		if (IsCanJump() && mLanded) {
			mLandedSoundEmitted = true;
		}
	}

	if (mLanded || headBumpObject) {
		mJumpTo = ruVector3(0, -400.0f, 0.0f);
		if (IsCanJump()) {
			mJumpTo = ruVector3(0.0f, 0.0f, 0.0f);
		}
	};
}

void Player::UpdateMoving() {
	mYawWalkOffset.SetTarget(0.0f);

	if (!mLadder.expired()) {
		// ladder climbing
		auto ladder = mLadder.lock();

		// stealth can't be done on ladders
		mStealthMode = false;

		// stand up
		Crouch(false);

		mAirPosition = mBody->GetPosition();
		mRunning = false;

		// when climbing on ladder, model is moving and physical body is sticked to it
		mBodyModel->Detach();
		mBodyModel->SetRotation(ruQuaternion(ruVector3(0, 1, 0), mYaw));
		mBody->SetPosition(mBodyModel->GetPosition());

		if (ladder->mStatus == Ladder::Status::ActorClimbInBottom || ladder->mStatus == Ladder::Status::ActorClimbInTop) {
			// hold player in position
			if (ladder->mStatus == Ladder::Status::ActorClimbInBottom) {
				mBodyModel->SetPosition(ladder->mBegin->GetPosition());
			} else if (ladder->mStatus == Ladder::Status::ActorClimbInTop) {
				mBodyModel->SetPosition(ladder->mEnd->GetPosition());
			}

			// if animation disabled, then player just climbed-in
			if (!mLadderCrawlInAnimation.IsEnabled()) {
				ladder->mStatus = Ladder::Status::ActorStand;
			}
		}

		// step is allowed only when player is just standing on the ladder
		if (ladder->mStatus == Ladder::Status::ActorStand) {
			float dBegin = ladder->mBegin->GetPosition().Distance(mBodyModel->GetPosition());
			float dEnd = ladder->mEnd->GetPosition().Distance(mBodyModel->GetPosition());

			if (dBegin < mLadderClimbDelta && ruInput::IsKeyDown(mKeyMoveBackward)) {
				// climb off on bottom
				ladder->mStatus = Ladder::Status::ActorClimbOffBottom;
			} else if (dEnd < mLadderClimbDelta && ruInput::IsKeyDown(mKeyMoveForward)) {
				// climb off on top
				ladder->mStatus = Ladder::Status::ActorClimbOffTop;
			} else if (ruInput::IsKeyDown(mKeyMoveForward)) {
				// step up
				ladder->mStatus = Ladder::Status::ActorClimbingUp;
				mLadderCrawlAnimation.SetDirection(ruAnimation::Direction::Forward);
				mLadderCrawlAnimation.SetEnabled(true);
				mBodyModel->SetPosition(mBodyModel->GetPosition() + ruVector3(0, 1, 0) * mLadderClimbDelta);
				SetBodyAnimation(&mLadderCrawlAnimation);
			} else if (ruInput::IsKeyDown(mKeyMoveBackward)) {
				// step down
				ladder->mStatus = Ladder::Status::ActorClimbingDown;
				mLadderCrawlAnimation.SetDirection(ruAnimation::Direction::Reverse);
				mLadderCrawlAnimation.SetEnabled(true);
				mBodyModel->SetPosition(mBodyModel->GetPosition() - ruVector3(0, 1, 0) * mLadderClimbDelta);
				SetBodyAnimation(&mLadderCrawlAnimation);
			}
		}

		if (!mLadderCrawlAnimation.IsEnabled() && (ladder->mStatus == Ladder::Status::ActorClimbingUp || ladder->mStatus == Ladder::Status::ActorClimbingDown)) {
			ladder->mStatus = Ladder::Status::ActorStand;
		}

		if (ruInput::IsKeyHit(mKeyJump) || ladder->mStatus == Ladder::Status::ActorClimbOffBottom || ladder->mStatus == Ladder::Status::ActorClimbOffTop) {
			// return control to physics
			Unfreeze();

			// move on specific position when climb off
			if (ladder->mStatus == Ladder::Status::ActorClimbOffBottom) {
				SetPosition(ladder->mBeginLeavePoint->GetPosition());
			} else if (ladder->mStatus == Ladder::Status::ActorClimbOffTop) {
				SetPosition(ladder->mEndLeavePoint->GetPosition());
			}

			// free ladder
			ladder->mStatus = Ladder::Status::Free;
			mLadder.reset();

			// jump back
			if (ruInput::IsKeyHit(mKeyJump)) {
				mBody->SetVelocity(ruVector3(-mBody->GetLookVector()));
			}
		}

	} else {
		// ordinary movement
		mSpeedTo = ruVector3(0, 0, 0);

		bool moveBack = false;
		bool isMoving = false;

		if (ruInput::IsKeyDown(mKeyMoveForward)) {
			mSpeedTo = mSpeedTo + mBody->GetLookVector();
			isMoving = true;
		}
		if (ruInput::IsKeyDown(mKeyMoveBackward)) {
			mSpeedTo = mSpeedTo - mBody->GetLookVector();
			moveBack = true;
			isMoving = true;
		}
		if (ruInput::IsKeyDown(mKeyStrafeLeft)) {
			mSpeedTo = mSpeedTo + mBody->GetRightVector();
			isMoving = true;
			// body rotation offset when strafing
			mYawWalkOffset.SetTarget(mYawWalkOffset.GetMax());
		}
		if (ruInput::IsKeyDown(mKeyStrafeRight)) {
			mSpeedTo = mSpeedTo - mBody->GetRightVector();
			isMoving = true;
			// body rotation offset when strafing
			mYawWalkOffset.SetTarget(mYawWalkOffset.GetMin());
		}

		mYawWalkOffset.ChaseTarget(0.07f);
		mBodyModel->SetRotation(ruQuaternion(ruVector3(0, 1, 0), mYawWalkOffset));



		mMoved = mSpeedTo.Length2() > 0;

		if (mMoved) {
			mSpeedTo = mSpeedTo.Normalize();
		}

		UpdateJumping();

		mFov.SetTarget(mFov.GetMin());

		// running and stamina management
		mRunning = false;
		if (!IsCrouch() && ruInput::IsKeyDown(mKeyRun) && mMoved && !mNodeInHands) {
			if (mStamina > 0) {
				mSpeedTo = mSpeedTo * mRunSpeedMult;
				mStamina -= 0.1333f;
				mFov.SetTarget(mFov.GetMax());
				mRunning = true;
			}
		} else {
			if (mStamina < mMaxStamina) {
				mStamina += 0.2666f;				
			}
		}

		// manage basic animations
		if (isMoving) {
			if (mCrouch) {
				if (!mCrouchAnimation.IsEnabled()) {
					SetBodyAnimation(&mCrouchWalkAnimation);
				}
			} else {
				if (mRunning) {
					SetBodyAnimation(&mRunAnimation);
				} else {
					SetBodyAnimation(&mWalkAnimation);
				}
			}
		} else {
			if (mCrouch) {
				SetBodyAnimation(&mCrouchAnimation);
			} else {
				if (!mCrouchAnimation.IsEnabled()) {
					SetBodyAnimation(&mStayAnimation);
				}
			}
		}

		mWalkAnimation.SetEnabled(isMoving && !mRunning);
		mCrouchWalkAnimation.SetEnabled(true);
		mRunAnimation.SetEnabled(isMoving && mRunning);

		// set animation playback direction according to actual speed of player
		mWalkAnimation.SetDirection(moveBack ? ruAnimation::Direction::Reverse : ruAnimation::Direction::Forward);
		mCrouchWalkAnimation.SetDirection(moveBack ? ruAnimation::Direction::Reverse : ruAnimation::Direction::Forward);
		mRunAnimation.SetDirection(moveBack ? ruAnimation::Direction::Reverse : ruAnimation::Direction::Forward);

		if (moveBack) {
			mSpeedTo = mSpeedTo * 0.4f;
		}

		if (ruInput::IsKeyHit(mKeyStealth)) {
			Crouch(!IsCrouch());
			mStealthMode = IsCrouch();
		}

		// crouch
		if (mCrouch) {
			mCrouchMultiplier -= 0.025f;
			if (mCrouchMultiplier < 0.5f) {
				mCrouchMultiplier = 0.5f;
			}
		} else {
			mCrouchMultiplier += 0.025f;
			if (mCrouchMultiplier > 1.0f) {
				mCrouchMultiplier = 1.0f;
			}
		}		

		mSpeedTo = mSpeedTo * (mStealthMode ? 0.4f : 1.0f);

		mFov.ChaseTarget(0.0666f);
		mpCamera->mCamera->SetFOV(mFov);

		mSpeed = mSpeed.Lerp(mSpeedTo + mGravity, 0.1666f);
		Step(mSpeed * ruVector3(100, 1, 100), 0.01666f);

		if (mBodyModel->GetParent() == nullptr) {
			mBodyModel->Attach(mBody);
		}


		mBody->SetLocalScale(ruVector3(1, mCrouchMultiplier, 1));
		
		mBodyModel->SetPosition(ruVector3(0, -1.f * mCrouchMultiplier, -0.1));
	}

	// update animations
	mWalkAnimation.Update();
	mStayAnimation.Update();
	mCrouchWalkAnimation.Update();
	mCrouchAnimation.Update();
	mPushDoorAnimation.Update();
	mPickUpAnimation.Update();
	mJumpAnimation.Update();
	mLookLeftAnimation.Update();
	mLookRightAnimation.Update();
	mLadderCrawlInAnimation.Update();
	mLadderCrawlAnimation.Update();
	mRunAnimation.Update();
	mSwitchFlashlightAnimation.Update();
}

void Player::Crouch(bool state) {
	mCrouch = state;

	mCrouchAnimation.SetEnabled(true);
	mCrouchAnimation.SetDirection(mCrouch ? ruAnimation::Direction::Forward : ruAnimation::Direction::Reverse);

	// stand up only if we can
	ruVector3 pickPoint;
	const ruVector3 rayBegin = mBody->GetPosition() + ruVector3(0, mBodyHeight * mCrouchMultiplier * 1.025f, 0);
	const ruVector3 rayEnd = mBody->GetPosition() + ruVector3(0, mBodyHeight * 1.05f, 0);
	const shared_ptr<ruSceneNode> upCast = ruPhysics::CastRay(rayBegin, rayEnd, &pickPoint);
	if (upCast) {
		if (!mCrouch) {
			mCrouch = true;
		}
	}
}

void Player::ComputeStealth() {
	mInLight = false;

	shared_ptr<ruLight> closestLight;
	float distance = FLT_MAX;
	// check if player inside any point light
	for (int i = 0; i < ruPointLight::GetCount(); i++) {
		const auto light = ruPointLight::Get(i);
		if (light->IsVisible()) {
			if (!(light == mFakeLight)) {
				const float d = light->GetPosition().Distance(mBody->GetPosition());
				if (d < distance) {
					closestLight = light;
					distance = d;
				}
				if (ruPointLight::Get(i)->IsSeePoint(mBody->GetPosition())) {
					mInLight = true;
				}
			}
		}
	}

	// check if player inside any spot light's cone
	if (!mInLight) {
		for (int i = 0; i < ruSpotLight::GetCount(); i++) {
			const auto light = ruSpotLight::Get(i);
			if (light->IsVisible()) {
				const float d = light->GetPosition().Distance(mBody->GetPosition());
				if (d < distance) {
					closestLight = light;
					distance = d;
				}

				if (ruSpotLight::Get(i)->IsSeePoint(mBody->GetPosition())) {
					mInLight = true;
					closestLight = light;
				}
			}
		}
	}

	if (ruDirectionalLight::GetCount()) {
		mInLight = true;
	}

	if (mFlashlightEnabled) {
		mInLight = true;
	}

	mWhispersSound->Play();

	if (mInLight) {
		mWhispersSoundVolume -= 0.001f;
		if (mWhispersSoundVolume < 0.0f) {
			mWhispersSoundVolume = 0.0f;
		}
	} else {
		mWhispersSoundVolume += 0.00004f;
		if (mWhispersSoundVolume > 0.085f) {
			mWhispersSoundVolume = 0.085f;
		}
	}

	mWhispersSound->SetVolume(mWhispersSoundVolume);

	mStealthFactor = 0.0f;

	const float d = closestLight ? 1.0f - closestLight->GetPosition().Distance(mBody->GetPosition()) / closestLight->GetRange() : 0.0f;

	mStealthFactor += mStealthMode ? d * 0.5f : d;
	if (mStealthMode) {
		mStealthFactor += mMoved ? 0.25f : 0.0f;		
	} else {
		mStealthFactor += mMoved ? 0.5f : 0.0f;
		mStealthFactor += mRunning ? 1.0f : 0.0f;
	}
	 
	if (mMoved) {
		mNoiseFactor = mStealthMode ? 0.5f : 1.0f;
	} else {
		mNoiseFactor = 0.1;
	}

	mHUD->SetNoise(mNoiseFactor * 100.0f);
	/*
	if (mInLight) {
		const float d = 1.0f - closestLight->GetPosition().Distance(mBody->GetPosition()) / closestLight->GetRange();

		mStealthFactor += mStealthMode ? d * 0.5f : d;
		mStealthFactor += mMoved ? 0.5f : 0.0f;
		mStealthFactor += mRunning ? 1.0f : 0.0f;
	} else {
		mStealthFactor += mStealthMode ? 0.0f : 0.1f;
		mStealthFactor += mMoved ? 0.1f : 0.0f;
		mStealthFactor += mRunning ? 0.25f : 0.0f;
	}
	*/

	int stealth = (100 * ((mStealthFactor > 1.05f) ? 1.0f : (mStealthFactor + 0.05f)));
	if (stealth > 100) {
		stealth = 100;
	}
	mHUD->SetStealth(stealth);
}


void Player::Update() {
	if (mDead) {
		mpCamera->mCamera->SetRotation(ruQuaternion(ruVector3(1, 0, 0), 0)); // HAAAAX!!
		mBody->SetLocalRotation(ruQuaternion(ruVector3(1, 0, 0), mDeadRotation));
		mDeadRotation += (mDestDeadRotation - mDeadRotation) * 0.1f;
	}

	mpCamera->Update();
	mHUD->Update();

	if (!pMainMenu->IsVisible()) {
		DrawStatusBar();
		if (!mDead) {
			mHUD->SetDead(false);
			mHUD->SetCursor(mNearestPickedNode != nullptr, IsObjectHasNormalMass(mNearestPickedNode), mNodeInHands != nullptr, mInventory->IsOpened());

			--mHealthRegenTimer;
			if (mHealthRegenTimer <= 0) {
				Heal(0.05f);
				mHealthRegenTimer = 0;
			}

			if (ruInput::IsKeyHit(mKeyFlashlightHotkey)) {
				mSwitchFlashlightAnimation.SetEnabled(true);
				SetBodyAnimation(&mSwitchFlashlightAnimation);
			}

			if (mFlashlightEnabled) {
				mFlashlight->Show();
			} else {
				mFlashlight->Hide();				
			}			

			UpdateMouseLook();
			Interact();
			UpdateMoving();
			ComputeStealth();
			UpdatePicking();
			UpdateItemsHandling();
			UpdateInventory();
			if (mLadder.expired()) { // prevent damaging from ladders
				if (!IsCanJump() && !mInAir) { // in air
					mAirPosition = mBody->GetPosition();
					mInAir = true;
				} else if (IsCanJump() && mInAir) { // landing 
					const ruVector3 curPos = mBody->GetPosition();
					const float heightDelta = fabsf(curPos.y - mAirPosition.y);
					if (heightDelta > 2.0f) {
						Damage(heightDelta * 10);
					}
					mInAir = false;
				}
			}

			// this must be placed in other place :)
			if (mAutoSaveTimer->GetElapsedTimeInSeconds() >= 30) {
				SaveWriter("autosave.save").SaveWorldState();
				mAutoSaveTimer->Restart();
			}
		} else {
			mHUD->SetDead(true);
		}

		// after finishing playing death sound, show menu and destroy current level
		if (mDeadSound) {
			if (!mDeadSound->IsPlaying()) {
				pMainMenu->Show();
			}
		}
	}
}

void Player::UpdateItemsHandling() {
	if (mNodeInHands) {
		if (mPitch < 70) {
			const ruVector3 ppPos = mItemPoint->GetPosition();
			const ruVector3 objectPos = mNodeInHands->GetPosition() + mPickCenterOffset;
			const ruVector3 dir = ppPos - objectPos;
			if (ruInput::IsMouseDown(ruInput::MouseButton::Left)) {
				mNodeInHands->Move(dir * 6);

				mNodeInHands->SetAngularVelocity(ruVector3(0, 0, 0));

				if (ruInput::IsMouseDown(ruInput::MouseButton::Right)) {
					if (UseStamina(mNodeInHands->GetMass())) {
						mNodeInHands->Move((ppPos - mpCamera->mCamera->GetPosition()).Normalize() * 6);
					}

					mObjectThrown = true;
					mNodeInHands = nullptr;
				}
			} else {
				mNodeInHands->SetAngularVelocity(ruVector3(1, 1, 1));
				mNodeInHands = nullptr;
			}
		} else {
			mNodeInHands = nullptr;
		}
	}


	if (!ruInput::IsMouseDown(ruInput::MouseButton::Left)) {
		mObjectThrown = false;
	}
}

void Player::UpdatePicking() {
	ruVector3 pickPosition;

	mPickedNode = ruPhysics::RayPick(ruVirtualScreenWidth / 2, ruVirtualScreenHeight / 2, &pickPosition);

	mNearestPickedNode = nullptr;

	// discard picked node if it is node of enemy - so funny when picking enemy as an object lol :)
	if (Level::Current()) {
		auto & enemy = Level::Current()->GetEnemy();
		if (enemy) {
			if (mPickedNode == enemy->GetBody()) {
				mPickedNode = nullptr;
			}
		}
	}

	if (mPickedNode && !mNodeInHands) {
		mNodeInHands = nullptr;

		const ruVector3 ppPos = mPickPoint->GetPosition();
		const ruVector3 dir = ppPos - pickPosition;

		const auto pIO = Level::Current()->FindInteractiveObject(mPickedNode->GetName());
		if (dir.Length2() < 1.5f) {
			mNearestPickedNode = mPickedNode;
			string pickedObjectDesc;
			if (pIO) {
				mHUD->SetAction(mKeyUse, StringBuilder() << pIO->GetPickDescription() << " - " << mLocalization.GetString("itemPick"));
			} else {
				if (IsObjectHasNormalMass(mPickedNode) && !mPickedNode->IsFrozen()) {
					mHUD->SetAction(ruInput::Key::None, mLocalization.GetString("objectPick"));
				}
			}

			if (ruInput::IsMouseDown(ruInput::MouseButton::Left)) {
				if (IsObjectHasNormalMass(mPickedNode)) {
					if (!mPickedNode->IsFrozen() && !mObjectThrown) {
						mNodeInHands = mPickedNode;
						mPickCenterOffset = pickPosition - mPickedNode->GetPosition();
					}
				}
			}
		}
	}
}

void Player::FreeHands() {
	mNodeInHands = nullptr;
}

bool Player::IsUseButtonHit() {
	return ruInput::IsKeyHit(mKeyUse);
}

bool Player::IsObjectHasNormalMass(shared_ptr<ruSceneNode> node) {
	return node ? (node->GetMass() > 0 && node->GetMass() < 40) : false;
}

void Player::Serialize(SaveFile & s) {
	ruVector3 position = mBody->GetPosition();
	string ladderName = mLadder.expired() ? "undefinedWay" : mLadder.lock()->mEnterZone->GetName();
	bool collisionEnabled = mBody->IsCollisionEnabled();

	s & ladderName;
	s & collisionEnabled;
	s & position;
	s & mSpeed;
	s & mSpeedTo;
	s & mGravity;
	s & mJumpTo;
	s & mLanded;
	s & mStamina;
	s & mHealth;
	s & mMaxHealth;
	s & mMaxStamina;
	s & mRunSpeedMult;
	s & mObjectThrown;
	s & mDead;
	s & mFrameColor;
	s & mMoved;
	s & mKeyMoveForward;
	s & mKeyMoveBackward;
	s & mKeyStrafeLeft;
	s & mKeyStrafeRight;
	s & mKeyJump;
	s & mKeyFlashlightHotkey;
	s & mKeyRun;
	s & mKeyInventory;
	s & mKeyUse;
	s & mStealthMode;
	s & mFlashlightLocked;
	s & mLastHealth;
	s & mCrouch;
	s & mPitch;
	s & mYaw;
	s & mFov;
	s & mHealthRegenTimer;
	s & mFlashlightEnabled;

	// serialize animations
	s & mWalkAnimation;
	s & mCrouchAnimation;
	s & mCrouchWalkAnimation;
	s & mRunAnimation;
	s & mStayAnimation;
	s & mGrabAnimation;
	s & mPushDoorAnimation;
	s & mPickUpAnimation;
	s & mJumpAnimation;
	s & mLookRightAnimation;
	s & mLookLeftAnimation;
	s & mLadderCrawlInAnimation;
	s & mLadderCrawlAnimation;
	s & mSwitchFlashlightAnimation;

	mInventory->Serialize(s);

	if (s.IsLoading()) {
		mBody->SetCollisionEnabled(collisionEnabled);
		mBody->SetFriction(0);
		mBody->SetPosition(position);
		mBody->SetAngularFactor(ruVector3(0, 0, 0));

		mLadder = Level::Current()->FindLadder(ladderName);
		if (!mLadder.expired()) {
			mBodyModel->Detach();
			mBodyModel->SetPosition(position);
			Freeze();
		}
	}
}

Parser * Player::GetLocalization() {
	return &mLocalization;
}


unique_ptr<Inventory> & Player::GetInventory() {
	return mInventory;
}

void Player::SetHUDVisible(bool state) {
	mHUD->SetVisible(state);
	if (!state) {
		mInventory->SetVisible(state);
	}
}

bool Player::IsDead() {
	return mHealth <= 0.0f;
}

void Player::SetPosition(ruVector3 position) {
	Actor::SetPosition(position);
	mAirPosition = mBody->GetPosition(); // prevent death from 'accidental' landing :)
}

void Player::TurnOffFakeLight() {
	mFakeLight->SetRange(0.001f);
}

float Player::GetHealth() {
	return mHealth;
}

void Player::SetHealth(float health) {
	mHealth = health;
}

void Player::LockFlashlight(bool state) {
	mFlashlightLocked = state;
}

inline void Player::Interact() {
	// interact with ladders
	for (auto ladder : Level::Current()->GetLadderList()) {
		// can climb-in only on free ladders
		if (ladder->mStatus == Ladder::Status::Free) {
			if (mNearestPickedNode == ladder->mEnterZone) {
				mHUD->SetAction(mKeyUse, mLocalization.GetString("crawlIn"));
				if (IsUseButtonHit()) {
					// select which way to climb-in
					if (ladder->mBegin->GetPosition().Distance(mBody->GetPosition()) < ladder->mEnd->GetPosition().Distance(mBody->GetPosition())) {
						ladder->mStatus = Ladder::Status::ActorClimbInBottom;
					} else {
						ladder->mStatus = Ladder::Status::ActorClimbInTop;
					}

					// enable climb-in animation
					mLadderCrawlInAnimation.SetEnabled(true);
					SetBodyAnimation(&mLadderCrawlInAnimation);

					// freeze physical body, to manual control of position
					Freeze();
					mLadder = ladder;
				}
			}
		}
	}


	// interact with doors
	for (auto pDoor : Level::Current()->GetDoorList()) {
		pDoor->Update();
		if (mNearestPickedNode == pDoor->GetNode()) {
			if (!pDoor->IsLocked()) {
				mHUD->SetAction(mKeyUse, mLocalization.GetString("openClose"));
				if (IsUseButtonHit()) {
					mPushDoorAnimation.SetEnabled(true); // <<<< Animation
					SetLeftArmAnimation(&mPushDoorAnimation);

					pDoor->SwitchState();
				}
			} else {
				mHUD->SetAction(ruInput::Key::None, mLocalization.GetString("doorLocked"));
			}
		}
	}
}

void Player::Step(ruVector3 direction, float speed) {
	// spring based step
	const ruVector3 currentPosition = mBody->GetPosition();
	const ruVector3 rayBegin = currentPosition;
	const ruVector3 rayEnd = rayBegin - ruVector3(0, 5, 0);
	ruVector3 intPoint;
	const shared_ptr<ruSceneNode> rayResult = ruPhysics::CastRay(rayBegin, rayEnd, &intPoint);
	ruVector3 pushpullVelocity = ruVector3(0, 0, 0);
	if (rayResult && !(rayResult == mBody)) {
		pushpullVelocity.y = -(currentPosition.y - intPoint.y - mSpringLength * mCrouchMultiplier) * 4.4f;
	}
	mBody->Move(direction * speed + pushpullVelocity);
}

void Player::EmitStepSound() {
	if (mMoved) {
		const ruRayCastResultEx result = ruPhysics::CastRayEx(mBody->GetPosition() + ruVector3(0, 0.1, 0), mBody->GetPosition() - ruVector3(0, mBodyHeight * 2.2, 0));
		if (result.valid) {
			for (auto & sMat : mSoundMaterialList) {
				const auto snd = sMat->GetRandomSoundAssociatedWith(result.textureName);
				if (snd) {
					snd->Play(false);
				}
			}
		}
	}
}

void Player::SetLeftLegAnimation(ruAnimation * anim) {
	for (auto bp : mLeftLeg) {
		bp->SetAnimation(anim, true);
	}
}

void Player::SetRightLegAnimation(ruAnimation * anim) {
	for (auto bp : mRightLeg) {
		bp->SetAnimation(anim, true);
	}
}

void Player::SetLeftArmAnimation(ruAnimation * anim) {
	for (auto bp : mLeftArm) {
		bp->SetAnimation(anim, true);
	}
}

void Player::SetRightArmAnimation(ruAnimation * anim) {
	for (auto bp : mRightArm) {
		bp->SetAnimation(anim, true);
	}
}

void Player::SetSpineAnimation(ruAnimation * anim) {
	for (auto bp : mSpine) {
		bp->SetAnimation(anim, true);
	}
}

void Player::SetLegsAnimation(ruAnimation * anim) {
	SetLeftLegAnimation(anim);
	SetRightLegAnimation(anim);
}

void Player::SetArmsAnimation(ruAnimation * anim) {
	SetLeftArmAnimation(anim);
	SetRightArmAnimation(anim);
}

void Player::SetBodyAnimation(ruAnimation * anim) {
	// do partial animation
	if (mPushDoorAnimation.IsEnabled()) {
		SetLeftArmAnimation(&mPushDoorAnimation);
		SetRightArmAnimation(anim);
		SetLegsAnimation(anim);
		SetSpineAnimation(anim);
	} else if (mPickUpAnimation.IsEnabled()) {
		SetLeftArmAnimation(&mPickUpAnimation);
		SetRightArmAnimation(anim);
		SetLegsAnimation(anim);
		SetSpineAnimation(anim);
	} else if (mSwitchFlashlightAnimation.IsEnabled()) {
		SetLeftArmAnimation(&mSwitchFlashlightAnimation);
		SetRightArmAnimation(anim);
		SetLegsAnimation(anim);
		SetSpineAnimation(anim);
	} else if (mLookLeftAnimation.IsEnabled()) {
		SetSpineAnimation(&mLookLeftAnimation);
		SetLegsAnimation(anim);
		SetArmsAnimation(anim);
	} else if (mLookRightAnimation.IsEnabled()) {
		SetSpineAnimation(&mLookRightAnimation);
		SetLegsAnimation(anim);
		SetArmsAnimation(anim);
	} else if (mJumpAnimation.IsEnabled()) {
		mBodyModel->SetAnimation(&mJumpAnimation);
	} else {
		mBodyModel->SetAnimation(anim);
	}
}

void Player::LadderEmitStepSound() {
	if (!mLadder.expired()) {
		auto ladder = mLadder.lock();
		if (ladder->mEnterZone->GetTextureCount() > 0) {
			for (auto & sMat : mSoundMaterialList) {
				shared_ptr<ruSound> & snd = sMat->GetRandomSoundAssociatedWith(ladder->mEnterZone->GetTexture(0)->GetName());
				if (snd) {
					snd->Play(true);
				}
			}
		}
	}
}