#include "Precompiled.h"
#include "Gate.h"
#include "Enemy.h"
#include "Menu.h"
#include "Door.h"
#include "Level.h"

Enemy::Enemy(vector<GraphVertex*> & path, vector<GraphVertex*> & patrol) :
	Actor(0.5f, 0.25f),
	mHitDistance(1.3),
	mCurrentPatrolPoint(0),
	mAngleTo(0.0f),
	mAngle(0.0f),
	mRunSpeed(1.5f),
	mMoveType(MoveType::GoToDestination),
	mDestinationWaypointNum(0),
	mLastDestinationIndex(-1),
	mPlayerDetected(false),
	mDead(false) {
	mPathfinder.SetVertices(path);
	mPatrolPointList = patrol;

	mModel = ruSceneNode::LoadFromFile("data/models/ripper/ripper0.scene");
	mModel->Attach(mBody);
	mModel->SetPosition(ruVector3(0, -0.7f, 0));
	mModel->SetBlurAmount(1.0f);

	FindBodyparts();
	CreateSounds();
	CreateAnimations();

	mPlayerInSightTimer = ruTimer::Create();
	mResurrectTimer = ruTimer::Create();
	mPathCheckTimer = ruTimer::Create();

	mFadeAwaySound = ruSound::Load2D("data/sounds/fadeaway.ogg");
	mFadeAwaySound->SetVolume(1.0f);

	mSoundMaterialList.push_back(make_unique<SoundMaterial>("data/materials/stone.smat", mBody));
	mSoundMaterialList.push_back(make_unique<SoundMaterial>("data/materials/metal.smat", mBody));
	mSoundMaterialList.push_back(make_unique<SoundMaterial>("data/materials/wood.smat", mBody));
	mSoundMaterialList.push_back(make_unique<SoundMaterial>("data/materials/gravel.smat", mBody));
	mSoundMaterialList.push_back(make_unique<SoundMaterial>("data/materials/muddyrock.smat", mBody));
	mSoundMaterialList.push_back(make_unique<SoundMaterial>("data/materials/rock.smat", mBody));
	mSoundMaterialList.push_back(make_unique<SoundMaterial>("data/materials/grass.smat", mBody));
	mSoundMaterialList.push_back(make_unique<SoundMaterial>("data/materials/soil.smat", mBody));
	mSoundMaterialList.push_back(make_unique<SoundMaterial>("data/materials/chain.smat", mBody));
	mSoundMaterialList.push_back(make_unique<SoundMaterial>("data/materials/mud.smat", mBody));
	for (auto & pMat : mSoundMaterialList) {
		for (auto & pSound : pMat->GetSoundList()) {
			pSound->SetVolume(0.75f);
			pSound->SetRolloffFactor(35);
			pSound->SetRoomRolloffFactor(35);
			pSound->SetReferenceDistance(5);
		}
	}

	// set callback to let enemy 'hear' sounds and properly react to it
	ruSound::PlayCallback.PlayEvent += [this] { Listen(); };
}

