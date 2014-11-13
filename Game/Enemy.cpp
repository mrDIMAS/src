#include "Enemy.h"
#include "Menu.h"
#include "Door.h"

//#define ENEMY_ANIMATION_DEBUG 1

void Enemy::Think() {
    if( menu->visible ) {
        return;
    }

	if( action == ActionChasePlayer ) {
		target = GetPosition( player->body );
	} else if( action == ActionPatrol ) {
		if( currentPath.size() ) {
			target = currentPath[currentWaypointNum]->position;
			if( ( target - GetPosition( body ) ).Length2() < 0.5f ) {
				currentWaypointNum += patrolDirection;
				if( currentWaypointNum >= ( currentPath.size() )) {
					patrolDirection = -1;
					currentWaypointNum = currentPath.size() - 1;
				}
				if( currentWaypointNum < 0 ) {
					patrolDirection = 1;
					currentWaypointNum = 0;
				}
			}
		}
	} else if( action == ActionGoToPlayer ) {		
		if( currentPath.size() ) {
			target = currentPath[currentWaypointNum]->position;
			if( ( target - GetPosition( body ) ).Length2() < 0.5f ) {
				currentWaypointNum += destWaypointNum - currentWaypointNum > 0 ? 1 : 0;
			}
		}
	}

    Vector3 direction = target - GetPosition( body );
	float heightUnderTarget = direction.y;
	direction.y = 0; // use XZ plane instead 3D

    float distanceToPlayer = direction.Length();
    direction.Normalize();    

    angleTo = atan2f( direction.x, direction.z ) - M_PI / 2;
    angleTo = (angleTo > 0 ? angleTo : (2*M_PI + angleTo)) * 360 / (2*M_PI);

    angle = angleTo;

    SetRotation( body, Quaternion( 0, angle, 0 ));

    bool move = true;
	bool targetTooFar = distanceToPlayer > 10.0f;
	bool targetTooHigh = heightUnderTarget > 2 * bodyHeight;
	bool wayObstructed = RayTest( GetPosition( head ) + GetLookVector( body ).Normalize() * 0.4f, GetPosition( player->camera->cameraNode ), nullptr ).pointer != player->body.pointer;
	
	if( action == ActionChasePlayer ){
		if( targetTooFar || player->dead || targetTooHigh || wayObstructed ) {
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
						move = false;					
						SetStayAndAttackAnimation();         

						if( GetCurrentAnimation( attackHand )->GetCurrentFrame() == animAttack.GetBeginFrame() ) {
							attackDone = false;
						}
						if( GetCurrentAnimation( attackHand )->GetCurrentFrame() == animAttack.GetEndFrame() - 5 && !attackDone ) {
							attackDone = true;
							player->Damage( 20 );						
							PlaySoundSource( hitFleshWithAxeSound, true );
						}
					} else {
						SetRunAndAttackAnimation();
					}
				} else {
					SetRunAnimation();
				}				
			}
		}
	} else if( action == ActionPatrol ) {
		if( needRebuildPath ) {
			GraphVertex * begin = pathfinder.GetPoint( 0 );
			GraphVertex * end = pathfinder.GetPoint( pathfinder.GetPointCount() - 1 );
			pathfinder.BuildPath( begin, end, currentPath );
			currentWaypointNum = 0;
			needRebuildPath = false;
		} 
		SetRunAnimation();
	} else if( action == ActionGoToPlayer ) {
		GraphVertex * playerNearestVertex = pathfinder.GetVertexNearestTo( GetPosition( player->body ), &currentPlayerIndex);
		GraphVertex * enemyNearestVertex = pathfinder.GetVertexNearestTo( GetPosition( body ) );
		if( currentPlayerIndex != lastPlayerIndex ) { // means player has moved to another waypoint
			pathfinder.BuildPath( enemyNearestVertex, playerNearestVertex, currentPath );			
			destWaypointNum = GetVertexIndexNearestTo( pathfinder.GetPoint( pathfinder.GetPointCount() - 1 )->position );
			currentWaypointNum = GetVertexIndexNearestTo( pathfinder.GetPoint( 0 )->position );
			if( currentWaypointNum > destWaypointNum ) {
				int temp = currentWaypointNum;
				currentWaypointNum = destWaypointNum;
				destWaypointNum = temp;
			}
			lastPlayerIndex = currentPlayerIndex;
		}
		DrawGUIText( Format( "Src:%d       Dest:%d", currentWaypointNum, destWaypointNum ).c_str(), 100, 100, 200, 200, gui->font, Vector3( 255, 0, 0 ), 1 );

		SetRunAnimation();
	}

	// check doors
	for( auto d : Door::all ) {
		if( ( GetPosition( d->door ) - GetPosition( body )).Length2() < 1.5f ) {
			if( d->GetState() == Door::State::Closed ) {
				d->Open();
			}
		}
	}

	if( move ) {
		Vector3 speedVector = direction * runSpeed + Vector3( 0, -1, 0 );
		Move( body, speedVector );
	}

