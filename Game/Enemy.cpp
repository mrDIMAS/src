#include "Precompiled.h"

#include "Enemy.h"
#include "Menu.h"
#include "Door.h"

vector<Enemy*> Enemy::msEnemyList;

Enemy::Enemy( const string & file, vector<GraphVertex*> & path, vector<GraphVertex*> & patrol ) : Actor( 0.5f, 0.25f ) {
	mPathfinder.SetVertices( path );
	mPatrolPointList = patrol;
	mCurrentPatrolPoint = 0;

	mStepLength = 0.0f;
	mLastStepLength = 0.0f;

	mModel = ruLoadScene( file );
	mModel.Attach( mBody );
	mModel.SetPosition( ruVector3( 0, -0.7f, 0 ));

	FindBodyparts();

	mAngleTo = 0.0f;
	mAngle = 0.0f;

	mDamageTimer = ruCreateTimer();

	CreateSounds();

	CreateAnimations();

	mRunSpeed = 1.5f;
	mStun = false;
	mStunTimer = ruCreateTimer();
	mMoveType = MoveType::GoToDestination;

	mDestinationWaypointNum = 0;
	mLastDestinationIndex = -1;
	mPlayerDetected = false;
	mPlayerInSightTimer = ruCreateTimer();

	msEnemyList.push_back( this );

	mDead = false;

	mFadeAwaySound = ruSound::Load2D( "data/sounds/fadeaway.ogg" );
	mFadeAwaySound.SetVolume( 1.5f );
	mResurrectTimer = ruCreateTimer();

	mPathCheckTimer = ruCreateTimer();
}


