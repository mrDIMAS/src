#include "Enemy.h"
#include "Menu.h"
#include "Door.h"

//#define ENEMY_ANIMATION_DEBUG 1

void Enemy::Think() {
    if( menu->visible ) {
        return;
    }

	if( moveType == MoveTypeChasePlayer ) {
		target = GetPosition( player->body );
	} else if( moveType == MoveTypeGoToDestination ) {		
		if( currentPath.size() ) {
			target = currentPath[currentWaypointNum]->position;
			if( ( target - GetPosition( body ) ).Length2() < 0.5f ) {
				currentWaypointNum += destWaypointNum - currentWaypointNum > 0 ? 1 : 0;
			}
		}
	} 
	
	bool reachPoint = ( destWaypointNum - currentWaypointNum ) < 0 ;

	if( doPatrol ) {
		moveType = MoveTypeGoToDestination ;
		if( destWaypointNum - currentWaypointNum == 0 ) {
			currentPatrolPoint++;
			if( currentPatrolPoint >= patrolPoints.size() ) {
				currentPatrolPoint = 0;
			}
		}
		destination = patrolPoints[currentPatrolPoint]->position;
	} else { 
		destination = GetPosition( player->body );
	}

    Vector3 direction = target - GetPosition( body );
	float heightUnderTarget = direction.y;
	//direction.y = 0; // use XZ plane instead 3D

    float distanceToPlayer = direction.Length();
    direction.Normalize();    

    angleTo = atan2f( direction.x, direction.z ) - M_PI / 2;
    angleTo = (angleTo > 0 ? angleTo : (2*M_PI + angleTo)) * 360 / (2*M_PI);

    angle = angleTo;

    SetRotation( body, Quaternion( 0, angle, 0 ));

    bool move = true;
	bool targetTooFar = distanceToPlayer > 10.0f;
	Vector3 toPlayer =  GetPosition( player->camera->cameraNode ) - (GetPosition( head ) + GetLookVector( body ).Normalize() * 0.4f);
	bool playerInView = RayTest( GetPosition( head ) + GetLookVector( body ).Normalize() * 0.4f, GetPosition( player->camera->cameraNode ), nullptr ).pointer == player->body.pointer;
	float angleToPlayer = abs( toPlayer.Angle( direction ) * 180.0f / M_PI );

	
	bool enemyDetectPlayer = false;
	if( player->flashlight->on ) {
		// if we light up enemy, he detects player
		enemyDetectPlayer = IsLightViewPoint( player->flashlight->light, GetPosition( body ) ); 
		if( enemyDetectPlayer && playerInView ) {
			if( !playerDetected ) {
				RestartTimer( detectedTimer );
				playerDetected = true; 
			}
		}
	}

	float detectDistance = player->stealthFactor * 10.0f;
	// player right in front of enemy
	if( playerInView && ( distanceToPlayer < detectDistance ) && ( angleToPlayer < 45 ) ) {
		if( !playerDetected ) {
			RestartTimer( detectedTimer );
			playerDetected = true; 
		}
	}

	// enemy doesn't see player, but can hear he, if he moved
	if( playerInView && ( player->stealthFactor >= 0.3f && player->moved && ( distanceToPlayer < 5.0f ))) {
		if( !playerDetected ) {
			RestartTimer( detectedTimer );
			playerDetected = true; 
		}
	}

	DrawGUIText( Format( "PD: %d", playerDetected ? 1 : 0 ).c_str(), 200, 200, 200, 200, gui->font, Vector3( 255, 0, 0 ), 0 );

	if( playerDetected ) {
		enemyDetectPlayer = true;
		if( GetElapsedTimeInSeconds( detectedTimer ) > 2.5f ) {
			playerDetected = false;
		}
	}

	if( enemyDetectPlayer ) {
		moveType = MoveTypeChasePlayer;		
		doPatrol = false;
		PauseSoundSource( breathSound );
		PlaySoundSource( screamSound, true );
		runSpeed = 3.0f;
	} else {
		moveType = MoveTypeGoToDestination;
		doPatrol = true;
		runSpeed = 1.5f;
		PlaySoundSource( breathSound, true );
		PauseSoundSource( screamSound );
	}

	if( moveType == MoveTypeChasePlayer ){
		if( targetTooFar || player->dead ) {
			doPatrol = true;
			moveType = MoveTypeGoToDestination;
			SetIdleAnimation();
		} else {
			if( !player->dead ) {
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
	} else if( moveType == MoveTypeGoToDestination ) {
		GraphVertex * destNearestVertex = pathfinder.GetVertexNearestTo( destination, &currentDestIndex);
		GraphVertex * enemyNearestVertex = pathfinder.GetVertexNearestTo( GetPosition( body ) );
		if( currentDestIndex != lastDestIndex ) { // means player has moved to another waypoint
			pathfinder.BuildPath( enemyNearestVertex, destNearestVertex, currentPath );			
			destWaypointNum = GetVertexIndexNearestTo( currentPath[ currentPath.size() - 1 ]->position );
			currentWaypointNum = GetVertexIndexNearestTo( currentPath[0]->position );
			if( currentWaypointNum > destWaypointNum ) {
				int temp = currentWaypointNum;
				currentWaypointNum = destWaypointNum;
				destWaypointNum = temp;
			}
			lastDestIndex = currentDestIndex;
		}
		SetRunAnimation();
	}

	// check doors
	for( auto d : Door::all ) {
		if( ( GetPosition( d->door ) - GetPosition( body )).Length2() < 2.5f ) {
			if( d->GetState() == Door::State::Closed ) {
				d->Open();
			}
		}
	}

	if( move && !reachPoint ) {
		pathLen += 0.1f;
		if( abs( pathLen - lastPathLen ) > 3 ) {
			lastPathLen = pathLen;
			PlaySoundSource( footstepsSounds[ rand() % 4 ]);
		}
		Vector3 speedVector = direction * runSpeed;// + Vector3( 0, -.1, 0 );
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

	//DrawGUIText( Format( "Patrol:%d", doPatrol ? 1 : 0 ).c_str(), 100, 100, 200, 200, gui->font, Vector3( 255, 0, 0 ), 0 );
}

Enemy::Enemy( const char * file, vector<GraphVertex*> & path, vector<GraphVertex*> & patrol ) {
	pathfinder.SetVertices( path );
	patrolPoints = patrol;
	currentPatrolPoint = 0;

	pathLen = 0.0f;
	lastPathLen = 0.0f;
	bodyHeight = 1.0f;

    body = CreateSceneNode();
    SetCapsuleBody( body, bodyHeight, 0.25f );
    SetAngularFactor( body, Vector3( 0, 0, 0 ));
    SetPosition( body, Vector3( 5, 1, -2.5 ));
	SetMass( body, 100 );
	SetFriction( body, 0 );

    model = LoadScene( file );
    Attach( model, body );
    SetPosition( model, Vector3( 0, -0.5f, 0 ));

	FindBodyparts();

	angleTo = 0.0f;
    angle = 0.0f;

    damageTimer = CreateTimer();

	CreateSounds();

	CreateAnimations();
	
	runSpeed = 1.5f; 

	moveType = MoveTypeGoToDestination;

	destWaypointNum = 0;
	lastDestIndex = -1;
	playerDetected = false;
	detectedTimer = CreateTimer();
	int a = 0;
}

void Enemy::SetWalkAnimation() {
	SetCommonAnimation( &animWalk );
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

void Enemy::CreateAnimations() {
	// Animations
	animIdle = Animation( 0, 15, 0.08, true );
	animRun = Animation( 16, 34, 0.08, true );
	animAttack = Animation( 35, 46, 0.035, true );
	animWalk = Animation( 47, 58, 0.045, true );
}

void Enemy::CreateSounds() {
	hitFleshWithAxeSound = CreateSound3D( "data/sounds/armor_axe_flesh.ogg" );
	AttachSound( hitFleshWithAxeSound, FindInObjectByName( model, "AttackHand" ));

	breathSound = CreateSound3D( "data/sounds/breath1.ogg" );
	AttachSound( breathSound, body );
	SetVolume( breathSound, 0.5f );
	SetRolloffFactor( breathSound, 20 );
	SetSoundReferenceDistance( breathSound, 5 );

	screamSound = CreateSound3D( "data/sounds/scream_creepy_1.ogg" );
	SetVolume( screamSound, 1.0f );
	AttachSound( screamSound, body );    
	SetRolloffFactor( screamSound, 20 );
	SetSoundReferenceDistance( screamSound, 5 );

	footstepsSounds[ 0 ] = CreateSound3D( "data/sounds/step1.ogg" );
	footstepsSounds[ 1 ] = CreateSound3D( "data/sounds/step2.ogg" );
	footstepsSounds[ 2 ] = CreateSound3D( "data/sounds/step3.ogg" );
	footstepsSounds[ 3 ] = CreateSound3D( "data/sounds/step4.ogg" );
	for( int i = 0; i < 4; i++ ) {
		AttachSound( footstepsSounds[i], body );   
		SetVolume( footstepsSounds[i], 0.75f );
		SetRolloffFactor( footstepsSounds[i], 10 );
		SetSoundReferenceDistance( footstepsSounds[i], 5 );
	}
}

int Enemy::GetVertexIndexNearestTo( Vector3 position ) {
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

void Enemy::FindBodyparts() {
	rightLeg = FindInObjectByName( model, "RightLeg" );
	leftLeg = FindInObjectByName( model, "LeftLeg" );
	rightLegDown = FindInObjectByName( model, "RightLegDown" );
	leftLegDown = FindInObjectByName( model, "LeftLegDown" );
	torsoBone = FindInObjectByName( model, "Torso" );
	attackHand = FindInObjectByName( model, "AttackHand" );
	head = FindInObjectByName( model, "HeadBone" );
}

void Enemy::Serialize( TextFileStream & out ) {
	out.WriteVector3( GetPosition( body ));
}

void Enemy::Deserialize( TextFileStream & in ) {
	SetPosition( body, in.ReadVector3( ));
}