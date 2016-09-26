#include "Precompiled.h"

#include "Player.h"
#include "Menu.h"
#include "Door.h"
#include "utils.h"
#include "Enemy.h"
#include "SaveWriter.h"
#include "BareHands.h"
#include "Level.h"



extern double gFixedTick;

Player::Player() :
	Actor(0.7f, 0.2f),
	mStepLength(0.0f),
	mCameraTrembleTime(0.0f),
	mFlashlightLocked(false),
	mCurrentUsableObject(nullptr),
	mLandedSoundEmitted(true),
	mMaxStamina(100.0f),
	mStamina(100.0f),
	mFov(75.0f, 75.0f, 80.0f),
	mRunSpeedMult(2.5f),
	mObjectThrown(false),
	mPitch(0.0f, -89.9f, 89.9f),
	mYaw(0.0f),
	mHeadAngle(0.0f, -12.50f, 12.50f),
	mDead(false),
	mLanded(false),
	mStealthMode(false),
	mWhispersSoundVolume(0.0f),
	mWhispersSoundVolumeTo(0.0f),
	mCameraShakeOffset(0, mHeadHeight, 0),
	mRunCameraShakeCoeff(1.0f),
	mCameraBobCoeff(0),
	mFrameColor(1.0f, 1.0f, 1.0f),
	mKeyMoveForward(ruInput::Key::W),
	mKeyMoveBackward(ruInput::Key::S),
	mKeyStrafeLeft(ruInput::Key::A),
	mKeyStrafeRight(ruInput::Key::D),
	mKeyJump(ruInput::Key::Space),
	mKeyFlashLight(ruInput::Key::F),
	mKeyRun(ruInput::Key::LeftShift),
	mKeyInventory(ruInput::Key::Tab),
	mKeyUse(ruInput::Key::R),
	mKeyStealth(ruInput::Key::C),
	mKeyLookLeft(ruInput::Key::Q),
	mKeyLookRight(ruInput::Key::E),
	mInAir(false),
	mDeadRotation(0.0f),
	mDestDeadRotation(0.0f),
	mNodeInHands(nullptr) {
	mLocalization.ParseFile(gLocalizationPath + "player.loc");

	mHUD = unique_ptr<HUD>(new HUD());

	mInventory = unique_ptr<Inventory>(new Inventory());
	

	gMouseSens = 0.5f;

	mLastHealth = mHealth;

	CreateCamera();
	LoadSounds();

	mBody->SetName("Player");



	// hack
	pMainMenu->SyncPlayerControls();

	mAutoSaveTimer = ruTimer::Create();

	mPainSound.push_back(ruSound::Load2D("data/sounds/player/grunt1.ogg"));
	mPainSound.push_back(ruSound::Load2D("data/sounds/player/grunt2.ogg"));
	mPainSound.push_back(ruSound::Load2D("data/sounds/player/grunt3.ogg"));

	mWhispersSound = ruSound::Load2D("data/sounds/whispers.ogg");
	mWhispersSound->SetVolume(0.085f);
	mWhispersSound->SetLoop(true);
	//mWhispersSound->Play();

	for (auto & ps : mPainSound) {
		ps->SetVolume(0.7);
	}

	mSoundMaterialList.push_back(unique_ptr<SoundMaterial>(new SoundMaterial("data/materials/stone.smat", mpCamera->mCamera)));
	mSoundMaterialList.push_back(unique_ptr<SoundMaterial>(new SoundMaterial("data/materials/metal.smat", mpCamera->mCamera)));
	mSoundMaterialList.push_back(unique_ptr<SoundMaterial>(new SoundMaterial("data/materials/wood.smat", mpCamera->mCamera)));
	mSoundMaterialList.push_back(unique_ptr<SoundMaterial>(new SoundMaterial("data/materials/gravel.smat", mpCamera->mCamera)));
	mSoundMaterialList.push_back(unique_ptr<SoundMaterial>(new SoundMaterial("data/materials/muddyrock.smat", mpCamera->mCamera)));
	mSoundMaterialList.push_back(unique_ptr<SoundMaterial>(new SoundMaterial("data/materials/rock.smat", mpCamera->mCamera)));
	mSoundMaterialList.push_back(unique_ptr<SoundMaterial>(new SoundMaterial("data/materials/grass.smat", mpCamera->mCamera)));
	mSoundMaterialList.push_back(unique_ptr<SoundMaterial>(new SoundMaterial("data/materials/soil.smat", mpCamera->mCamera)));
	mSoundMaterialList.push_back(unique_ptr<SoundMaterial>(new SoundMaterial("data/materials/chain.smat", mpCamera->mCamera)));
	mSoundMaterialList.push_back(unique_ptr<SoundMaterial>(new SoundMaterial("data/materials/mud.smat", mpCamera->mCamera)));

	AddUsableObject(new BareHands);
}