void Enemy::Think() {
    if( pMainMenu->IsVisible() || mHealth <= 0.0f ) {
		mScreamSound.Pause( );
		mBreathSound.Pause();
		mHitFleshWithAxeSound.Pause();

		if( mDead ) {
			if( ruGetElapsedTimeInSeconds( mResurrectTimer ) >= 10 ) {
				Resurrect();
			}
		}

        return;
    }



	if( mStun ) {
		mScreamSound.Pause();
		mBreathSound.Play();
		if( ruGetElapsedTimeInSeconds( mStunTimer ) > 2.0f ) {
			Stun( false );
		}
	} else {
		if( mMoveType == MoveType::ChasePlayer ) {
			mTarget = pPlayer->GetCurrentPosition();
		} else if( mMoveType == MoveType::GoToDestination ) {
			if( mCurrentPath.size() ) {
				mTarget = mCurrentPath[mCurrentWaypointNum]->mPosition;
				if( ( mTarget - mBody.GetPosition() ).Length2() < 0.5f ) {
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

		ruVector3 direction = mTarget - mBody.GetPosition();

		float distanceToPlayer = ( pPlayer->GetCurrentPosition() - mBody.GetPosition() ).Length();
		float distanceToTarget = direction.Length();
		direction.Normalize();

		mAngleTo = atan2f( direction.x, direction.z ) - M_PI / 2;
		mAngleTo = (mAngleTo > 0 ? mAngleTo : (2*M_PI + mAngleTo)) * 360 / (2*M_PI);

		mAngle += ( mAngleTo - mAngle ) * 0.025f;

		mBody.SetRotation( ruQuaternion( 0, mAngle, 0 ));

		bool move = true;
		bool playerTooFar = mTargetIsPlayer && distanceToTarget > 10.0f;
		ruVector3 toPlayer = pPlayer->mpCamera->mNode.GetPosition() - ( mHead.GetPosition() + mBody.GetLookVector().Normalize() * 0.4f);
		bool playerInView = pPlayer->IsVisibleFromPoint( mHead.GetPosition() + mBody.GetLookVector().Normalize() * 0.4f );
		float angleToPlayer = abs( toPlayer.Angle( direction ) * 180.0f / M_PI );

		bool enemyDetectPlayer = false;
		if( playerInView ) {
			if( pPlayer->mpFlashlight->IsOn() ) {
				// if we light up enemy, he detects player
				if( pPlayer->mpFlashlight->IsBeamContainsPoint( mBody.GetPosition() )) {
					if( !mPlayerDetected ) {
						ruRestartTimer( mPlayerInSightTimer );
						mPlayerDetected = true;
					}
				}
			}
			float detectDistance = pPlayer->mStealthFactor * 10.0f;
			// player right in front of enemy
			if( ( distanceToPlayer < detectDistance ) && ( angleToPlayer < 45 ) ) {
				if( !mPlayerDetected ) {
					ruRestartTimer( mPlayerInSightTimer );
					mPlayerDetected = true;
				}
			}
			// enemy doesn't see player, but can hear he, if he moved
			if(( pPlayer->mStealthFactor >= 0.3f && pPlayer->mMoved && ( distanceToPlayer < 5.0f ))) {
				if( !mPlayerDetected ) {
					ruRestartTimer( mPlayerInSightTimer );
					mPlayerDetected = true;
				}
			}

		}
		// if player too close to the enemy, he detects player
		if( angleToPlayer < 45 && distanceToPlayer < 2.0f ) {
			if( !mPlayerDetected ) {
				ruRestartTimer( mPlayerInSightTimer );
				mPlayerDetected = true;
			}
		}

		if( mPlayerDetected ) {
			enemyDetectPlayer = true;
			if( ruGetElapsedTimeInSeconds( mPlayerInSightTimer ) > 2.5f ) {
				mPlayerDetected = false;
			}
		}

		// DEBUG
		//enemyDetectPlayer = false;

		if( enemyDetectPlayer ) {
			mMoveType = MoveType::ChasePlayer;
			mDoPatrol = false;
			mBreathSound.Pause();
			mScreamSound.Play( true );
			mRunSpeed = 3.0f;
		} else {
			mMoveType = MoveType::GoToDestination;
			mDoPatrol = true;
			mRunSpeed = 1.5f;
			mBreathSound.Play( true );
			mScreamSound.Pause();
		}

		if( mMoveType == MoveType::ChasePlayer ) {
			if( playerTooFar || pPlayer->mDead ) {
				mDoPatrol = true;
				mMoveType = MoveType::GoToDestination;
				SetIdleAnimation();
			} else {
				if( !pPlayer->mDead ) {
					if( distanceToPlayer < 4.0f ) {
						if( distanceToPlayer < 1 ) {
							move = false;
							SetStayAndAttackAnimation();
							if( mAttackHand.GetCurrentAnimation()->GetCurrentFrame() == mAttackAnimation.GetBeginFrame() ) {
								mAttackDone = false;
							}
							if( mAttackHand.GetCurrentAnimation()->GetCurrentFrame() == mAttackAnimation.GetEndFrame() - 5 && !mAttackDone ) {
								mAttackDone = true;
								pPlayer->Damage( 20 );
								mHitFleshWithAxeSound.Play( true );
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
			GraphVertex * enemyNearestVertex = mPathfinder.GetVertexNearestTo( mBody.GetPosition() );
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
			SetRunAnimation();
		}

		// check doors
		for( auto pDoor : Door::msDoorList ) {
			if( ( pDoor->mDoorNode.GetPosition() - mBody.GetPosition()).Length2() < 2.5f ) {
				if( pDoor->GetState() == Door::State::Closed && !pDoor->IsLocked() ) {
					pDoor->Open();
				}
			}
		}

		if( move && !reachPoint ) {
			mStepLength += 0.1f;
			if( abs( mStepLength - mLastStepLength ) > 3 ) {
				mLastStepLength = mStepLength;
				mFootstepsSounds[ rand() % 4 ].Play();
			}
			ruVector3 speedVector = direction * mRunSpeed;// + Vector3( 0, -.1, 0 );
			mBody.Move( speedVector );

			// adjust animation speed according to real speed of moving
			float realSpeed = ( mModel.GetPosition() - mLastPosition).Length();
			//mWalkAnimation.animSpeed = 7 * realSpeed;
			//mRunAnimation.animSpeed = 7 * realSpeed;
		}

		if( ruGetElapsedTimeInSeconds( mPathCheckTimer ) > 0.85f ) {
			// got obstacle (door), can't get throuh it, try next patrol point
			if( ( mModel.GetPosition() - mLastCheckPosition).Length2() < 0.05 ) {
				SetNextPatrolPoint( );
			}
			mLastCheckPosition = mModel.GetPosition();

			ruRestartTimer( mPathCheckTimer );
		}

		mAttackAnimation.enabled = true;
		mIdleAnimation.enabled = true;
		mRunAnimation.enabled = true;

		mAttackAnimation.Update();
		mIdleAnimation.Update();
		mRunAnimation.Update();

		ManageEnvironmentDamaging();

		mLastPosition = mModel.GetPosition();
	}
}

void Enemy::Resurrect() {
	mDead = false;
	mBody.SetPosition( mDeathPosition );
	mBody.Unfreeze();
	mBody.Show();
	
	DoBloodSpray();
	mHealth = 100;
}

void Enemy::SetWalkAnimation() {
    SetCommonAnimation( &mWalkAnimation );
}


void Enemy::SetStayAndAttackAnimation() {
    SetCommonAnimation( &mIdleAnimation );
    SetTorsoAnimation( &mAttackAnimation );
}

void Enemy::SetRunAndAttackAnimation() {
    SetCommonAnimation( &mAttackAnimation );
    SetLegsAnimation( &mRunAnimation );
}

void Enemy::SetRunAnimation() {
    SetCommonAnimation( &mRunAnimation );
}

void Enemy::SetIdleAnimation() {
    SetCommonAnimation( &mIdleAnimation );
}

void Enemy::SetCommonAnimation( ruAnimation * anim ) {
    mModel.SetAnimation( anim );
}

void Enemy::SetTorsoAnimation( ruAnimation * anim ) {
    mTorsoBone.SetAnimation( anim );
}

void Enemy::SetLegsAnimation( ruAnimation *pAnim ) {
    mRightLeg.SetAnimation( pAnim );
    mLeftLeg.SetAnimation( pAnim );
    mRightLegDown.SetAnimation( pAnim );
    mLeftLegDown.SetAnimation( pAnim );
}

void Enemy::CreateAnimations() {
    // Animations
    mIdleAnimation = ruAnimation( 0, 15, 1.5, true );
    mRunAnimation = ruAnimation( 16, 34, 1.2, true );
    mAttackAnimation = ruAnimation( 35, 46, 0.865, true );
    mWalkAnimation = ruAnimation( 47, 58, 2, true );
}

void Enemy::CreateSounds() {
    mHitFleshWithAxeSound = ruSound::Load3D( "data/sounds/armor_axe_flesh.ogg" );
    mHitFleshWithAxeSound.Attach( ruFindInObjectByName( mModel, "AttackHand" ));

    mBreathSound = ruSound::Load3D( "data/sounds/breath1.ogg" );
    mBreathSound.Attach( mBody );
    mBreathSound.SetVolume( 0.25f );
    mBreathSound.SetRolloffFactor( 20 );
    mBreathSound.SetReferenceDistance( 2.8 );

    mScreamSound = ruSound::Load3D( "data/sounds/scream_creepy_1.ogg" );
    mScreamSound.SetVolume( 1.0f );
    mScreamSound.Attach( mBody );
    mScreamSound.SetRolloffFactor( 20 );
    mScreamSound.SetReferenceDistance( 4 );

    mFootstepsSounds[ 0 ] = ruSound::Load3D( "data/sounds/step1.ogg" );
    mFootstepsSounds[ 1 ] = ruSound::Load3D( "data/sounds/step2.ogg" );
    mFootstepsSounds[ 2 ] = ruSound::Load3D( "data/sounds/step3.ogg" );
    mFootstepsSounds[ 3 ] = ruSound::Load3D( "data/sounds/step4.ogg" );
    for( int i = 0; i < 4; i++ ) {
        mFootstepsSounds[i].Attach( mBody );
        mFootstepsSounds[i].SetVolume( 0.75f );
        mFootstepsSounds[i].SetRolloffFactor( 10 );
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

void Enemy::FindBodyparts() {
    mRightLeg = ruFindInObjectByName( mModel, "RightLeg" );
    mLeftLeg = ruFindInObjectByName( mModel, "LeftLeg" );
    mRightLegDown = ruFindInObjectByName( mModel, "RightLegDown" );
    mLeftLegDown = ruFindInObjectByName( mModel, "LeftLegDown" );
    mTorsoBone = ruFindInObjectByName( mModel, "Torso" );
    mAttackHand = ruFindInObjectByName( mModel, "AttackHand" );
    mHead = ruFindInObjectByName( mModel, "HeadBone" );
}

void Enemy::Serialize( SaveFile & out ) {
    out.WriteVector3( mBody.GetPosition() );
	out.WriteBoolean( mDead );
	out.WriteFloat( mHealth );
}

void Enemy::Deserialize( SaveFile & in ) {
    mBody.SetPosition( in.ReadVector3( ));
	mDead = in.ReadBoolean();
	mHealth = in.ReadFloat();
}

Enemy::~Enemy() {
	auto iter = find( msEnemyList.begin(), msEnemyList.end(), this );
	if( iter != msEnemyList.end() ) {
		msEnemyList.erase( iter );
	}
}

void Enemy::DoBloodSpray() {
	if( mBloodSpray.IsValid() ) {
		mBloodSpray.Free();
	} 

	ruParticleSystemProperties psProps;
	psProps.texture = ruGetTexture( "data/textures/particles/spray.png");
	psProps.type = PS_BOX;
	psProps.speedDeviationMin = ruVector3( -0.0015, 0.02, -0.0015 );
	psProps.speedDeviationMax = ruVector3( 0.0015, -0.09, 0.0015 );
	psProps.colorBegin = ruVector3( 200, 0, 0 );
	psProps.colorEnd = ruVector3( 200, 0, 0 );
	psProps.pointSize = 0.455f;
	psProps.boundingBoxMin = ruVector3( -mBodyWidth, 0.0, -mBodyWidth );
	psProps.boundingBoxMax = ruVector3(  mBodyWidth, mBodyHeight, mBodyWidth );
	psProps.particleThickness = 20.5f;
	psProps.autoResurrectDeadParticles = false;
	psProps.useLighting = true;
	mBloodSpray = ruCreateParticleSystem( 50, psProps );
	mBloodSpray.SetPosition( mBody.GetPosition() );	
}

void Enemy::Damage( float dmg ) {
	Actor::Damage( dmg );
	if( mHealth <= 0.0f ) {
		ruRestartTimer( mResurrectTimer );
		if( !mDead ) {
			DoBloodSpray();
			mFadeAwaySound.Play();
			mDead = true;
		}
		mDeathPosition = mBody.GetPosition();
		mBody.SetPosition( ruVector3( 1000, 1000, 1000 ));
		mBody.Hide();
		mBody.Freeze();
	}
}

void Enemy::Stun( bool state ) {
	mStun = state;
	if( mStun ) {
		mBody.SetLinearFactor( ruVector3( 0,0,0 ));
		mBody.SetVelocity( ruVector3( 0,0,0 ));
		ruRestartTimer( mStunTimer );
	} else {
		mBody.SetLinearFactor( ruVector3( 1,1,1 ));
	}
}

void Enemy::SetNextPatrolPoint() {
	mCurrentPatrolPoint++;
	if( mCurrentPatrolPoint >= mPatrolPointList.size() ) {
		mCurrentPatrolPoint = 0;
	}
}

ruSceneNode Enemy::GetBody() {
	return mBody;
}
