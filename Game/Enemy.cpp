#include "Precompiled.h"
#include "Gate.h"
#include "Enemy.h"
#include "Menu.h"
#include "Door.h"
#include "Level.h"

Enemy::Enemy(unique_ptr<Game> & game, const vector<shared_ptr<GraphVertex>> & path, const vector<shared_ptr<GraphVertex>> & patrol) :
	Actor(game, 0.5f, 0.25f),
	mHitDistance(1.3),
	mCurrentPatrolPoint(0),
	mAngleTo(0.0f),
	mAngle(0.0f),
	mRunSpeed(1.5f),
	mMoveType(MoveType::Patrol),
	mPathCheckTimer(120),
	mCurrentPathPoint(0),
	mLostTimer(0) {
	mPathfinder.SetVertices(path);
	mPatrolPointList = patrol;

	mModel =  mGame->GetEngine()->GetSceneFactory()->LoadScene("data/models/ripper/ripper0.scene");
	mModel->Attach(mBody);
	mModel->SetPosition(ruVector3(0, -0.7f, 0));
	mModel->SetBlurAmount(1.0f);
	mModel->SetAnimationBlendingEnabled(false);

	auto light = dynamic_pointer_cast<ruSpotLight>(mModel->FindChild("Fspot001"));
	light->SetShadowCastEnabled(false);

	FillByNamePattern(mRightLegParts, "RightLegP?([[:digit:]]+)");
	FillByNamePattern(mLeftLegParts, "LeftLegP?([[:digit:]]+)");
	FillByNamePattern(mRightArmParts, "RightArmP?([[:digit:]]+)");
	FillByNamePattern(mLeftArmParts, "LeftArmP?([[:digit:]]+)");
	FillByNamePattern(mTorsoParts, "TorsoBoneP?([[:digit:]]+)");

	mHead = mModel->FindChild("Head");
	mHead->SetAnimationOverride(true);

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

	mRunAnimation = ruAnimation(0, 33, 0.8, true);
	mRunAnimation.AddFrameListener(5, [this] {EmitStepSound(); });
	mRunAnimation.AddFrameListener(23, [this] {EmitStepSound(); });

	mAttackAnimation = ruAnimation(34, 48, 0.78, true);
	mAttackAnimation.AddFrameListener(44, [this] { HitPlayer(); });

	mWalkAnimation = ruAnimation(49, 76, 0.7, true);
	mWalkAnimation.AddFrameListener(51, [this] {EmitStepSound(); });
	mWalkAnimation.AddFrameListener(67, [this] {EmitStepSound(); });

	mIdleAnimation = ruAnimation(77, 85, 1.5, true);

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
	for(auto & pMat : mSoundMaterialList) {
		for(auto & pSound : pMat->GetSoundList()) {
			pSound->SetVolume(0.75f);
			pSound->SetRolloffFactor(35);
			pSound->SetRoomRolloffFactor(35);
			pSound->SetReferenceDistance(5);
		}
	}


	// set callback to let enemy 'hear' sounds and properly react to it
	ruSound::PlayCallback.PlayEvent += [this] { Listen(); };
}

