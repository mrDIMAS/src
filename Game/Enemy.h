#pragma once

#include "Player.h"

class Enemy {
public:
    NodeHandle model;
    NodeHandle body;

    NodeHandle rightLeg;
    NodeHandle leftLeg;

    NodeHandle rightLegDown;
    NodeHandle leftLegDown;

    NodeHandle torsoBone;

    SoundHandle hitFleshWithAxeSound;
    SoundHandle breathSound;
    SoundHandle screamSound;

    float angleTo;
    float angle;

    bool detectPlayer;

    SoundHandle footstepsSounds[ 4 ];

    TimerHandle damageTimer;

	Animation animIdle;
	Animation animRun;
	Animation animAttack;

    void SetLegsAnimation( Animation * anim );
    void SetTorsoAnimation( Animation * anim );
    void SetCommonAnimation( Animation * anim );
    virtual void SetIdleAnimation( );
    virtual void SetRunAnimation();
    virtual void SetRunAndAttackAnimation();
    virtual void SetStayAndAttackAnimation();
    Enemy( const char * file );
    void Update();
};