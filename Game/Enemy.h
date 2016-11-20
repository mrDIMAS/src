#pragma once

#include "Player.h"
#include "Pathfinder.h"
#include "Actor.h"

class Enemy : public Actor {
public:
	enum class MoveType {
		ChasePlayer, // chase&kill player
		Patrol, // patrolling
		CheckSound // enemy hears something
	};
	vector<shared_ptr<ruSound>> mReactSounds;
private:

	vector<shared_ptr<GraphVertex>> mCurrentPath;
	vector<shared_ptr<GraphVertex>> mPatrolPointList;
	shared_ptr<GraphVertex> mTargetVertex;
	int mCurrentPatrolPoint;
	int mCurrentPathPoint;
	Pathfinder mPathfinder;
	shared_ptr<ruSceneNode> mModel;
	ruVector3 mTarget;
	ruVector3 mDestination;
	ruVector3 mCheckSoundPosition;
	vector<shared_ptr<ruSceneNode>> mRightLegParts;
	vector<shared_ptr<ruSceneNode>> mLeftLegParts;
	vector<shared_ptr<ruSceneNode>> mRightArmParts;
	vector<shared_ptr<ruSceneNode>> mLeftArmParts;
	vector<shared_ptr<ruSceneNode>> mTorsoParts;
	shared_ptr<ruSceneNode> mHead;
	shared_ptr<ruSound> mHitFleshWithAxeSound;
	shared_ptr<ruSound> mBreathSound;
	shared_ptr<ruSound> mScreamSound;
	MoveType mMoveType;
	float mAngleTo;
	float mAngle;
	float mRunSpeed;
	float mHitDistance;
	int mPathCheckTimer;
	int mLostTimer;
	ruAnimation mIdleAnimation;
	ruAnimation mRunAnimation;
	ruAnimation mAttackAnimation;
	ruAnimation mWalkAnimation;
	ruVector3 mLastCheckPosition;
	vector<unique_ptr<SoundMaterial>> mSoundMaterialList;
	void HitPlayer();
	void EmitStepSound();
	void FillByNamePattern(vector< shared_ptr<ruSceneNode> > & container, const string & pattern);
	void Listen();
public:
	void ForceCheckSound(ruVector3 position);
	shared_ptr<ruSceneNode> GetBody();
	void SetNextPatrolPoint();
	void SetLegsAnimation(ruAnimation *pAnim);
	void SetTorsoAnimation(ruAnimation * anim);
	void SetCommonAnimation(ruAnimation * anim);
	virtual void SetIdleAnimation();
	virtual void SetRunAnimation();
	virtual void SetWalkAnimation();
	virtual void SetRunAndAttackAnimation();
	virtual void SetStayAndAttackAnimation();
	explicit Enemy(unique_ptr<Game> & game, const vector<shared_ptr<GraphVertex>> & path, const vector<shared_ptr<GraphVertex>> & patrol);
	virtual ~Enemy();
	void Think();
	void Serialize(SaveFile & out);
	virtual void Damage(float dmg) override final;
	void LookAt(const ruVector3 & lookAt);
};