// grim code ahead
void Enemy::Think() {
	if (pMainMenu->IsVisible() || mHealth <= 0.0f) {
		mScreamSound->Pause();
		mBreathSound->Pause();
		mHitFleshWithAxeSound->Pause();

		if (mDead) {
			if (mResurrectTimer->GetElapsedTimeInSeconds() >= 10) {
				Resurrect();
			}
		}

		return;
	}

	auto & player = Level::Current()->GetPlayer();

	if (mMoveType == MoveType::ChasePlayer) {
		mTarget = player->GetCurrentPosition();
	} else if (mMoveType == MoveType::GoToDestination) {
		if (mCurrentPath.size()) {
			mTarget = mCurrentPath[mCurrentWaypointNum]->mPosition;
			if ((mTarget - mBody->GetPosition()).Length2() < 0.25f) {
				mCurrentWaypointNum += mDestinationWaypointNum - mCurrentWaypointNum > 0 ? 1 : 0;
			}
		}
	}



	if (mDoPatrol) {
		mMoveType = MoveType::GoToDestination;
		if (mDestinationWaypointNum - mCurrentWaypointNum == 0) {
			SetNextPatrolPoint();
		}
		mDestination = mPatrolPointList[mCurrentPatrolPoint]->mPosition;
		mTargetIsPlayer = false;
	} else {
		mDestination = player->GetCurrentPosition();
		mTargetIsPlayer = true;
	}

	ruVector3 direction = mTarget - mBody->GetPosition();

	float distanceToPlayer = (player->GetCurrentPosition() - mBody->GetPosition()).Length();
	float distanceToTarget = direction.Length();
	direction.Normalize();

	LookAt(mTarget);

	bool move = true;

	ruVector3 toPlayer = player->mpCamera->mCamera->GetPosition() - (mHead->GetPosition() + mBody->GetLookVector().Normalize() * 0.4f);
	bool playerInView = player->IsVisibleFromPoint(mHead->GetPosition() + mBody->GetLookVector().Normalize() * 0.4f);
	float angleToPlayer = abs(toPlayer.Angle(direction) * 180.0f / M_PI);

	bool enemyDetectPlayer = false;
	if (playerInView) {
		if (player->mFlashlightEnabled) {
			// if we light up enemy, he detects player
			if (player->mFlashlight->IsSeePoint(mBody->GetPosition())) {
				if (!mPlayerDetected) {
					mPlayerInSightTimer->Restart();
					mPlayerDetected = true;
				}
			}
		}


		float detectDistance = player->mStealthFactor * 10.0f;
		// player right in front of enemy
		if ((distanceToPlayer < detectDistance) && (angleToPlayer < 45)) {
			if (!mPlayerDetected) {
				mPlayerInSightTimer->Restart();
				mPlayerDetected = true;
			}
		}

		// enemy doesn't see player, but can hear he, if he moved
		if ((player->mStealthFactor >= 0.3f && player->mMoved && (distanceToPlayer < 5.0f))) {
			if (!mPlayerDetected) {
				mPlayerInSightTimer->Restart();
				mPlayerDetected = true;
			}
		}

	}
	// if player too close to the enemy, he detects player
	if (angleToPlayer < 45 && distanceToPlayer < 2.0f) {
		if (!mPlayerDetected) {
			mPlayerInSightTimer->Restart();
			mPlayerDetected = true;
		}
	}

	if (mPlayerDetected) {
		enemyDetectPlayer = true;
		if (mPlayerInSightTimer->GetElapsedTimeInSeconds() > 10.0f) {
			mPlayerDetected = false;
		}
	}



	// DEBUG
	enemyDetectPlayer = false;

	if (enemyDetectPlayer) {
		mMoveType = MoveType::ChasePlayer;
		mDoPatrol = false;
		mBreathSound->Pause();
		mScreamSound->Play(true);
		mRunSpeed = 2.8f;
		Level::Current()->PlayChaseMusic();
	} else {
		mMoveType = MoveType::GoToDestination;
		mDoPatrol = true;
		mRunSpeed = 2.5f;
		mBreathSound->Play(true);
		mScreamSound->Pause();
	}

	if (mMoveType == MoveType::ChasePlayer) {
		bool playerTooFar = mTargetIsPlayer && distanceToTarget > 10.0f;
		if (playerTooFar || player->mDead) {
			mDoPatrol = true;
			mMoveType = MoveType::GoToDestination;
			SetIdleAnimation();
		} else {
			if (!player->mDead) {
				if (distanceToPlayer < 2.5f) {
					if (distanceToPlayer < mHitDistance) {
						move = false;
						if (!player->mMoved) {
							SetStayAndAttackAnimation();
						}
					} else {
						SetRunAndAttackAnimation();
					}
				} else {
					SetRunAnimation();
				}
			}
		}
	} else if (mMoveType == MoveType::GoToDestination) {
		GraphVertex * destNearestVertex = mPathfinder.GetVertexNearestTo(mDestination, &mCurrentDestinationIndex);
		GraphVertex * enemyNearestVertex = mPathfinder.GetVertexNearestTo(mBody->GetPosition());

		// means player has moved to another waypoint
		if (mCurrentDestinationIndex != mLastDestinationIndex) {
			mPathfinder.BuildPath(enemyNearestVertex, destNearestVertex, mCurrentPath);
			mDestinationWaypointNum = GetVertexIndexNearestTo(mCurrentPath.back()->mPosition);
			mCurrentWaypointNum = GetVertexIndexNearestTo(mCurrentPath.front()->mPosition);
			// go back
			if (mCurrentWaypointNum > mDestinationWaypointNum) {
				std::swap(mCurrentWaypointNum, mDestinationWaypointNum);
			}

			mLastDestinationIndex = mCurrentDestinationIndex;
		}
		//SetWalkAnimation();
		SetRunAnimation();
	}

	const float doorCheckDistance = 7.0f;
	// check doors
	bool allDoorsAreOpen = true;
	for (auto pDoor : Level::Current()->GetDoorList()) {
		if ((pDoor->mDoorNode->GetPosition() - mBody->GetPosition()).Length2() < doorCheckDistance) {
			if (pDoor->GetState() != Door::State::Opened && !pDoor->IsLocked()) {
				allDoorsAreOpen = false;
			}
			if (pDoor->GetState() == Door::State::Closed && !pDoor->IsLocked()) {
				pDoor->Open();
			}
		}
	}

	// check gates (door too)
	for (auto pGate : Level::Current()->GetGateList()) {
		if ((pGate->GetNode()->GetPosition() - mBody->GetPosition()).Length2() < doorCheckDistance) {
			if (pGate->GetState() != Gate::State::Opened) {
				allDoorsAreOpen = false;
			}
			if (pGate->GetState() == Gate::State::Closed) {
				pGate->Open();
			}
		}
	}

	bool reachDestination = (mDestinationWaypointNum - mCurrentWaypointNum) < 0;
	if (move && !reachDestination && allDoorsAreOpen) {
		ruVector3 gravity;
		if (direction.y > 0.1) {
			gravity = ruVector3(0, 0, 0);
		} else {
			gravity = ruVector3(0, -0.1, 0);
		}
		ruVector3 speedVector = direction * mRunSpeed + gravity;
		mBody->Move(speedVector);
	}

	if (!move) {
		StopInstant();
	}

	// wait while all doors nearby (exculuding locked) becomes open
	if (!allDoorsAreOpen) {
		StopInstant();
		SetIdleAnimation();
	}


	if (mPathCheckTimer->GetElapsedTimeInSeconds() > 2.0f) {
		// got obstacle (locked door, solid object and etc.), can't get throuh it, try next patrol point
		if ((mModel->GetPosition() - mLastCheckPosition).Length2() < 0.055) {
			SetNextPatrolPoint();
		}
		mLastCheckPosition = mModel->GetPosition();

		mPathCheckTimer->Restart();
	}

	mIdleAnimation.SetEnabled(true);

	mAttackAnimation.Update();
	mIdleAnimation.Update();
	mRunAnimation.Update();
	mWalkAnimation.Update();
}