Player::~Player() {
	for (auto uo : mUsableObjectList) {
		delete uo;
	}
	mUsableObjectList.clear();
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

void Player::Damage(float dmg, bool headJitter) {
	if (mDead) {
		return;
	}

	Actor::Damage(dmg);
	mHUD->ShowDamage();
	
	if (headJitter) {
		mPitch.SetTarget(mPitch.GetTarget() + frandom(20, 40));
		mYaw.SetTarget(mYaw.GetTarget() + frandom(-40, 40));
	}
	if (mLastHealth - mHealth > 5) {
		mPainSound[rand() % mPainSound.size()]->Play();
		mLastHealth = mHealth;
	}
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
	if (mSheetInHands.expired()) {
		if (ruInput::IsKeyHit(mKeyInventory)) {
			mInventory->Open(!mInventory->IsOpened());
		}
		mInventory->Update();
	}
}

void Player::UpdateMouseLook() {
	if (!mInventory->IsOpened()) {
		float mouseSpeed = gMouseSens / 2.0f;
		mPitch.SetTarget(mPitch.GetTarget() + ruInput::GetMouseYSpeed() * mouseSpeed);
		mYaw.SetTarget(mYaw.GetTarget() - ruInput::GetMouseXSpeed() * mouseSpeed);
	}


	mPitch.ChaseTarget(0.233f);
	mYaw.ChaseTarget(0.233f);


	mpCamera->mCamera->SetRotation(ruQuaternion(ruVector3(1, 0, 0), mPitch));
	mBody->SetRotation(ruQuaternion(ruVector3(0, 1, 0), mYaw));

	mHeadAngle.SetTarget(0.0f);
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
			mHeadAngle.SetTarget(mHeadAngle.GetMin());
		}
	}

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
			mHeadAngle.SetTarget(mHeadAngle.GetMax());
		}
	}
	mHeadAngle.ChaseTarget(0.28f);
	mHead->SetRotation(ruQuaternion(ruVector3(0.0f, 0.0f, 1.0f), mHeadAngle));
}

