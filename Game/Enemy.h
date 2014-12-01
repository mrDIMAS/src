#pragma once

#include "Player.h"
#include "Pathfinder.h"

class Enemy {
public:
	vector< GraphVertex* > currentPath;
	vector< GraphVertex* > patrolPoints;
	int currentWaypointNum;
	int destWaypointNum;
	int lastDestIndex;
	int currentDestIndex;

	bool doPatrol;
	int currentPatrolPoint;

	Pathfinder pathfinder;

    NodeHandle model;
    NodeHandle body;

	Vector3 target;
	Vector3 destination;

    NodeHandle rightLeg;
    NodeHandle leftLeg;

    NodeHandle rightLegDown;
    NodeHandle leftLegDown;

    NodeHandle torsoBone;
	NodeHandle attackHand;
	NodeHandle head;

    SoundHandle hitFleshWithAxeSound;
    SoundHandle breathSound;
    SoundHandle screamSound;

	bool playerDetected;
	TimerHandle detectedTimer;

	float lastPathLen;
	float pathLen;
	enum {
		MoveTypeChasePlayer,
		MoveTypeGoToDestination,		
	};

	int GetVertexIndexNearestTo( Vector3 position );
	
	int moveType;

	bool attackDone;

    float angleTo;
    float angle;
	float bodyHeight;

	float runSpeed;

    SoundHandle footstepsSounds[ 4 ];

    TimerHandle damageTimer;

	Animation animIdle;
	Animation animRun;
	Animation animAttack;
	Animation animWalk;

	void DrawAnimationDebugInfo( NodeHandle node, int & y );
    void SetLegsAnimation( Animation * anim );
    void SetTorsoAnimation( Animation * anim );
    void SetCommonAnimation( Animation * anim );
    virtual void SetIdleAnimation( );
    virtual void SetRunAnimation();
	virtual void SetWalkAnimation();
    virtual void SetRunAndAttackAnimation();
    virtual void SetStayAndAttackAnimation();
    Enemy( const char * file, vector<GraphVertex*> & path, vector<GraphVertex*> & patrol );

	void FindBodyparts();

	void CreateSounds();
	void CreateAnimations();
	void Think();

	void Serialize( TextFileStream & out );
	void Deserialize( TextFileStream & in );
}; 