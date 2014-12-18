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

    ruNodeHandle mModel;

    ruVector3 mTarget;
    ruVector3 mDestination;

    ruNodeHandle mRightLeg;
    ruNodeHandle mLeftLeg;

    ruNodeHandle mRightLegDown;
    ruNodeHandle mLeftLegDown;

    ruNodeHandle mTorsoBone;
    ruNodeHandle mAttackHand;
    ruNodeHandle mHead;

    ruSoundHandle mHitFleshWithAxeSound;
    ruSoundHandle mBreathSound;
    ruSoundHandle mScreamSound;

    bool mPlayerDetected;
    ruTimerHandle mPlayerInSightTimer;

    float mLastStepLength;
    float mStepLength;

    MoveType mMoveType;

    bool mAttackDone;
    bool mTargetIsPlayer;

    float mAngleTo;
    float mAngle;

    float mRunSpeed;

    ruSoundHandle mFootstepsSounds[ 4 ];

    ruTimerHandle mDamageTimer;

    ruAnimation mIdleAnimation;
    ruAnimation mRunAnimation;
    ruAnimation mAttackAnimation;
    ruAnimation mWalkAnimation;
public:
    int GetVertexIndexNearestTo( ruVector3 position );

    void SetLegsAnimation( ruAnimation *pAnim );
    void SetTorsoAnimation( ruAnimation * anim );
    void SetCommonAnimation( ruAnimation * anim );
    virtual void SetIdleAnimation( );
    virtual void SetRunAnimation();
    virtual void SetWalkAnimation();
    virtual void SetRunAndAttackAnimation();
    virtual void SetStayAndAttackAnimation();
    explicit Enemy( const char * file, vector<GraphVertex*> & path, vector<GraphVertex*> & patrol );
    virtual ~Enemy();
    void FindBodyparts();
    void CreateSounds();
    void CreateAnimations();
    void Think();
    void Serialize( TextFileStream & out );
    void Deserialize( TextFileStream & in );
};