void Enemy::Resurrect() {
	mDead = false;
	mBody->SetPosition(mDeathPosition);
	mBody->Unfreeze();
	mBody->Show();

	DoBloodSpray();
	mHealth = 100;
}

void Enemy::SetWalkAnimation() {
	SetCommonAnimation(&mWalkAnimation);
	mRunAnimation.SetEnabled(false);
	mWalkAnimation.SetEnabled(true);
	mAttackAnimation.SetEnabled(false);
}

void Enemy::SetStayAndAttackAnimation() {
	SetCommonAnimation(&mIdleAnimation);
	SetTorsoAnimation(&mAttackAnimation);
	mAttackAnimation.SetEnabled(true);
}

void Enemy::SetRunAndAttackAnimation() {
	SetCommonAnimation(&mAttackAnimation);
	SetLegsAnimation(&mRunAnimation);
	mRunAnimation.SetEnabled(true);
	mWalkAnimation.SetEnabled(false);
	mAttackAnimation.SetEnabled(true);
}

void Enemy::SetRunAnimation() {
	SetCommonAnimation(&mRunAnimation);
	mRunAnimation.SetEnabled(true);
	mWalkAnimation.SetEnabled(false);
	mAttackAnimation.SetEnabled(false);
}

void Enemy::SetIdleAnimation() {
	SetCommonAnimation(&mIdleAnimation);
	mAttackAnimation.SetEnabled(false);
}

