#pragma once

#include "Player.h"
#include "Pathfinder.h"
#include "Actor.h"

class Enemy : public Actor {
public:
    enum class MoveType {
        ChasePlayer,
        GoToDestination,
    };
private:
    vector< GraphVertex* > mCurrentPath;
    vector< GraphVertex* > mPatrolPointList;
    int mCurrentWaypointNum;
    int mDestinationWaypointNum;
    int mLastDestinationIndex;
    int mCurrentDestinationIndex;

    bool mDoPatrol;
    int mCurrentPatrolPoint;

    Pathfinder mPathfinder;

    shared_ptr<ruSceneNode> mModel;

    ruVector3 mTarget;
    ruVector3 mDestination;
	ruVector3 mDeathPosition;

	vector<shared_ptr<ruSceneNode>> mRightLegParts;
	vector<shared_ptr<ruSceneNode>> mLeftLegParts;
	vector<shared_ptr<ruSceneNode>> mRightArmParts;
	vector<shared_ptr<ruSceneNode>> mLeftArmParts;
	vector<shared_ptr<ruSceneNode>> mTorsoParts;
	shared_ptr<ruSceneNode> mHead;

    shared_ptr<ruSound> mHitFleshWithAxeSound;
    shared_ptr<ruSound> mBreathSound;
    shared_ptr<ruSound> mScreamSound;

    bool mPlayerDetected;
    shared_ptr<ruTimer> mPlayerInSightTimer;

    float mLastStepLength;
    float mStepLength;

    MoveType mMoveType;

    bool mAttackDone;
    bool mTargetIsPlayer;

    float mAngleTo;
    float mAngle;

    float mRunSpeed;
	float mHitDistance;

    ruAnimation mIdleAnimation;
    ruAnimation mRunAnimation;
    ruAnimation mAttackAnimation;
    ruAnimation mWalkAnimation;

	shared_ptr<ruTimer> mResurrectTimer;

	shared_ptr<ruTimer> mPathCheckTimer;
	ruVector3 mLastCheckPosition;

	vector<unique_ptr<SoundMaterial>> mSoundMaterialList;

	shared_ptr<ruParticleSystem > mBloodSpray;
	shared_ptr<ruSound> mFadeAwaySound;
	bool mDead;


	void Proxy_HitPlayer() {
		// shit, but works :D
		float distanceToPlayer = ( pPlayer->GetCurrentPosition() - mBody->GetPosition() ).Length();
		if( distanceToPlayer < mHitDistance ) {
			pPlayer->Damage( 20 );
			mHitFleshWithAxeSound->Play( true );
		}		
	}

	// called from animation frames
	void Proxy_EmitStepSound() {
		ruRayCastResultEx result = ruPhysics::CastRayEx( mBody->GetPosition() + ruVector3( 0, 0.1, 0 ), mBody->GetPosition() - ruVector3( 0, mBodyHeight * 2.2, 0 ));
		if( result.valid ) {
			for( auto & sMat : mSoundMaterialList ) {
				shared_ptr<ruSound> & snd = sMat->GetRandomSoundAssociatedWith( result.textureName );
				if( snd ) {
					snd->Play( true );
				}
			}
		}
	}

	void FillByNamePattern( vector< shared_ptr<ruSceneNode> > & container, const string & pattern );
public:
    int GetVertexIndexNearestTo( ruVector3 position );
	shared_ptr<ruSceneNode> GetBody();
	void SetNextPatrolPoint();
	void DoBloodSpray();
	void Resurrect();
    void SetLegsAnimation( ruAnimation *pAnim );
    void SetTorsoAnimation( ruAnimation * anim );
    void SetCommonAnimation( ruAnimation * anim );
    virtual void SetIdleAnimation( );
    virtual void SetRunAnimation();
    virtual void SetWalkAnimation();
    virtual void SetRunAndAttackAnimation();
    virtual void SetStayAndAttackAnimation();
    explicit Enemy( vector<GraphVertex*> & path, vector<GraphVertex*> & patrol );
    virtual ~Enemy();
    void FindBodyparts();
    void CreateSounds();
    void CreateAnimations();
    void Think();
    void Serialize( SaveFile & out );
    void Deserialize( SaveFile & in );
	virtual void Damage( float dmg ) final;
};