#include "Precompiled.h"

#include "Enemy.h"
#include "Menu.h"
#include "Door.h"

vector<Enemy*> Enemy::msEnemyList;

Enemy::Enemy( vector<GraphVertex*> & path, vector<GraphVertex*> & patrol ) : Actor( 0.5f, 0.25f ), mHitDistance( 1.3 ) {
	mPathfinder.SetVertices( path );
	mPatrolPointList = patrol;
	mCurrentPatrolPoint = 0;

	mHead = nullptr;
	mBloodSpray = nullptr;

	mStepLength = 0.0f;
	mLastStepLength = 0.0f;

	mModel = ruSceneNode::LoadFromFile( "data/models/ripper/ripper0.scene" );
	mModel->Attach( mBody );
	mModel->SetPosition( ruVector3( 0, -0.7f, 0 ));
	mModel->SetBlurAmount( 1.0f );

	FindBodyparts();

	mAngleTo = 0.0f;
	mAngle = 0.0f;

	CreateSounds();

	CreateAnimations();

	mRunSpeed = 1.5f;

	mMoveType = MoveType::GoToDestination;

	mDestinationWaypointNum = 0;
	mLastDestinationIndex = -1;
	mPlayerDetected = false;
	mPlayerInSightTimer = ruTimer::Create();

	msEnemyList.push_back( this );

	mDead = false;

	mFadeAwaySound = ruSound::Load2D( "data/sounds/fadeaway.ogg" );
	mFadeAwaySound.SetVolume( 1.0f );
	mResurrectTimer = ruTimer::Create();

	mPathCheckTimer = ruTimer::Create();
}