void Enemy::SetCommonAnimation(ruAnimation * anim) {
	mModel->SetAnimation(anim);
}

void Enemy::SetTorsoAnimation(ruAnimation * anim) {
	for (auto & torsoPart : mTorsoParts) {
		torsoPart->SetAnimation(anim);
	}
}

void Enemy::SetLegsAnimation(ruAnimation *pAnim) {
	for (auto & rightLegPart : mRightLegParts) {
		rightLegPart->SetAnimation(pAnim);
	}
	for (auto & leftLegPart : mLeftLegParts) {
		leftLegPart->SetAnimation(pAnim);
	}
}

void Enemy::CreateAnimations() {
	mRunAnimation = ruAnimation(0, 33, 0.8, true);
	mRunAnimation.AddFrameListener(5, [this] {EmitStepSound(); });
	mRunAnimation.AddFrameListener(23, [this] {EmitStepSound(); });

	mAttackAnimation = ruAnimation(34, 48, 0.78, true);
	mAttackAnimation.AddFrameListener(44, [this] { HitPlayer(); });

	mWalkAnimation = ruAnimation(49, 76, 1, true);
	mWalkAnimation.AddFrameListener(51, [this] {EmitStepSound(); });
	mWalkAnimation.AddFrameListener(67, [this] {EmitStepSound(); });

	mIdleAnimation = ruAnimation(77, 85, 1.5, true);
}

void Enemy::CreateSounds() {
	mHitFleshWithAxeSound = ruSound::Load3D("data/sounds/armor_axe_flesh.ogg");
	mHitFleshWithAxeSound->Attach(mModel->FindChild("Weapon"));

	mBreathSound = ruSound::Load3D("data/sounds/breath1.ogg");
	mBreathSound->Attach(mBody);
	mBreathSound->SetVolume(0.25f);
	mBreathSound->SetRolloffFactor(35);
	mBreathSound->SetRoomRolloffFactor(35);
	mBreathSound->SetReferenceDistance(2.8);

	mScreamSound = ruSound::Load3D("data/sounds/scream_creepy_1.ogg");
	mScreamSound->SetVolume(1.0f);
	mScreamSound->Attach(mBody);
	mScreamSound->SetRolloffFactor(20);
	mScreamSound->SetRoomRolloffFactor(20);
	mScreamSound->SetReferenceDistance(4);
}

int Enemy::GetVertexIndexNearestTo(ruVector3 position) {
	if (mCurrentPath.size() == 0) {
		return 0;
	}
	int nearestIndex = 0;
	for (int i = 0; i < mCurrentPath.size(); i++) {
		if ((mCurrentPath[i]->mPosition - position).Length2() < (mCurrentPath[nearestIndex]->mPosition - position).Length2()) {
			nearestIndex = i;
		}
	}
	return nearestIndex;
}

inline void Enemy::HitPlayer() {
	auto & player = Level::Current()->GetPlayer();
	float distanceToPlayer = (player->GetCurrentPosition() - mBody->GetPosition()).Length();
	if (distanceToPlayer < mHitDistance) {

		//DEBUG <<<<<<<<<<<<<<<<<<<<<<<<<<<
		player->Damage(20);

		mHitFleshWithAxeSound->Play(true);
	}
}


// called from animation frames

