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

    ruSceneNode * mModel;

    ruVector3 mTarget;
    ruVector3 mDestination;
	ruVector3 mDeathPosition;

	vector<ruSceneNode *> mRightLegParts;
	vector<ruSceneNode *> mLeftLegParts;
	vector<ruSceneNode *> mRightArmParts;
	vector<ruSceneNode *> mLeftArmParts;
	vector<ruSceneNode *> mTorsoParts;
	ruSceneNode * mHead;

    ruSound mHitFleshWithAxeSound;
    ruSound mBreathSound;
    ruSound mScreamSound;

    bool mPlayerDetected;
    ruTimer * mPlayerInSightTimer;

    float mLastStepLength;
    float mStepLength;

    MoveType mMoveType;

    bool mAttackDone;
    bool mTargetIsPlayer;

    float mAngleTo;
    float mAngle;

    float mRunSpeed;
	float mHitDistance;

    ruSound mFootstepsSounds[ 4 ];

    ruAnimation mIdleAnimation;
    ruAnimation mRunAnimation;
    ruAnimation mAttackAnimation;
    ruAnimation mWalkAnimation;

	ruTimer * mResurrectTimer;

	ruTimer * mPathCheckTimer;
	ruVector3 mLastCheckPosition;

	ruParticleSystem * mBloodSpray;
	ruSound mFadeAwaySound;
	bool mDead;


	void Proxy_HitPlayer() {
		// shit, but works :D
		float distanceToPlayer = ( pPlayer->GetCurrentPosition() - mBody->GetPosition() ).Length();
		if( distanceToPlayer < mHitDistance ) {
			pPlayer->Damage( 20 );
			mHitFleshWithAxeSound.Play( true );
		}		
	}

	// called from animation frames
	void Proxy_RandomStepSound() {
		mFootstepsSounds[ rand() % 4 ].Play();
	}

	void FillByNamePattern( vector< ruSceneNode * > & container, const string & pattern );
public:
	static vector<Enemy*> msEnemyList;

    int GetVertexIndexNearestTo( ruVector3 position );
	ruSceneNode * GetBody();
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