void Enemy::Think() {
	const auto & player = Game::Instance()->GetLevel()->GetPlayer();

	// tweakable parameters
	const float reachDistanceTolerance = 0.7f;
	const float attackPlayerDistance = 3.0f;
	const float runSpeed = 3.1f;
	const float walkSpeed = 1.0f;
	const float doorCheckDistance = 2.8f;
	const float nearAttackPlayerDistance = 1.0f;
	const float detectionDistance = 16.0f * player->mStealthFactor;
	const float pathCheckTolerance = 1.0f;
	const float detectionAngle = 45.0f; // in degress
	const int lostTime = 5 * 60; // in frames
	const bool ignorePlayer = player->IsDead(); // for debug only, force enemy to ignore player
	const int pathCheckTime = 4 * 60; // in frames

	const ruVector3 toPlayer = player->mpCamera->mCamera->GetPosition() - (mHead->GetPosition() + mBody->GetLookVector().Normalize() * 0.4f);
	const ruVector3 toDestination = (mDestination - mBody->GetPosition()).Normalize();

	// select move type (also see Enemy::Listen)
	const bool eyeContact = player->IsVisibleFromPoint(mHead->GetPosition() + mBody->GetLookVector().Normalize() * 0.4f);
	const bool flashLightHighlight = player->mFlashlightEnabled && player->mFlashlight->IsSeePoint(mBody->GetPosition());
	const bool playerInSight = abs(toPlayer.Normalized().Angle(toDestination) * 180.0f / M_PI) <= detectionAngle;
	const float distanceToPlayer = mBody->GetPosition().Distance(player->GetBody()->GetPosition());
	const bool playerCloseEnough = distanceToPlayer <= detectionDistance;
	if(!ignorePlayer && ((eyeContact || flashLightHighlight) && playerInSight && playerCloseEnough)) {
		mMoveType = MoveType::ChasePlayer;
		Game::Instance()->GetLevel()->PlayChaseMusic();
		mLostTimer = lostTime;
	} else if(mLostTimer <= 0) {
		if(mMoveType != MoveType::CheckSound) {
			mMoveType = MoveType::Patrol;
		}
	}

	--mLostTimer;

	// select move speed
	const float moveSpeed = (mMoveType == MoveType::ChasePlayer) ? runSpeed : walkSpeed;

	// select target
	switch(mMoveType) {
	case MoveType::Patrol: mTarget = mPatrolPointList[mCurrentPatrolPoint]->mNode->GetPosition(); break;
	case MoveType::ChasePlayer: mTarget = player->GetCurrentPosition(); break;
	case MoveType::CheckSound: mTarget = mCheckSoundPosition; break;
	}

	// build path to target 
	const auto pathBegin = mPathfinder.GetVertexNearestTo(mBody->GetPosition());
	const auto pathEnd = mPathfinder.GetVertexNearestTo(mTarget);
	if(pathEnd != mTargetVertex) {
		// rebuild path only if target vertex has changed
		mPathfinder.BuildPath(pathBegin, pathEnd, mCurrentPath);
		mTargetVertex = pathEnd;
		mCurrentPathPoint = 0;
	}

	// follow path
	if(!mCurrentPath.empty()) {
		// if close enough to player, then follow him directly, instead of following path points
		if(mCurrentPathPoint == (mCurrentPath.size() - 1) && mMoveType == MoveType::ChasePlayer) {
			mDestination = mTarget;
		} else {
			mDestination = mCurrentPath[mCurrentPathPoint]->mNode->GetPosition();
		}

		LookAt(mDestination);

		// if reach destination
		const ruVector3 destinationVector = mDestination - mBody->GetPosition();
		const float distanceToDestination = destinationVector.Length();
		if(distanceToDestination < reachDistanceTolerance) {
			// switch to next vertex
			++mCurrentPathPoint;
			// if reach end of path
			if(mCurrentPathPoint >= mCurrentPath.size()) {
				mCurrentPathPoint = mCurrentPath.size() - 1;
				if(mMoveType == MoveType::Patrol) {
					// if patrolling, select next patrol point
					++mCurrentPatrolPoint;
					if(mCurrentPatrolPoint >= mPatrolPointList.size()) {
						mCurrentPatrolPoint = 0;
					}
				} else if(mMoveType == MoveType::CheckSound) {
					// sound checked, switch to patrolling
					mMoveType = MoveType::Patrol;
				}

				// stand still on end of the path
				StopInstantly();
				if(mMoveType != MoveType::ChasePlayer) {
					SetIdleAnimation();
				}
			}
		} else {
			// check doors
			bool allDoorsAreOpen = true;
			for(auto & pDoor : Game::Instance()->GetLevel()->GetDoorList()) {
				if((pDoor->mDoorNode->GetPosition() - mBody->GetPosition()).Length() < doorCheckDistance) {
					// ignore locked doors
					if(pDoor->GetState() != Door::State::Opened && !pDoor->IsLocked()) {
						allDoorsAreOpen = false;
					}
					if(pDoor->GetState() == Door::State::Closed && !pDoor->IsLocked()) {
						pDoor->Open();
					}
				}
			}

			// check gates (door too)
			for(auto & pGate : Game::Instance()->GetLevel()->GetGateList()) {
				if((pGate->GetNode()->GetPosition() - mBody->GetPosition()).Length() < doorCheckDistance) {
					if(pGate->GetState() == Gate::State::Closed && !pGate->mLocked) {
						pGate->Open();
					}
				}
			}

			// check for stuck when patrolling (i.e. because of locked doors)
			if(mMoveType == MoveType::Patrol) {
				--mPathCheckTimer;
				if(mPathCheckTimer <= 0) {
					if(mLastCheckPosition.Distance(mBody->GetPosition()) < pathCheckTolerance) {
						// select next patrol point
						++mCurrentPatrolPoint;
						if(mCurrentPatrolPoint >= mPatrolPointList.size()) {
							mCurrentPatrolPoint = 0;
						}
					}
					mPathCheckTimer = pathCheckTime;
					mLastCheckPosition = mBody->GetPosition();
				}
			}

			bool allowMovement = allDoorsAreOpen;
			// select animation type by distance to player
			if(mMoveType == MoveType::ChasePlayer) {
				if(distanceToPlayer < nearAttackPlayerDistance) {
					allowMovement = false;
					SetStayAndAttackAnimation();
				} else if(distanceToPlayer >= nearAttackPlayerDistance && distanceToPlayer <= attackPlayerDistance) {
					SetRunAndAttackAnimation();
				} else {
					SetRunAnimation();
				}
			} else {
				// just walk
				SetWalkAnimation();
			}

			if(allowMovement) {
				Move(destinationVector.Normalized(), moveSpeed);
			} else {
				if(mMoveType != MoveType::ChasePlayer) {
					SetIdleAnimation();
				}
				StopInstantly();
			}
		}
	}

	// update animations
	mAttackAnimation.Update();
	mIdleAnimation.Update();
	mRunAnimation.Update();
	mWalkAnimation.Update();
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
	for(auto & torsoPart : mTorsoParts) {
		torsoPart->SetAnimation(anim);
	}
}

