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

    ruNodeHandle model;
    ruNodeHandle body;

	ruVector3 target;
	ruVector3 destination;

    ruNodeHandle rightLeg;
    ruNodeHandle leftLeg;

    ruNodeHandle rightLegDown;
    ruNodeHandle leftLegDown;

    ruNodeHandle torsoBone;
	ruNodeHandle attackHand;
	ruNodeHandle head;

    ruSoundHandle hitFleshWithAxeSound;
    ruSoundHandle breathSound;
    ruSoundHandle screamSound;

	bool playerDetected;
	ruTimerHandle detectedTimer;

	float lastPathLen;
	float pathLen;
	enum {
		MoveTypeChasePlayer,
		MoveTypeGoToDestination,		
	};

	int GetVertexIndexNearestTo( ruVector3 position );
	
	int moveType;

	bool attackDone;
	bool targetIsPlayer;

    float angleTo;
    float angle;
	float bodyHeight;

	float runSpeed;

    ruSoundHandle footstepsSounds[ 4 ];

    ruTimerHandle damageTimer;

	ruAnimation animIdle;
	ruAnimation animRun;
	ruAnimation animAttack;
	ruAnimation animWalk;

	void DrawAnimationDebugInfo( ruNodeHandle node, int & y );
    void SetLegsAnimation( ruAnimation * anim );
    void SetTorsoAnimation( ruAnimation * anim );
    void SetCommonAnimation( ruAnimation * anim );
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