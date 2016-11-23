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
	vector<shared_ptr<ISound>> mReactSounds;
private:

	vector<shared_ptr<GraphVertex>> mCurrentPath;
	vector<shared_ptr<GraphVertex>> mPatrolPointList;
	shared_ptr<GraphVertex> mTargetVertex;
	int mCurrentPatrolPoint;
	int mCurrentPathPoint;
	Pathfinder mPathfinder;
	shared_ptr<ISceneNode> mModel;
	Vector3 mTarget;
	Vector3 mDestination;
	Vector3 mCheckSoundPosition;
	vector<shared_ptr<ISceneNode>> mRightLegParts;
	vector<shared_ptr<ISceneNode>> mLeftLegParts;
	vector<shared_ptr<ISceneNode>> mRightArmParts;
	vector<shared_ptr<ISceneNode>> mLeftArmParts;
	vector<shared_ptr<ISceneNode>> mTorsoParts;
	shared_ptr<ISceneNode> mHead;
	shared_ptr<ISound> mHitFleshWithAxeSound;
	shared_ptr<ISound> mBreathSound;
	shared_ptr<ISound> mScreamSound;
	MoveType mMoveType;
	float mAngleTo;
	float mAngle;
	float mRunSpeed;
	float mHitDistance;
	int mPathCheckTimer;
	int mLostTimer;
	bool mForceRun;
	Animation mIdleAnimation;
	Animation mRunAnimation;
	Animation mAttackAnimation;
	Animation mWalkAnimation;
	Vector3 mLastCheckPosition;
	vector<unique_ptr<SoundMaterial>> mSoundMaterialList;
	void HitPlayer();
	void EmitStepSound();
	void FillByNamePattern(vector< shared_ptr<ISceneNode> > & container, const string & pattern);
	void Listen();
public:
	void ForceCheckSound(Vector3 position);
	shared_ptr<ISceneNode> GetBody();
	void SetNextPatrolPoint();
	void SetLegsAnimation(Animation *pAnim);
	void SetTorsoAnimation(Animation * anim);
	void SetCommonAnimation(Animation * anim);
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
	void LookAt(const Vector3 & lookAt);
};