void Enemy::SetLegsAnimation(ruAnimation *pAnim) {
	for(auto & rightLegPart : mRightLegParts) {
		rightLegPart->SetAnimation(pAnim);
	}
	for(auto & leftLegPart : mLeftLegParts) {
		leftLegPart->SetAnimation(pAnim);
	}
}

void Enemy::HitPlayer() {
	auto & player = Game::Instance()->GetLevel()->GetPlayer();
	float distanceToPlayer = (player->GetCurrentPosition() - mBody->GetPosition()).Length();
	if(distanceToPlayer < mHitDistance) {
		player->Damage(55.0f);
		mHitFleshWithAxeSound->Play(true);
	}
}

void Enemy::EmitStepSound() {
	ruRayCastResultEx result = mGame->GetEngine()->GetPhysics()->CastRayEx(mBody->GetPosition() + ruVector3(0, 0.1, 0), mBody->GetPosition() - ruVector3(0, mBodyHeight * 2.2, 0));
	if(result.valid) {
		for(auto & sMat : mSoundMaterialList) {
			shared_ptr<ruSound> & snd = sMat->GetRandomSoundAssociatedWith(result.textureName);
			if(snd) {
				snd->Play(true);
			}
		}
	}
}

void Enemy::FillByNamePattern(vector< shared_ptr<ruSceneNode> > & container, const string & pattern) {
	std::regex rx(pattern);
	for(int i = 0; i < mModel->GetCountChildren(); i++) {
		shared_ptr<ruSceneNode> child = mModel->GetChild(i);
		if(regex_match(child->GetName(), rx)) {
			container.push_back(child);
		}
	}
}

void Enemy::Listen() {
	// if hear something before, then check it and only then check new sound
	if(mMoveType != MoveType::ChasePlayer) {
		const float hearDistance = Game::Instance()->GetLevel()->GetPlayer()->mNoiseFactor * 10.0f;
		const auto sound = ruSound::PlayCallback.Caller;
		for(const auto & reactSound : mReactSounds) {
			if(sound == reactSound) {
				if(sound->GetPosition().Distance(mBody->GetPosition()) < hearDistance) {
					mCheckSoundPosition = sound->GetPosition();
					if(mMoveType != MoveType::ChasePlayer) {
						mMoveType = MoveType::CheckSound;
					}
				}
			}
		}
	}
}


void Enemy::Serialize(SaveFile & s) {
	ruVector3 position = mBody->GetPosition();

	s & position;
	s & mHealth;
	s & mLostTimer;
	s & mPathCheckTimer;

	mBody->SetPosition(position);
}

Enemy::~Enemy() {
	ruSound::PlayCallback.PlayEvent.Clear();
}

void Enemy::Damage(float dmg) {
	// invulnerable
}

inline float RadToDeg(float rad) {
	return rad * 180.0f / M_PI;
}

void Enemy::LookAt(const ruVector3 & lookAt) {
	ruVector3 delta = (lookAt - mBody->GetPosition()).Normalize();

	mAngleTo = RadToDeg(atan2(delta.x, delta.z));

	mAngleTo = mAngleTo > 0 ? mAngleTo : (360.0f + mAngleTo);
	mAngle = mAngle > 0 ? mAngle : (360.0f + mAngle);

	mAngleTo = fmod(mAngleTo, 360.0f);
	mAngle = fmod(mAngle, 360.0f);

	if((int)mAngleTo != (int)mAngle) {
		float change = 0;
		float diff = mAngle - mAngleTo;
		if(diff < 0) {
			change = 1;
		} else {
			change = -1;
		}

		if(fabs(diff) > 180) {
			change = -change;
		}

		mAngle += change * 2;
	}

	mBody->SetRotation(ruQuaternion(ruVector3(0, 1, 0), mAngle - 90.0f));

	// head
	//float headAngle = RadToDeg(atan2(delta.y, delta.z));
	//mHead->SetRotation(ruQuaternion(ruVector3(1, 0, 0), headAngle));
}

void Enemy::SetNextPatrolPoint() {
	mCurrentPatrolPoint++;
	if(mCurrentPatrolPoint >= mPatrolPointList.size()) {
		mCurrentPatrolPoint = 0;
	}
}

void Enemy::ForceCheckSound(ruVector3 position) {
	mCheckSoundPosition = position;
	mMoveType = MoveType::CheckSound;
}

shared_ptr<ruSceneNode> Enemy::GetBody() {
	return mBody;
}