#ifdef ENEMY_ANIMATION_DEBUG
	int y = 100;
	DrawAnimationDebugInfo( model, y );
#endif

	SetAnimationEnabled( model, true );

	animAttack.Update();
	animIdle.Update();
	animRun.Update();
}

Enemy::Enemy( const char * file, vector<GraphVertex*> & path ) {
	pathfinder.SetVertices( path );

	bodyHeight = 1.0f;

    body = CreateSceneNode();
    SetCapsuleBody( body, bodyHeight, 0.25f );
    SetAngularFactor( body, Vector3( 0, 0, 0 ));
    SetPosition( body, Vector3( 5, 1, -2.5 ));
	SetMass( body, 100 );

    model = LoadScene( file );
    Attach( model, body );
    SetPosition( model, Vector3( 0, -0.5f, 0 ));

	// Find bodyparts
    rightLeg = FindInObjectByName( model, "RightLeg" );
    leftLeg = FindInObjectByName( model, "LeftLeg" );
    rightLegDown = FindInObjectByName( model, "RightLegDown" );
    leftLegDown = FindInObjectByName( model, "LeftLegDown" );
    torsoBone = FindInObjectByName( model, "Torso" );
	attackHand = FindInObjectByName( model, "AttackHand" );
	head = FindInObjectByName( model, "HeadBone" );

    //SetIdleAnimation();

    angleTo = 0.0f;
    angle = 0.0f;

    damageTimer = CreateTimer();

    hitFleshWithAxeSound = CreateSound3D( "data/sounds/armor_axe_flesh.ogg" );
    AttachSound( hitFleshWithAxeSound, FindInObjectByName( model, "AttackHand" ));

    breathSound = CreateSound3D( "data/sounds/breath1.ogg" );
    AttachSound( breathSound, model );

    screamSound = CreateSound3D( "data/sounds/scream_creepy_1.ogg" );
	SetVolume( screamSound, 0 ); // FIX
    AttachSound( screamSound, model );

    detectPlayer = false;

    footstepsSounds[ 0 ] = CreateSound3D( "data/sounds/step1.ogg" );
    footstepsSounds[ 1 ] = CreateSound3D( "data/sounds/step2.ogg" );
    footstepsSounds[ 2 ] = CreateSound3D( "data/sounds/step3.ogg" );
    footstepsSounds[ 3 ] = CreateSound3D( "data/sounds/step4.ogg" );

	// Animations
	animIdle = Animation( 0, 15, 0.08, true );
	animRun = Animation( 16, 31, 0.08, true );
	animAttack = Animation( 32, 44, 0.035, true );
		
	runSpeed = 4.0f; 

	action = ActionGoToPlayer;
	patrolDirection = 1;
	needRebuildPath = true;
	destWaypointNum = 0;
	lastPlayerIndex = 0;
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

void Enemy::DrawAnimationDebugInfo( NodeHandle node, int & y )
{
	Animation * ca = GetCurrentAnimation( node );
	string animName; 
	if( ca == &animIdle ) {
		animName = "Idle";
	} else if ( ca == &animRun ) {
		animName = "Run";
	} else if( ca == &animAttack ) {
		animName = "Attack";
	}
	y += 16;
	DrawGUIText( Format( 
		"Name: %-20.20sType: %-20.20sFrame: %-8dBegin: %-8dEnd: %-8dNext: %-8d", 
		GetName( node ),
		animName.c_str(),
		ca->GetCurrentFrame(), 
		ca->GetBeginFrame(), 
		ca->GetEndFrame(), 
		ca->GetNextFrame() ).c_str(), 100, y, 700, 200, gui->font, Vector3( 200, 0, 0 ), 0 );

	for( int i = 0; i < GetCountChildren( node ); i++ ) {
		DrawAnimationDebugInfo( GetChild( node, i ), y );
	}
}
