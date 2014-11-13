#pragma once

#include "Player.h"
#include "Pathfinder.h"

class Enemy {
public:
	vector< GraphVertex* > currentPath;
	int currentWaypointNum;
	int destWaypointNum;
	int lastPlayerIndex;
	bool needRebuildPath;
	int patrolDirection;
	int currentPlayerIndex;
	Pathfinder pathfinder;

    NodeHandle model;
    NodeHandle body;

	Vector3 target;

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

	enum {
		ActionPatrol,
		ActionChasePlayer,
		ActionGoToPlayer,
	};

	int GetVertexIndexNearestTo( Vector3 position ) {
		if( currentPath.size() == 0 ) {
			return 0;
		};
		int nearestIndex = 0;
		for( int i = 0; i < currentPath.size(); i++ ) {
			if( ( currentPath[i]->position - position ).Length2() < ( currentPath[nearestIndex]->position - position ).Length2() ) {
				nearestIndex = i;
			}

		}
		return nearestIndex;
	}
	
	int action;

	bool attackDone;

    float angleTo;
    float angle;
	float bodyHeight;

    bool detectPlayer;
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
    Enemy( const char * file, vector<GraphVertex*> & path );
    void Think();
}; 