inline void Enemy::EmitStepSound() {
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

void Enemy::FillByNamePattern(vector< shared_ptr<ruSceneNode> > & container, const string & pattern) {
	std::regex rx(pattern);
	for (int i = 0; i < mModel->GetCountChildren(); i++) {
		shared_ptr<ruSceneNode> child = mModel->GetChild(i);
		if (regex_match(child->GetName(), rx)) {
			container.push_back(child);
		}
	}
}

void Enemy::FindBodyparts() {
	FillByNamePattern(mRightLegParts, "RightLegP?([[:digit:]]+)");
	FillByNamePattern(mLeftLegParts, "LeftLegP?([[:digit:]]+)");
	FillByNamePattern(mRightArmParts, "RightArmP?([[:digit:]]+)");
	FillByNamePattern(mLeftArmParts, "LeftArmP?([[:digit:]]+)");
	FillByNamePattern(mTorsoParts, "TorsoBoneP?([[:digit:]]+)");
	mHead = mModel->FindChild("Head");
}

void Enemy::Serialize(SaveFile & s) {
	ruVector3 position = mBody->GetPosition();

	s & mDeathPosition;
	s & position;
	s & mDead;
	s & mHealth;

	mBody->SetPosition(position);
}

Enemy::~Enemy() {

}

void Enemy::DoBloodSpray() {
	mBloodSpray = ruParticleSystem::Create(50);
	mBloodSpray->SetTexture(ruTexture::Request("data/textures/particles/spray.png"));
	mBloodSpray->SetType(ruParticleSystem::Type::Box);
	mBloodSpray->SetSpeedDeviation(ruVector3(-0.0015, 0.02, -0.0015), ruVector3(0.0015, -0.09, 0.0015));
	mBloodSpray->SetColorRange(ruVector3(200, 0, 0), ruVector3(200, 0, 0));
	mBloodSpray->SetPointSize(0.455f);
	mBloodSpray->SetBoundingBox(ruVector3(-mBodyWidth, 0.0, -mBodyWidth), ruVector3(mBodyWidth, mBodyHeight, mBodyWidth));
	mBloodSpray->SetParticleThickness(20.5f);
	mBloodSpray->SetAutoResurrection(false);
	mBloodSpray->SetLightingEnabled(true);
	mBloodSpray->SetPosition(mBody->GetPosition());
}

void Enemy::Damage(float dmg) {
	Actor::Damage(dmg);
	if (mHealth <= 0.0f) {
		mResurrectTimer->Restart();
		if (!mDead) {
			DoBloodSpray();
			mFadeAwaySound->Play();
			mDead = true;
		}
		mDeathPosition = mBody->GetPosition();
		mBody->SetPosition(ruVector3(1000, 1000, 1000));
		mBody->Hide();
		mBody->Freeze();
	}
}

void Enemy::LookAt(const ruVector3 & lookAt) {
	ruVector3 delta = (lookAt - mBody->GetPosition()).Normalize();

	mAngleTo = atan2(delta.x, delta.z) * 180.0f / M_PI;

	mAngleTo = mAngleTo > 0 ? mAngleTo : (360.0f + mAngleTo);
	mAngle = mAngle > 0 ? mAngle : (360.0f + mAngle);

	mAngleTo = fmod(mAngleTo, 360.0f);
	mAngle = fmod(mAngle, 360.0f);

	if ((int)mAngleTo != (int)mAngle) {
		float change = 0;
		float diff = mAngle - mAngleTo;
		if (diff < 0) {
			change = 1;
		} else {
			change = -1;
		}

		if (fabs(diff) > 180) {
			change = -change;
		}

		mAngle += change * 2;
	}

	mBody->SetRotation(ruQuaternion(ruVector3(0, 1, 0), mAngle - 90.0f));
}

void Enemy::SetNextPatrolPoint() {
	mCurrentPatrolPoint++;
	if (mCurrentPatrolPoint >= mPatrolPointList.size()) {
		mCurrentPatrolPoint = 0;
	}
}

shared_ptr<ruSceneNode> Enemy::GetBody() {
	return mBody;
}