void Enemy::Think() {
    if( pMainMenu->IsVisible() || mHealth <= 0.0f ) {
		mScreamSound.Pause( );
		mBreathSound.Pause();
		mHitFleshWithAxeSound.Pause();

		if( mDead ) {
			if( mResurrectTimer->GetElapsedTimeInSeconds() >= 10 ) {
				Resurrect();
			}
		}

        return;
    }

	if( mMoveType == MoveType::ChasePlayer ) {
		mTarget = pPlayer->GetCurrentPosition();
	} else if( mMoveType == MoveType::GoToDestination ) {
		if( mCurrentPath.size() ) {
			mTarget = mCurrentPath[mCurrentWaypointNum]->mPosition;
			if( ( mTarget - mBody->GetPosition() ).Length2() < 0.5f ) {
				mCurrentWaypointNum += mDestinationWaypointNum - mCurrentWaypointNum > 0 ? 1 : 0;
			}
		}
	}

	bool reachPoint = ( mDestinationWaypointNum - mCurrentWaypointNum ) < 0 ;

	if( mDoPatrol ) {
		mMoveType = MoveType::GoToDestination ;
		if( mDestinationWaypointNum - mCurrentWaypointNum == 0 ) {
			SetNextPatrolPoint();
		}
		mDestination = mPatrolPointList[mCurrentPatrolPoint]->mPosition;
		mTargetIsPlayer = false;
	} else {
		mDestination = pPlayer->GetCurrentPosition();
		mTargetIsPlayer = true;
	}

	ruVector3 direction = mTarget - mBody->GetPosition();

	float distanceToPlayer = ( pPlayer->GetCurrentPosition() - mBody->GetPosition() ).Length();
	float distanceToTarget = direction.Length();
	direction.Normalize();

	mAngleTo = atan2f( direction.x, direction.z ) - M_PI / 2;
	mAngleTo = (mAngleTo > 0 ? mAngleTo : (2*M_PI + mAngleTo)) * 360 / (2*M_PI);



	mBody->SetRotation( ruQuaternion( 0, mAngle, 0 ));

	bool move = true;
	bool playerTooFar = mTargetIsPlayer && distanceToTarget > 10.0f;
	ruVector3 toPlayer = pPlayer->mpCamera->mCamera->GetPosition() - ( mHead->GetPosition() + mBody->GetLookVector().Normalize() * 0.4f);
	bool playerInView = pPlayer->IsVisibleFromPoint( mHead->GetPosition() + mBody->GetLookVector().Normalize() * 0.4f );
	float angleToPlayer = abs( toPlayer.Angle( direction ) * 180.0f / M_PI );

	bool enemyDetectPlayer = false;
	if( playerInView ) {
		Flashlight * flashLight = pPlayer->GetFlashLight();
		if( flashLight ) {
			if( flashLight->IsOn() ) {
				// if we light up enemy, he detects player
				if( flashLight->IsBeamContainsPoint( mBody->GetPosition() )) {
					if( !mPlayerDetected ) {
						mPlayerInSightTimer->Restart();
						mPlayerDetected = true;
					}
				}
			}
		}
		float detectDistance = pPlayer->mStealthFactor * 10.0f;
		// player right in front of enemy
		if( ( distanceToPlayer < detectDistance ) && ( angleToPlayer < 45 ) ) {
			if( !mPlayerDetected ) {
				mPlayerInSightTimer->Restart();
				mPlayerDetected = true;
			}
		}
		// enemy doesn't see player, but can hear he, if he moved
		if(( pPlayer->mStealthFactor >= 0.3f && pPlayer->mMoved && ( distanceToPlayer < 5.0f ))) {
			if( !mPlayerDetected ) {
				mPlayerInSightTimer->Restart();
				mPlayerDetected = true;
			}
		}

	}
	// if player too close to the enemy, he detects player
	if( angleToPlayer < 45 && distanceToPlayer < 2.0f ) {
		if( !mPlayerDetected ) {
			mPlayerInSightTimer->Restart();
			mPlayerDetected = true;
		}
	}

	if( mPlayerDetected ) {
		enemyDetectPlayer = true;
		if( mPlayerInSightTimer->GetElapsedTimeInSeconds() > 2.5f ) {
			mPlayerDetected = false;
		}
	}



	// DEBUG
	enemyDetectPlayer = false;

	if( enemyDetectPlayer ) {
		mMoveType = MoveType::ChasePlayer;
		mDoPatrol = false;
		mBreathSound.Pause();
		mScreamSound.Play( true );
		mRunSpeed = 3.1f;
	} else {
		mMoveType = MoveType::GoToDestination;
		mDoPatrol = true;
		mRunSpeed = 2.5f;
		mBreathSound.Play( true );
		mScreamSound.Pause();
	}

	if( enemyDetectPlayer ) {	
		mAngle = mAngleTo;
	} else {
		mAngle += ( mAngleTo - mAngle ) * 0.05f;
	}
	if( mMoveType == MoveType::ChasePlayer ) {
		if( playerTooFar || pPlayer->mDead ) {
			mDoPatrol = true;
			mMoveType = MoveType::GoToDestination;
			SetIdleAnimation();
		} else {
			if( !pPlayer->mDead ) {
				if( distanceToPlayer < 2.5f ) {
					if( distanceToPlayer < mHitDistance ) {
						move = false;
						if( !pPlayer->mMoved ) {
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
	} else if( mMoveType == MoveType::GoToDestination ) {
		GraphVertex * destNearestVertex = mPathfinder.GetVertexNearestTo( mDestination, &mCurrentDestinationIndex);
		GraphVertex * enemyNearestVertex = mPathfinder.GetVertexNearestTo( mBody->GetPosition() );
		if( mCurrentDestinationIndex != mLastDestinationIndex ) { // means player has moved to another waypoint
			mPathfinder.BuildPath( enemyNearestVertex, destNearestVertex, mCurrentPath );
			mDestinationWaypointNum = GetVertexIndexNearestTo( mCurrentPath.back()->mPosition );
			mCurrentWaypointNum = GetVertexIndexNearestTo( mCurrentPath.front()->mPosition );
			// go back
			if( mCurrentWaypointNum > mDestinationWaypointNum ) {
				int temp = mCurrentWaypointNum;
				mCurrentWaypointNum = mDestinationWaypointNum;
				mDestinationWaypointNum = temp;
			}
				
			mLastDestinationIndex = mCurrentDestinationIndex;
		}
		//SetWalkAnimation();
		SetRunAnimation();
	}

	// check doors
	bool allDoorsAreOpen = true;
	for( auto pDoor : Door::msDoorList ) {
		if( ( pDoor->mDoorNode->GetPosition() - mBody->GetPosition()).Length2() < 2.2f ) {
			if( pDoor->GetState() != Door::State::Opened && !pDoor->IsLocked() ) {
				allDoorsAreOpen = false;
			}
			if( pDoor->GetState() == Door::State::Closed && !pDoor->IsLocked() ) {
				pDoor->Open();
			}
		}
	}

	if( move && !reachPoint && allDoorsAreOpen ) {
		mStepLength += 0.1f;
		ruVector3 gravity;
		if( direction.y > 0.1 ) {
			gravity = ruVector3( 0,0,0 );
		} else {
			gravity = ruVector3( 0, -0.1, 0);
		}
		ruVector3 speedVector = direction * mRunSpeed + gravity;
		mBody->Move( speedVector );
	}

	if( !move ) {
		StopInstant();
	}
	if( mPathCheckTimer->GetElapsedTimeInSeconds() > 2.5f ) {
		// got obstacle (door), can't get throuh it, try next patrol point
		if( ( mModel->GetPosition() - mLastCheckPosition).Length2() < 0.055 ) {
			SetNextPatrolPoint( );
		}
		mLastCheckPosition = mModel->GetPosition();

		mPathCheckTimer->Restart();
	}

	mIdleAnimation.enabled = true;

	mAttackAnimation.Update();
	mIdleAnimation.Update();
	mRunAnimation.Update();
	mWalkAnimation.Update();
}

void Enemy::Resurrect() {
	mDead = false;
	mBody->SetPosition( mDeathPosition );
	mBody->Unfreeze();
	mBody->Show();
	
	DoBloodSpray();
	mHealth = 100;
}

void Enemy::SetWalkAnimation() {
    SetCommonAnimation( &mWalkAnimation );
	mRunAnimation.enabled = false;
	mWalkAnimation.enabled = true;
	mAttackAnimation.enabled = false;
}


void Enemy::SetStayAndAttackAnimation() {
    SetCommonAnimation( &mIdleAnimation );
    SetTorsoAnimation( &mAttackAnimation );
	mAttackAnimation.enabled = true;
}

void Enemy::SetRunAndAttackAnimation() {
    SetCommonAnimation( &mAttackAnimation );
    SetLegsAnimation( &mRunAnimation );
	mRunAnimation.enabled = true;
	mWalkAnimation.enabled = false;
	mAttackAnimation.enabled = true;
}

void Enemy::SetRunAnimation() {
    SetCommonAnimation( &mRunAnimation );
	mRunAnimation.enabled = true;
	mWalkAnimation.enabled = false;
	mAttackAnimation.enabled = false;
}

void Enemy::SetIdleAnimation() {
    SetCommonAnimation( &mIdleAnimation );
	mAttackAnimation.enabled = false;
}

void Enemy::SetCommonAnimation( ruAnimation * anim ) {
    mModel->SetAnimation( anim );
}

void Enemy::SetTorsoAnimation( ruAnimation * anim ) {
	for( auto & torsoPart : mTorsoParts ) {
		torsoPart->SetAnimation( anim );
	}
}

void Enemy::SetLegsAnimation( ruAnimation *pAnim ) {
	for( auto & rightLegPart : mRightLegParts ) {
		rightLegPart->SetAnimation( pAnim );
	}
	for( auto & leftLegPart : mLeftLegParts ) {
		leftLegPart->SetAnimation( pAnim );
	}
}

void Enemy::CreateAnimations() {    
    mRunAnimation = ruAnimation( 0, 33, 0.8, true );
	mRunAnimation.AddFrameListener( 5, ruDelegate::Bind( this, &Enemy::Proxy_RandomStepSound ));
	mRunAnimation.AddFrameListener( 23, ruDelegate::Bind( this, &Enemy::Proxy_RandomStepSound ));

    mAttackAnimation = ruAnimation( 34, 48, 0.78, true );
	mAttackAnimation.AddFrameListener( 44, ruDelegate::Bind( this, &Enemy::Proxy_HitPlayer ));

    mWalkAnimation = ruAnimation( 49, 76, 1, true );
	mWalkAnimation.AddFrameListener( 51, ruDelegate::Bind( this, &Enemy::Proxy_RandomStepSound ));
	mWalkAnimation.AddFrameListener( 67, ruDelegate::Bind( this, &Enemy::Proxy_RandomStepSound ));

	mIdleAnimation = ruAnimation( 77, 85, 1.5, true );
}

void Enemy::CreateSounds() {
    mHitFleshWithAxeSound = ruSound::Load3D( "data/sounds/armor_axe_flesh.ogg" );
    mHitFleshWithAxeSound.Attach( mModel->FindChild( "Weapon" ));

    mBreathSound = ruSound::Load3D( "data/sounds/breath1.ogg" );
    mBreathSound.Attach( mBody );
    mBreathSound.SetVolume( 0.25f );
    mBreathSound.SetRolloffFactor( 35 );
	mBreathSound.SetRoomRolloffFactor( 35 );
    mBreathSound.SetReferenceDistance( 2.8 );

    mScreamSound = ruSound::Load3D( "data/sounds/scream_creepy_1.ogg" );
    mScreamSound.SetVolume( 1.0f );
    mScreamSound.Attach( mBody );
    mScreamSound.SetRolloffFactor( 20 );
	mScreamSound.SetRoomRolloffFactor( 20 );
    mScreamSound.SetReferenceDistance( 4 );

    mFootstepsSounds[ 0 ] = ruSound::Load3D( "data/sounds/step1.ogg" );
    mFootstepsSounds[ 1 ] = ruSound::Load3D( "data/sounds/step2.ogg" );
    mFootstepsSounds[ 2 ] = ruSound::Load3D( "data/sounds/step3.ogg" );
    mFootstepsSounds[ 3 ] = ruSound::Load3D( "data/sounds/step4.ogg" );
    for( int i = 0; i < 4; i++ ) {
        mFootstepsSounds[i].Attach( mBody );
        mFootstepsSounds[i].SetVolume( 0.75f );
        mFootstepsSounds[i].SetRolloffFactor( 35 );
		mFootstepsSounds[i].SetRoomRolloffFactor( 35 );
        mFootstepsSounds[i].SetReferenceDistance( 5 );
    }
}

int Enemy::GetVertexIndexNearestTo( ruVector3 position ) {
    if( mCurrentPath.size() == 0 ) {
        return 0;
    }
    int nearestIndex = 0;
    for( int i = 0; i < mCurrentPath.size(); i++ ) {
        if( ( mCurrentPath[i]->mPosition - position ).Length2() < ( mCurrentPath[nearestIndex]->mPosition - position ).Length2() ) {
            nearestIndex = i;
        }
    }
    return nearestIndex;
}

void Enemy::FillByNamePattern( vector< shared_ptr<ruSceneNode> > & container, const string & pattern ) {
	std::regex rx( pattern );
	for( int i = 0; i < mModel->GetCountChildren(); i++ ) {
		shared_ptr<ruSceneNode> child = mModel->GetChild( i );
		if( regex_match( child->GetName(), rx )) {
			container.push_back( child );
		}
	}
}

void Enemy::FindBodyparts() {
	FillByNamePattern( mRightLegParts, "RightLegP?([[:digit:]]+)" );
	FillByNamePattern( mLeftLegParts, "LeftLegP?([[:digit:]]+)" );
	FillByNamePattern( mRightArmParts, "RightArmP?([[:digit:]]+)" );
	FillByNamePattern( mLeftArmParts, "LeftArmP?([[:digit:]]+)" );
	FillByNamePattern( mTorsoParts, "TorsoBoneP?([[:digit:]]+)" );
	mHead = mModel->FindChild( "Head" );
}

void Enemy::Serialize( SaveFile & out ) {
	out.WriteVector3( mDeathPosition );
    out.WriteVector3( mBody->GetPosition() );
	out.WriteBoolean( mDead );
	out.WriteFloat( mHealth );
}

void Enemy::Deserialize( SaveFile & in ) {
	mDeathPosition = in.ReadVector3();
    mBody->SetPosition( in.ReadVector3( ));
	mDead = in.ReadBoolean();
	mHealth = in.ReadFloat();
}

Enemy::~Enemy() {
	mResurrectTimer->Free();
	mPathCheckTimer->Free();

	mFadeAwaySound.Free();
	auto iter = find( msEnemyList.begin(), msEnemyList.end(), this );
	if( iter != msEnemyList.end() ) {
		msEnemyList.erase( iter );
	}
}

void Enemy::DoBloodSpray() {
	mBloodSpray = ruParticleSystem::Create( 50 );
	mBloodSpray->SetTexture( ruTexture::Request( "data/textures/particles/spray.png" ));
	mBloodSpray->SetType( ruParticleSystem::Type::Box );
	mBloodSpray->SetSpeedDeviation( ruVector3( -0.0015, 0.02, -0.0015 ), ruVector3( 0.0015, -0.09, 0.0015 ) );
	mBloodSpray->SetColorRange( ruVector3( 200, 0, 0 ), ruVector3( 200, 0, 0 ) );
    mBloodSpray->SetPointSize( 0.455f );
	mBloodSpray->SetBoundingBox( ruVector3( -mBodyWidth, 0.0, -mBodyWidth ), ruVector3(  mBodyWidth, mBodyHeight, mBodyWidth ));
	mBloodSpray->SetParticleThickness( 20.5f );
	mBloodSpray->SetAutoResurrection( false );
	mBloodSpray->SetLightingEnabled( true );	
	mBloodSpray->SetPosition( mBody->GetPosition() );	
}

void Enemy::Damage( float dmg ) {
	Actor::Damage( dmg );
	if( mHealth <= 0.0f ) {
		mResurrectTimer->Restart();
		if( !mDead ) {
			DoBloodSpray();
			mFadeAwaySound.Play();
			mDead = true;
		}
		mDeathPosition = mBody->GetPosition();
		mBody->SetPosition( ruVector3( 1000, 1000, 1000 ));
		mBody->Hide();
		mBody->Freeze();
	}
}

void Enemy::SetNextPatrolPoint() {
	mCurrentPatrolPoint++;
	if( mCurrentPatrolPoint >= mPatrolPointList.size() ) {
		mCurrentPatrolPoint = 0;
	}
}

shared_ptr<ruSceneNode> Enemy::GetBody() {
	return mBody;
}
