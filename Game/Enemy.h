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

    enum class Animation {
        Idle = 0x01,
        Run,
        Attack,
        RunAndAttack,
        StayAndAttack,
    };

    Animation currentAnimation;
    void SetLegsAnimation( int frameBegin, int frameEnd );
    void SetTorsoAnimation( int frameBegin, int frameEnd );
    void SetCommonAnimation( int frameBegin, int frameEnd );
    virtual void SetIdleAnimation( );
    virtual void SetRunAnimation();
    virtual void SetRunAndAttackAnimation();
    virtual void SetStayAndAttackAnimation();
    Enemy( const char * file );
    void Update();
};