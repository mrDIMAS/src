#include "Enemy.h"
#include "Menu.h"

void Enemy::Update() {
    if( menu->visible ) {
        return;
    }

    static float PI = 3.14159f;

    Vector3 direction = GetPosition( player->body ) - GetPosition( body );

    float distanceToPlayer = direction.Length();

    direction.Normalize();
    direction.y = 0;

    angleTo = atan2f( direction.x, direction.z ) - PI / 2;
    angleTo = (angleTo > 0 ? angleTo : (2*PI + angleTo)) * 360 / (2*PI);

    angle = angleTo;//+= ( angleTo - angle ) * 0.1f;

    SetRotation( body, Quaternion( 0, angle, 0 ));

    bool move = true;

    if( distanceToPlayer > 10.0f || player->dead ) {
        SetIdleAnimation();

        PlaySoundSource( breathSound, true );

        PauseSoundSource( screamSound );

        detectPlayer = false;
    } else {
        if( !detectPlayer ) {
            detectPlayer = IsNodeInFrustum( torsoBone );
        }

        if( !player->dead && detectPlayer ) {
            PauseSoundSource( breathSound );

            PlaySoundSource( screamSound, true );

            if( distanceToPlayer < 4.0f ) {
                if( distanceToPlayer < 1 ) {
                    SetStayAndAttackAnimation();

                    move = false;

                    if( GetElapsedTimeInSeconds( damageTimer ) > 0.8f ) {
                        player->Damage( 20 );

                        PlaySoundSource( hitFleshWithAxeSound, true );

                        RestartTimer( damageTimer );
                    }
                } else {
                    SetRunAndAttackAnimation();
                }
            } else {
                SetRunAnimation();
            }

            if( move ) {
                Move( body, direction * 5.42f + Vector3( 0, -1, 0 ) );
            }
        }
    }

    if( !IsAnimationEnabled( model )) {
        SetAnimationEnabled( model, true );
    }
}

Enemy::Enemy( const char * file ) {
    body = CreateSceneNode();
    SetCapsuleBody( body, 1.0f, 0.25f );
    SetAngularFactor( body, Vector3( 0, 0, 0 ));
    SetPosition( body, Vector3( 5, 1, -2.5 ));

    model = LoadScene( file );
    Attach( model, body );
    SetPosition( model, Vector3( 0, -0.5f, 0 ));

    rightLeg = FindInObjectByName( model, "RightLeg" );
    leftLeg = FindInObjectByName( model, "LeftLeg" );

    rightLegDown = FindInObjectByName( model, "RightLegDown" );
    leftLegDown = FindInObjectByName( model, "LeftLegDown" );

    torsoBone = FindInObjectByName( model, "Torso" );

    //SetIdleAnimation();

    angleTo = 0.0f;
    angle = 0.0f;

    damageTimer = CreateTimer();

    hitFleshWithAxeSound = CreateSound3D( "data/sounds/armor_axe_flesh.ogg" );
    AttachSound( hitFleshWithAxeSound, FindInObjectByName( model, "AttackHand" ));

    breathSound = CreateSound3D( "data/sounds/breath1.ogg" );
    AttachSound( breathSound, model );

    screamSound = CreateSound3D( "data/sounds/scream_creepy_1.ogg" );
    AttachSound( screamSound, model );

    detectPlayer = false;

    footstepsSounds[ 0 ] = CreateSound3D( "data/sounds/step1.ogg" );
    footstepsSounds[ 1 ] = CreateSound3D( "data/sounds/step2.ogg" );
    footstepsSounds[ 2 ] = CreateSound3D( "data/sounds/step3.ogg" );
    footstepsSounds[ 3 ] = CreateSound3D( "data/sounds/step4.ogg" );

	// Animations
	animIdle = Animation( 0, 15, 0.8, false );
	animRun = Animation( 15, 32, 0.8, false );
	animAttack = Animation( 32, 44, 0.35, false );

	Animation * anim = GetCurrentAnimation( model );

	int a = 0;
}


void Enemy::SetStayAndAttackAnimation() {
	SetCommonAnimation( &animIdle );
	SetTorsoAnimation( &animAttack );
}

void Enemy::SetRunAndAttackAnimation() {
	SetCommonAnimation( &animAttack );
	SetLegsAnimation( &animRun );
}

void Enemy::SetRunAnimation() {
    SetCommonAnimation( &animRun );
}

void Enemy::SetIdleAnimation() {
    SetCommonAnimation( &animIdle );
}

void Enemy::SetCommonAnimation( Animation * anim ) {
    SetAnimation( model, anim );
}

void Enemy::SetTorsoAnimation( Animation * anim ) {
    SetAnimation( torsoBone, anim );
}

void Enemy::SetLegsAnimation( Animation * anim ) {
    SetAnimation( rightLeg, anim );
    SetAnimation( leftLeg, anim );
    SetAnimation( rightLegDown, anim );
    SetAnimation( leftLegDown, anim );
}