void Player::UpdateJumping() {
	// do ray test, to determine collision with objects above camera
	shared_ptr<ruSceneNode> headBumpObject = ruPhysics::CastRay(mBody->GetPosition() + ruVector3(0.0f, mBodyHeight * 0.98f, 0.0f), mBody->GetPosition() + ruVector3(0, 1.02 * mBodyHeight, 0), nullptr);

	if (ruInput::IsKeyHit(mKeyJump)) {
		if (IsCanJump()) {
			mJumpTo = ruVector3(0.0f, 350.0f, 0.0f);
			mLanded = false;
			mLandedSoundEmitted = false;
		}
	}

	mGravity = mGravity.Lerp(mJumpTo, 0.65f);

	if (mGravity.y >= mJumpTo.y) {
		mLanded = true;
	}

	if (!mLandedSoundEmitted) {
		if (IsCanJump() && mLanded) {
			// two feet
			EmitStepSound();
			EmitStepSound();
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


	if (!mLadder.expired()) {
		auto ladder = mLadder.lock();

		mStealthMode = false;
		ladder->DoEntering();
		mAirPosition = mBody->GetPosition();
		Crouch(false);
		mRunning = false;
		if (ladder->IsPlayerInside()) {
			mMoved = false;
			if (ruInput::IsKeyDown(mKeyMoveForward)) {
				ladder->SetDirection(Ladder::Direction::Forward);
				mMoved = true;
			}
			if (ruInput::IsKeyDown(mKeyMoveBackward)) {
				ladder->SetDirection(Ladder::Direction::Backward);
				mMoved = true;
			}
			if (mMoved) {
				ladder->DoPlayerCrawling();
				if (!ladder->IsPlayerInside()) {
					mLadder.reset();
				}
			} else {
				StopInstant();
			}
		}
		if (ruInput::IsKeyHit(mKeyJump)) {
			ladder->LeaveInstantly();
			mLadder.reset();
			Unfreeze();
			mBody->SetVelocity(ruVector3(-mBody->GetLookVector()));
		}
	} else {
		ruVector3 look = mBody->GetLookVector();
		ruVector3 right = mBody->GetRightVector();

		mSpeedTo = ruVector3(0, 0, 0);

		bool moveBack = false;

		if (ruInput::IsKeyDown(mKeyMoveForward)) {
			mSpeedTo = mSpeedTo + look;
		}
		if (ruInput::IsKeyDown(mKeyMoveBackward)) {
			mSpeedTo = mSpeedTo - look;
			moveBack = true;
		}
		if (ruInput::IsKeyDown(mKeyStrafeLeft)) {
			mSpeedTo = mSpeedTo + right;
		}
		if (ruInput::IsKeyDown(mKeyStrafeRight)) {
			mSpeedTo = mSpeedTo - right;
		}

		mMoved = mSpeedTo.Length2() > 0;

		if (mMoved) {
			mSpeedTo = mSpeedTo.Normalize();
		}

		UpdateJumping();

		mRunCameraShakeCoeff = 1.0f;
		mFov.SetTarget(mFov.GetMin());

		mRunning = false;
		if (!IsCrouch() && ruInput::IsKeyDown(mKeyRun) && mMoved && !mNodeInHands) {
			if (mStamina > 0) {
				mSpeedTo = mSpeedTo * mRunSpeedMult;
				mStamina -= 0.1333f;
				mFov.SetTarget(mFov.GetMax());
				mRunCameraShakeCoeff = 1.425f;
				mRunning = true;
			}
		} else {
			if (mStamina < mMaxStamina) {
				if (mInLight) {
					mStamina += 0.2666f;
				}
			}
		}

		if (moveBack) {
			mSpeedTo = mSpeedTo * 0.4f;
		}

		if (ruInput::IsKeyHit(mKeyStealth)) {
			Crouch(!IsCrouch());
			mStealthMode = IsCrouch();
		}

		UpdateCrouch();

		mSpeedTo = mSpeedTo * (mStealthMode ? 0.4f : 1.0f);

		mFov.ChaseTarget(0.0666f);
		mpCamera->mCamera->SetFOV(mFov);

		mSpeed = mSpeed.Lerp(mSpeedTo + mGravity, 0.1666f);
		Step(mSpeed * ruVector3(100, 1, 100), 0.01666f);
	}

	UpdateCameraShake();
}


void Player::ComputeStealth() {
	mInLight = false;

	for (int i = 0; i < ruPointLight::GetCount(); i++) {
		if (!(ruPointLight::Get(i) == mFakeLight)) {
			if (ruPointLight::Get(i)->IsSeePoint(mBody->GetPosition())) {
				mInLight = true;
				break;
			}
		}
	}

	if (!mInLight) {
		for (int i = 0; i < ruSpotLight::GetCount(); i++) {
			if (ruSpotLight::Get(i)->IsSeePoint(mBody->GetPosition())) {
				mInLight = true;
				break;
			}
		}
	}

	if (GetFlashLight()) {
		if (GetFlashLight()->IsOn()) {
			mInLight = true;
		}
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

	if (mInLight) {
		mStealthFactor += mStealthMode ? 0.25f : 0.5f;
		mStealthFactor += mMoved ? 0.5f : 0.0f;
		mStealthFactor += mRunning ? 1.0f : 0.0f;
	} else {
		mStealthFactor += mStealthMode ? 0.0f : 0.25f;
		mStealthFactor += mMoved ? 0.1f : 0.0f;
		mStealthFactor += mRunning ? 0.25f : 0.0f;
	}

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

	if (GetFlashLight()) {
		if (GetFlashLight()->IsOn()) {
			mFakeLight->Hide();
		} else {
			mFakeLight->Show();
		}
	} else {
		mFakeLight->Show();
	}

	
	if (!pMainMenu->IsVisible()) {
		DrawStatusBar();
		if (!mDead) {
			mHUD->SetDead(false);
			mHUD->SetCursor(mNearestPickedNode != nullptr, IsObjectHasNormalMass(mNearestPickedNode), mNodeInHands != nullptr, mInventory->IsOpened());
			UpdateFright(); 			
			UpdateMouseLook();
			Interact(); 
			UpdateMoving();
			ComputeStealth();
			UpdatePicking();
			UpdateItemsHandling();
			UpdateInventory();
			DrawSheetInHands();			
			UpdateUsableObjects();
			if (mLadder.expired()) { // prevent damaging from ladders
				if (!IsCanJump() && !mInAir) { // in air
					mAirPosition = mBody->GetPosition();
					mInAir = true;
				} else if (IsCanJump() && mInAir) { // landing 
					ruVector3 curPos = mBody->GetPosition();
					float heightDelta = fabsf(curPos.y - mAirPosition.y);
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

void Player::CreateCamera() {
	mHeadHeight = 2.1;

	mHead = ruSceneNode::Create();
	mHead->Attach(mBody);
	mHead->SetPosition(ruVector3(0, -2.0f, 0.0f));
	mpCamera = unique_ptr<GameCamera>(new GameCamera(mHUD->GetScene(), mFov));
	mpCamera->mCamera->Attach(mHead);
	mCameraOffset = ruVector3(0, mHeadHeight, 0);
	mCameraShakeOffset = ruVector3(0, mHeadHeight, 0);

	// Pick
	mPickPoint = ruSceneNode::Create();
	mPickPoint->Attach(mpCamera->mCamera);
	mPickPoint->SetPosition(ruVector3(0, 0, 0.1));

	mItemPoint = ruSceneNode::Create();
	mItemPoint->Attach(mpCamera->mCamera);
	mItemPoint->SetPosition(ruVector3(0, 0, 1.0f));

	mFakeLight = ruPointLight::Create();
	mFakeLight->Attach(mpCamera->mCamera);
	mFakeLight->SetRange(2);
	mFakeLight->SetColor(ruVector3(25, 25, 25));
}

void Player::LoadSounds() {
	mLighterCloseSound = ruSound::Load3D("data/sounds/lighter_close.ogg");
	mLighterOpenSound = ruSound::Load3D("data/sounds/lighter_open.ogg");

	mLighterCloseSound->Attach(mpCamera->mCamera);
	mLighterOpenSound->Attach(mpCamera->mCamera);

	mHeartBeatSound = ruSound::Load2D("data/sounds/heart.ogg");
	mBreathSound = ruSound::Load2D("data/sounds/breath.ogg");

	mHeartBeatSound->SetReferenceDistance(100.0f);
	mBreathSound->SetReferenceDistance(100.0f);

	mBreathVolume = SmoothFloat(0.1f);
	mHeartBeatVolume = SmoothFloat(0.15f);
	mHeartBeatPitch = SmoothFloat(1.0f);
	mBreathPitch = SmoothFloat(1.0f);
}

void Player::DoFright() {
	mBreathVolume.SetTarget(0.1f);
	mBreathVolume.Set(0.25f);
	mHeartBeatVolume.SetTarget(0.15f);
	mHeartBeatVolume.Set(0.45f);
	mHeartBeatPitch.Set(2.0f);
	mHeartBeatPitch.SetTarget(1.0f);
	mBreathPitch.Set(1.5f);
	mBreathPitch.SetTarget(1.0f);
}

void Player::UpdateFright() {
	mBreathVolume.ChaseTarget(0.075f);
	mHeartBeatVolume.ChaseTarget(0.075f);
	mHeartBeatPitch.ChaseTarget(0.0025f);
	mBreathPitch.ChaseTarget(0.0025f);
}

void Player::UpdateCameraShake() {
	static int stepPlayed = 0;

	if (mMoved) {
		const float mBodyWidthDelta = mBodyWidth / 60.0f;
		mStepLength += mRunning ? mBodyWidthDelta * 2.0f : mBodyWidthDelta;


		mCameraBobCoeff += 0.19f * mRunCameraShakeCoeff;

		float yOffset = 0.045f * sinf(mCameraBobCoeff) * (mRunCameraShakeCoeff * mRunCameraShakeCoeff);
		float xOffset = 0.045f * cosf(mCameraBobCoeff / 2) * (mRunCameraShakeCoeff * mRunCameraShakeCoeff);

		if (mStepLength > mBodyWidth / 2.0f) {
			if (!mLadder.expired()) {
				auto ladder = mLadder.lock();
				if (ladder->GetEnterZone()->GetTextureCount() > 0) {
					for (auto & sMat : mSoundMaterialList) {
						shared_ptr<ruSound> & snd = sMat->GetRandomSoundAssociatedWith(ladder->GetEnterZone()->GetTexture(0)->GetName());
						if (snd) {
							snd->Play(true);
						}
					}
				}
			} else {
				EmitStepSound();
			}
			mStepLength = 0.0f;
		}

		mCameraShakeOffset = ruVector3(xOffset, yOffset + mHeadHeight, 0.0f);
	} else {
		mCameraBobCoeff = 0.0f;
	}

	if (mCameraTrembleTime > 0) {
		const float trembleVol = 0.085;
		mCameraTrembleOffset = ruVector3(frandom(-trembleVol, trembleVol), frandom(-trembleVol, trembleVol), frandom(-trembleVol, trembleVol));
		mCameraTrembleTime -= gFixedTick;
	} else {
		mCameraTrembleOffset = ruVector3(0.0f, 0.0f, 0.0f);
	}
	mCameraOffset = mCameraOffset.Lerp(mCameraShakeOffset, 0.25f) + mCameraTrembleOffset;
	mpCamera->mCamera->SetPosition(mCameraOffset);
}

void Player::DrawSheetInHands() {
	if (!mSheetInHands.expired()) {
		auto sheet = mSheetInHands.lock();
		mHUD->SetAction(ruInput::Key::None, StringBuilder() << sheet->GetDescription() << mLocalization.GetString("sheetOpen"));
		if (ruInput::IsMouseHit(ruInput::MouseButton::Right) || (sheet->mObject->GetPosition() - mBody->GetPosition()).Length2() > 2) {
			CloseCurrentSheet();
		}
	}
}


void Player::UpdateItemsHandling() {
	if (!mInventory->IsOpened()) {
		if (mNearestPickedNode) {
			if (IsUseButtonHit()) {
				auto sheet = Level::Current()->FindSheet(mNearestPickedNode->GetName());

				if (mSheetInHands.expired()) {
					if (sheet) {
						mSheetInHands = sheet;
						sheet->SetVisible(true);


						// add note to readed
						mInventory->AddReadedNote(sheet->GetDescription(), sheet->GetText());
					}
				} else {
					CloseCurrentSheet();
				}
			}
		}
	}

	if (mNodeInHands) {
		if (mPitch < 70) {
			ruVector3 ppPos = mItemPoint->GetPosition();
			ruVector3 objectPos = mNodeInHands->GetPosition() + mPickCenterOffset;
			ruVector3 dir = ppPos - objectPos;
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

		ruVector3 ppPos = mPickPoint->GetPosition();
		ruVector3 dir = ppPos - pickPosition;

		auto pIO = Level::Current()->FindInteractiveObject(mPickedNode->GetName());
		auto pSheet = Level::Current()->FindSheet(mPickedNode->GetName());

		if (dir.Length2() < 1.5f) {
			mNearestPickedNode = mPickedNode;
			string pickedObjectDesc;
			if (pIO) {
				mHUD->SetAction(mKeyUse, StringBuilder() << pIO->GetPickDescription() << " - " << mLocalization.GetString("itemPick"));
			} else if (pSheet) {
				mHUD->SetAction(mKeyUse, StringBuilder() << pIO->GetPickDescription() << " - " << mLocalization.GetString("sheetPick"));
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
	int usableObjectCount = mUsableObjectList.size();
	s & usableObjectCount;

	int currentUsableObject = -1;
	int i = 0;
	for (auto uo : mUsableObjectList) {
		if (uo == mCurrentUsableObject) {
			currentUsableObject = i;
		}
		i++;
	}
	s & currentUsableObject;

	if (s.IsLoading()) {
		for (int i = 0; i < usableObjectCount; i++) {
			AddUsableObject(UsableObject::Deserialize(s));
		}
		mCurrentUsableObject = mUsableObjectList[currentUsableObject];
		mCurrentUsableObject->Appear();
	} else {
		for (auto uo : mUsableObjectList) {
			uo->Serialize(s);
		}
	}

	ruVector3 position = mBody->GetPosition();
	string ladderName = mLadder.expired() ? "undefinedWay" : mLadder.lock()->GetEnterZone()->GetName();
	string sheetName = mSheetInHands.expired() ? "undefinedSheet" : mSheetInHands.lock()->mObject->GetName();
	bool collisionEnabled = mBody->IsCollisionEnabled();

	s & ladderName;
	s & sheetName;
	s & collisionEnabled;
	s & position;
	s & mRunCameraShakeCoeff;
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
	s & mCameraBobCoeff;
	s & mCameraOffset;
	s & mCameraShakeOffset;
	s & mHeadHeight;
	s & mObjectThrown;
	s & mDead;
	s & mFrameColor;
	s & mMoved;
	s & mKeyMoveForward;
	s & mKeyMoveBackward;
	s & mKeyStrafeLeft;
	s & mKeyStrafeRight;
	s & mKeyJump;
	s & mKeyFlashLight;
	s & mKeyRun;
	s & mKeyInventory;
	s & mKeyUse;
	s & mStealthMode;
	s & mFlashlightLocked;
	s & mLastHealth;
	s & mCrouch;
	mPitch.Serialize(s);
	mYaw.Serialize(s);
	mBreathVolume.Serialize(s);
	mHeartBeatVolume.Serialize(s);
	mHeartBeatPitch.Serialize(s);
	mBreathPitch.Serialize(s);
	mFov.Serialize(s);
	mInventory->Serialize(s);

	if (s.IsLoading()) {
		mBody->SetCollisionEnabled(collisionEnabled);
		mBody->SetFriction(0);
		mBody->SetPosition(position);
		mBody->SetAngularFactor(ruVector3(0, 0, 0));

		mLadder = Level::Current()->FindLadder(ladderName);
		if (mLadder.use_count()) {
			mBody->Freeze();
		}

		mSheetInHands = Level::Current()->FindSheet(sheetName);
	}
}

void Player::CloseCurrentSheet() {
	if (!mSheetInHands.expired()) {
		mSheetInHands.lock()->SetVisible(false);
		mSheetInHands.reset();
	}
}

Parser * Player::GetLocalization() {
	return &mLocalization;
}

Flashlight * Player::GetFlashLight() {
	for (auto uo : mUsableObjectList) {
		if (typeid(*uo) == typeid(Flashlight)) {
			return dynamic_cast<Flashlight*>(uo);
		}
	}
	return nullptr;
}

Weapon * Player::GetWeapon() {
	for (auto uo : mUsableObjectList) {
		if (typeid(*uo) == typeid(Weapon)) {
			return dynamic_cast<Weapon*>(uo);
		}
	}
	return nullptr;
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

void Player::UpdateUsableObjects() {
	if (mCurrentUsableObject) {
		for (auto & usableObject : mUsableObjectList) {
			usableObject->GetModel()->Hide();
		}
		mCurrentUsableObject->GetModel()->Show();
		if (ruInput::GetMouseWheelSpeed() < 0) {
			mCurrentUsableObject->Prev();
		} else if (ruInput::GetMouseWheelSpeed() > 0) {
			mCurrentUsableObject->Next();
		}
		mCurrentUsableObject->Update();
	}
}

bool Player::IsDead() {
	return mHealth <= 0.0f;
}

void Player::SetPosition(ruVector3 position) {
	Actor::SetPosition(position);
	mAirPosition = mBody->GetPosition(); // prevent death from 'accidental' landing :)
}

void Player::TrembleCamera(float time) {
	mCameraTrembleTime = time;
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

bool Player::AddUsableObject(UsableObject * usObj) {
	bool alreadyGotObjectOfThisType = false;
	UsableObject * existingUsableObject = nullptr;
	for (auto uo : mUsableObjectList) {
		if (typeid(*uo) == typeid(*usObj)) {
			alreadyGotObjectOfThisType = true;
			existingUsableObject = uo;
			break;
		}
	}
	if (alreadyGotObjectOfThisType) {		
		existingUsableObject->OnPickupSame();
	
		delete usObj;

		// object is not added
		return false;
	} else {
		if (mCurrentUsableObject == nullptr) {
			mCurrentUsableObject = usObj;
		}

		// attach to camera
		usObj->GetModel()->Attach(mpCamera->mCamera);


		// register in inventory
		mInventory->AddItem(usObj->GetItemType());

		// link last object with new for correct switching
		if (mUsableObjectList.size() > 0) {
			mUsableObjectList.at(mUsableObjectList.size() - 1)->Link(usObj);
		}

		// add it to list
		mUsableObjectList.push_back(usObj);

		// object added
		return true;
	}
}

void Player::DumpUsableObjects(vector<UsableObject*> & otherPlace) {
	// this method is useful to transfer usable objects between levels
	for (auto uo : mUsableObjectList) {
		uo->GetModel()->Detach();
	}
	otherPlace = mUsableObjectList;
	mUsableObjectList.clear();
}

inline void Player::Interact() {
	for (auto ladder : Level::Current()->GetLadderList()) {
		if (!ladder->IsPlayerInside()) {
			if (ladder->IsEnterPicked()) {
				mHUD->SetAction(mKeyUse, mLocalization.GetString("crawlIn"));
				if (IsUseButtonHit()) {
					ladder->Enter();
					mLadder = ladder;
				}
			}
		}
	}

	for (auto pDoor : Level::Current()->GetDoorList()) {
		pDoor->Update();
		if (mNearestPickedNode == pDoor->GetNode()) {
			if (!pDoor->IsLocked()) {
				mHUD->SetAction(mKeyUse, mLocalization.GetString("openClose"));
				if (IsUseButtonHit()) {
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
	ruVector3 currentPosition = mBody->GetPosition();
	ruVector3 rayBegin = currentPosition;
	ruVector3 rayEnd = rayBegin - ruVector3(0, 5, 0);
	ruVector3 intPoint;
	shared_ptr<ruSceneNode> rayResult = ruPhysics::CastRay(rayBegin, rayEnd, &intPoint);
	ruVector3 pushpullVelocity = ruVector3(0, 0, 0);
	if (rayResult && !(rayResult == mBody)) {
		pushpullVelocity.y = -(currentPosition.y - intPoint.y - mSpringLength * mCrouchMultiplier) * 4.4f;
	}
	mBody->Move(direction * speed + pushpullVelocity);
}

void Player::EmitStepSound() {
	ruRayCastResultEx result = ruPhysics::CastRayEx(mBody->GetPosition() + ruVector3(0, 0.1, 0), mBody->GetPosition() - ruVector3(0, mBodyHeight * 2.2, 0));
	if (result.valid) {
		for (auto & sMat : mSoundMaterialList) {
			shared_ptr<ruSound> & snd = sMat->GetRandomSoundAssociatedWith(result.textureName);
			if (snd) {
				snd->Play(true);
			}
		}
	}
}

