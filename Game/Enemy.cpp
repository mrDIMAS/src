#include "Enemy.h"
#include "Menu.h"
#include "Door.h"

Enemy::Enemy( const string & file, vector<GraphVertex*> & path, vector<GraphVertex*> & patrol ) : Actor( 0.5f, 0.25f ) {
	mPathfinder.SetVertices( path );
	mPatrolPointList = patrol;
	mCurrentPatrolPoint = 0;

	mStepLength = 0.0f;
	mLastStepLength = 0.0f;

	mModel = ruLoadScene( file );
	ruAttachNode( mModel, mBody );
	ruSetNodePosition( mModel, ruVector3( 0, -0.7f, 0 ));

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
}


void Enemy::Think() {
    if( pMainMenu->IsVisible() || mHealth <= 0.0f ) {
		ruPauseSound( mScreamSound );
		ruPauseSound( mBreathSound );
		ruPauseSound( mHitFleshWithAxeSound );
        return;
    }

	if( mStun ) {
		ruPauseSound( mScreamSound );
		ruPlaySound( mBreathSound );
		if( ruGetElapsedTimeInSeconds( mStunTimer ) > 2.0f ) {
			Stun( false );
		}
	} else {
		if( mMoveType == MoveType::ChasePlayer ) {
			mTarget = pPlayer->GetCurrentPosition();
		} else if( mMoveType == MoveType::GoToDestination ) {
			if( mCurrentPath.size() ) {
				mTarget = mCurrentPath[mCurrentWaypointNum]->mPosition;
				if( ( mTarget - ruGetNodePosition( mBody ) ).Length2() < 0.5f ) {
					mCurrentWaypointNum += mDestinationWaypointNum - mCurrentWaypointNum > 0 ? 1 : 0;
				}
			}
		}

		bool reachPoint = ( mDestinationWaypointNum - mCurrentWaypointNum ) < 0 ;

		if( mDoPatrol ) {
			mMoveType = MoveType::GoToDestination ;
			if( mDestinationWaypointNum - mCurrentWaypointNum == 0 ) {
				mCurrentPatrolPoint++;
				if( mCurrentPatrolPoint >= mPatrolPointList.size() ) {
					mCurrentPatrolPoint = 0;
				}
			}
			mDestination = mPatrolPointList[mCurrentPatrolPoint]->mPosition;
			mTargetIsPlayer = false;
		} else {
			mDestination = pPlayer->GetCurrentPosition();
			mTargetIsPlayer = true;
		}

		ruVector3 direction = mTarget - ruGetNodePosition( mBody );

		float distanceToPlayer = ( pPlayer->GetCurrentPosition() - ruGetNodePosition( mBody )).Length();
		float distanceToTarget = direction.Length();
		direction.Normalize();

		mAngleTo = atan2f( direction.x, direction.z ) - M_PI / 2;
		mAngleTo = (mAngleTo > 0 ? mAngleTo : (2*M_PI + mAngleTo)) * 360 / (2*M_PI);

		mAngle += ( mAngleTo - mAngle ) * 0.025f;

		ruSetNodeRotation( mBody, ruQuaternion( 0, mAngle, 0 ));

		bool move = true;
		bool playerTooFar = mTargetIsPlayer && distanceToTarget > 10.0f;
		ruVector3 toPlayer =  ruGetNodePosition( pPlayer->mpCamera->mNode ) - (ruGetNodePosition( mHead ) + ruGetNodeLookVector( mBody ).Normalize() * 0.4f);
		bool playerInView = pPlayer->IsVisibleFromPoint( ruGetNodePosition( mHead ) + ruGetNodeLookVector( mBody ).Normalize() * 0.4f );
		float angleToPlayer = abs( toPlayer.Angle( direction ) * 180.0f / M_PI );

		bool enemyDetectPlayer = false;
		if( playerInView ) {
			if( pPlayer->mpFlashlight->IsOn() ) {
				// if we light up enemy, he detects player
				if( pPlayer->mpFlashlight->IsBeamContainsPoint( ruGetNodePosition( mBody ))) {
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

		if( enemyDetectPlayer ) {
			mMoveType = MoveType::ChasePlayer;
			mDoPatrol = false;
			ruPauseSound( mBreathSound );
			ruPlaySound( mScreamSound, true );
			mRunSpeed = 3.0f;
		} else {
			mMoveType = MoveType::GoToDestination;
			mDoPatrol = true;
			mRunSpeed = 1.5f;
			ruPlaySound( mBreathSound, true );
			ruPauseSound( mScreamSound );
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
							if( ruGetCurrentAnimation( mAttackHand )->GetCurrentFrame() == mAttackAnimation.GetBeginFrame() ) {
								mAttackDone = false;
							}
							if( ruGetCurrentAnimation( mAttackHand )->GetCurrentFrame() == mAttackAnimation.GetEndFrame() - 5 && !mAttackDone ) {
								mAttackDone = true;
								pPlayer->Damage( 20 );
								ruPlaySound( mHitFleshWithAxeSound, true );
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
			GraphVertex * enemyNearestVertex = mPathfinder.GetVertexNearestTo( ruGetNodePosition( mBody ) );
			if( mCurrentDestinationIndex != mLastDestinationIndex ) { // means player has moved to another waypoint
				mPathfinder.BuildPath( enemyNearestVertex, destNearestVertex, mCurrentPath );
				mDestinationWaypointNum = GetVertexIndexNearestTo( mCurrentPath[ mCurrentPath.size() - 1 ]->mPosition );
				mCurrentWaypointNum = GetVertexIndexNearestTo( mCurrentPath[0]->mPosition );
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
			if( ( ruGetNodePosition( pDoor->mDoorNode ) - ruGetNodePosition( mBody )).Length2() < 2.5f ) {
				if( pDoor->GetState() == Door::State::Closed ) {
					pDoor->Open();
				}
			}
		}

		if( move && !reachPoint ) {
			mStepLength += 0.1f;
			if( abs( mStepLength - mLastStepLength ) > 3 ) {
				mLastStepLength = mStepLength;
				ruPlaySound( mFootstepsSounds[ rand() % 4 ]);
			}
			ruVector3 speedVector = direction * mRunSpeed;// + Vector3( 0, -.1, 0 );
			ruMoveNode( mBody, speedVector );
		}

		ruSetAnimationEnabled( mModel, true );
		mAttackAnimation.Update();
		mIdleAnimation.Update();
		mRunAnimation.Update();

		ManageEnvironmentDamaging();
	}
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
    ruSetAnimation( mModel, anim );
}

void Enemy::SetTorsoAnimation( ruAnimation * anim ) {
    ruSetAnimation( mTorsoBone, anim );
}

void Enemy::SetLegsAnimation( ruAnimation *pAnim ) {
    ruSetAnimation( mRightLeg, pAnim );
    ruSetAnimation( mLeftLeg, pAnim );
    ruSetAnimation( mRightLegDown, pAnim );
    ruSetAnimation( mLeftLegDown, pAnim );
}

void Enemy::CreateAnimations() {
    // Animations
    mIdleAnimation = ruAnimation( 0, 15, 0.08, true );
    mRunAnimation = ruAnimation( 16, 34, 0.08, true );
    mAttackAnimation = ruAnimation( 35, 46, 0.035, true );
    mWalkAnimation = ruAnimation( 47, 58, 0.045, true );
}

void Enemy::CreateSounds() {
    mHitFleshWithAxeSound = ruLoadSound3D( "data/sounds/armor_axe_flesh.ogg" );
    ruAttachSound( mHitFleshWithAxeSound, ruFindInObjectByName( mModel, "AttackHand" ));

    mBreathSound = ruLoadSound3D( "data/sounds/breath1.ogg" );
    ruAttachSound( mBreathSound, mBody );
    ruSetSoundVolume( mBreathSound, 0.25f );
    ruSetRolloffFactor( mBreathSound, 20 );
    ruSetSoundReferenceDistance( mBreathSound, 2.8 );

    mScreamSound = ruLoadSound3D( "data/sounds/scream_creepy_1.ogg" );
    ruSetSoundVolume( mScreamSound, 1.0f );
    ruAttachSound( mScreamSound, mBody );
    ruSetRolloffFactor( mScreamSound, 20 );
    ruSetSoundReferenceDistance( mScreamSound, 4 );

    mFootstepsSounds[ 0 ] = ruLoadSound3D( "data/sounds/step1.ogg" );
    mFootstepsSounds[ 1 ] = ruLoadSound3D( "data/sounds/step2.ogg" );
    mFootstepsSounds[ 2 ] = ruLoadSound3D( "data/sounds/step3.ogg" );
    mFootstepsSounds[ 3 ] = ruLoadSound3D( "data/sounds/step4.ogg" );
    for( int i = 0; i < 4; i++ ) {
        ruAttachSound( mFootstepsSounds[i], mBody );
        ruSetSoundVolume( mFootstepsSounds[i], 0.75f );
        ruSetRolloffFactor( mFootstepsSounds[i], 10 );
        ruSetSoundReferenceDistance( mFootstepsSounds[i], 5 );
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

void Enemy::Serialize( TextFileStream & out ) {
    out.WriteVector3( ruGetNodePosition( mBody ));
}

void Enemy::Deserialize( TextFileStream & in ) {
    ruSetNodePosition( mBody, in.ReadVector3( ));
}

Enemy::~Enemy() {

}

void Enemy::Damage( float dmg ) {
	Actor::Damage( dmg );
	if( dmg > 2.5f ) {
		Stun( true );
	}
	if( mHealth <= 0.0f ) {
		ruFreeze( mBody );
	}
}

void Enemy::Stun( bool state ) {
	mStun = state;
	if( mStun ) {
		ruSetNodeLinearFactor( mBody, ruVector3( 0,0,0 ));
		ruSetNodeVelocity( mBody, ruVector3( 0,0,0 ));
		ruRestartTimer( mStunTimer );
	} else {
		ruSetNodeLinearFactor( mBody, ruVector3( 1,1,1 ));
	}
}
