#include "Enemy.h"
#include "Menu.h"
#include "Door.h"

//#define ENEMY_ANIMATION_DEBUG 1

void Enemy::Think()
{
    if( pMainMenu->mVisible )
        return;

    if( moveType == MoveTypeChasePlayer )
        target = ruGetNodePosition( pPlayer->mBody );
    else if( moveType == MoveTypeGoToDestination )
    {
        if( currentPath.size() )
        {
            target = currentPath[currentWaypointNum]->position;
            if( ( target - ruGetNodePosition( body ) ).Length2() < 0.5f )
                currentWaypointNum += destWaypointNum - currentWaypointNum > 0 ? 1 : 0;
        }
    }

    bool reachPoint = ( destWaypointNum - currentWaypointNum ) < 0 ;

    if( doPatrol )
    {
        moveType = MoveTypeGoToDestination ;
        if( destWaypointNum - currentWaypointNum == 0 )
        {
            currentPatrolPoint++;
            if( currentPatrolPoint >= patrolPoints.size() )
                currentPatrolPoint = 0;
        }
        destination = patrolPoints[currentPatrolPoint]->position;
        targetIsPlayer = false;
    }
    else
    {
        destination = ruGetNodePosition( pPlayer->mBody );
        targetIsPlayer = true;
    }

    ruVector3 direction = target - ruGetNodePosition( body );
    float heightUnderTarget = direction.y;

    float distanceToPlayer = (ruGetNodePosition( pPlayer->mBody ) - ruGetNodePosition( body )).Length();
    float distanceToTarget = direction.Length();
    direction.Normalize();

    angleTo = atan2f( direction.x, direction.z ) - M_PI / 2;
    angleTo = (angleTo > 0 ? angleTo : (2*M_PI + angleTo)) * 360 / (2*M_PI);

    angle += ( angleTo - angle ) * 0.025f;

    ruSetNodeRotation( body, ruQuaternion( 0, angle, 0 ));

    bool move = true;
    bool playerTooFar = targetIsPlayer && distanceToTarget > 10.0f;
    ruVector3 toPlayer =  ruGetNodePosition( pPlayer->mpCamera->mNode ) - (ruGetNodePosition( head ) + ruGetNodeLookVector( body ).Normalize() * 0.4f);
    bool playerInView = ruCastRay( ruGetNodePosition( head ) + ruGetNodeLookVector( body ).Normalize() * 0.4f, ruGetNodePosition( pPlayer->mpCamera->mNode ), nullptr ).pointer == pPlayer->mBody.pointer;
    float angleToPlayer = abs( toPlayer.Angle( direction ) * 180.0f / M_PI );

    bool enemyDetectPlayer = false;
    if( playerInView )
    {
        if( pPlayer->mpFlashlight->on )
        {
            // if we light up enemy, he detects player
            if( ruIsLightSeePoint( pPlayer->mpFlashlight->light, ruGetNodePosition( body ) ) )
            {
                if( !playerDetected )
                {
                    ruRestartTimer( detectedTimer );
                    playerDetected = true;
                }
            }
        }
        float detectDistance = pPlayer->mStealthFactor * 10.0f;
        // player right in front of enemy
        if( ( distanceToPlayer < detectDistance ) && ( angleToPlayer < 45 ) )
        {
            if( !playerDetected )
            {
                ruRestartTimer( detectedTimer );
                playerDetected = true;
            }
        }
        // enemy doesn't see player, but can hear he, if he moved
        if(( pPlayer->mStealthFactor >= 0.3f && pPlayer->mMoved && ( distanceToPlayer < 5.0f )))
        {
            if( !playerDetected )
            {
                ruRestartTimer( detectedTimer );
                playerDetected = true;
            }
        }

    }
    // if player too close to the enemy, he detects player
    if( angleToPlayer < 45 && distanceToPlayer < 2.0f )
    {
        if( !playerDetected )
        {
            ruRestartTimer( detectedTimer );
            playerDetected = true;
        }
    }

    if( playerDetected )
    {
        enemyDetectPlayer = true;
        if( ruGetElapsedTimeInSeconds( detectedTimer ) > 2.5f )
            playerDetected = false;
    }

    if( enemyDetectPlayer )
    {
        moveType = MoveTypeChasePlayer;
        doPatrol = false;
        ruPauseSound( breathSound );
        ruPlaySound( screamSound, true );
        runSpeed = 3.0f;
    }
    else
    {
        moveType = MoveTypeGoToDestination;
        doPatrol = true;
        runSpeed = 1.5f;
        ruPlaySound( breathSound, true );
        ruPauseSound( screamSound );
    }

    if( moveType == MoveTypeChasePlayer )
    {
        if( playerTooFar || pPlayer->mDead )
        {
            doPatrol = true;
            moveType = MoveTypeGoToDestination;
            SetIdleAnimation();
        }
        else
        {
            if( !pPlayer->mDead )
            {
                if( distanceToPlayer < 4.0f )
                {
                    if( distanceToPlayer < 1 )
                    {
                        move = false;
                        SetStayAndAttackAnimation();
                        if( ruGetCurrentAnimation( attackHand )->GetCurrentFrame() == animAttack.GetBeginFrame() )
                            attackDone = false;
                        if( ruGetCurrentAnimation( attackHand )->GetCurrentFrame() == animAttack.GetEndFrame() - 5 && !attackDone )
                        {
                            attackDone = true;
                            pPlayer->Damage( 20 );
                            ruPlaySound( hitFleshWithAxeSound, true );
                        }
                    }
                    else
                        SetRunAndAttackAnimation();
                }
                else
                    SetRunAnimation();
            }
        }
    }
    else if( moveType == MoveTypeGoToDestination )
    {
        GraphVertex * destNearestVertex = pathfinder.GetVertexNearestTo( destination, &currentDestIndex);
        GraphVertex * enemyNearestVertex = pathfinder.GetVertexNearestTo( ruGetNodePosition( body ) );
        if( currentDestIndex != lastDestIndex )   // means player has moved to another waypoint
        {
            pathfinder.BuildPath( enemyNearestVertex, destNearestVertex, currentPath );
            destWaypointNum = GetVertexIndexNearestTo( currentPath[ currentPath.size() - 1 ]->position );
            currentWaypointNum = GetVertexIndexNearestTo( currentPath[0]->position );
            if( currentWaypointNum > destWaypointNum )
            {
                int temp = currentWaypointNum;
                currentWaypointNum = destWaypointNum;
                destWaypointNum = temp;
            }
            lastDestIndex = currentDestIndex;
        }
        SetRunAnimation();
    }

    // check doors
    for( auto d : Door::all )
    {
        if( ( ruGetNodePosition( d->door ) - ruGetNodePosition( body )).Length2() < 2.5f )
        {
            if( d->GetState() == Door::State::Closed )
                d->Open();
        }
    }

    if( move && !reachPoint )
    {
        pathLen += 0.1f;
        if( abs( pathLen - lastPathLen ) > 3 )
        {
            lastPathLen = pathLen;
            ruPlaySound( footstepsSounds[ rand() % 4 ]);
        }
        ruVector3 speedVector = direction * runSpeed;// + Vector3( 0, -.1, 0 );
        ruMoveNode( body, speedVector );
    }

#ifdef ENEMY_ANIMATION_DEBUG
    int y = 100;
    DrawAnimationDebugInfo( model, y );
#endif
    ruSetAnimationEnabled( model, true );
    animAttack.Update();
    animIdle.Update();
    animRun.Update();
}

Enemy::Enemy( const char * file, vector<GraphVertex*> & path, vector<GraphVertex*> & patrol )
{
    pathfinder.SetVertices( path );
    patrolPoints = patrol;
    currentPatrolPoint = 0;

    pathLen = 0.0f;
    lastPathLen = 0.0f;
    bodyHeight = 1.0f;

    body = ruCreateSceneNode();
    ruSetCapsuleBody( body, bodyHeight, 0.25f );
    ruSetAngularFactor( body, ruVector3( 0, 0, 0 ));
    ruSetNodePosition( body, ruVector3( 5, 1, -2.5 ));
    ruSetNodeMass( body, 100 );
    ruSetNodeFriction( body, 0 );

    model = ruLoadScene( file );
    ruAttachNode( model, body );
    ruSetNodePosition( model, ruVector3( 0, -0.5f, 0 ));

    FindBodyparts();

    angleTo = 0.0f;
    angle = 0.0f;

    damageTimer = ruCreateTimer();

    CreateSounds();

    CreateAnimations();

    runSpeed = 1.5f;

    moveType = MoveTypeGoToDestination;

    destWaypointNum = 0;
    lastDestIndex = -1;
    playerDetected = false;
    detectedTimer = ruCreateTimer();
    int a = 0;
}

void Enemy::SetWalkAnimation()
{
    SetCommonAnimation( &animWalk );
}


void Enemy::SetStayAndAttackAnimation()
{
    SetCommonAnimation( &animIdle );
    SetTorsoAnimation( &animAttack );
}

void Enemy::SetRunAndAttackAnimation()
{
    SetCommonAnimation( &animAttack );
    SetLegsAnimation( &animRun );
}

void Enemy::SetRunAnimation()
{
    SetCommonAnimation( &animRun );
}

void Enemy::SetIdleAnimation()
{
    SetCommonAnimation( &animIdle );
}

void Enemy::SetCommonAnimation( ruAnimation * anim )
{
    ruSetAnimation( model, anim );
}

void Enemy::SetTorsoAnimation( ruAnimation * anim )
{
    ruSetAnimation( torsoBone, anim );
}

void Enemy::SetLegsAnimation( ruAnimation * anim )
{
    ruSetAnimation( rightLeg, anim );
    ruSetAnimation( leftLeg, anim );
    ruSetAnimation( rightLegDown, anim );
    ruSetAnimation( leftLegDown, anim );
}

void Enemy::DrawAnimationDebugInfo( ruNodeHandle node, int & y )
{
    ruAnimation * ca = ruGetCurrentAnimation( node );
    string animName;
    if( ca == &animIdle )
        animName = "Idle";
    else if ( ca == &animRun )
        animName = "Run";
    else if( ca == &animAttack )
        animName = "Attack";
    y += 16;
    ruDrawGUIText( Format(
                       "Name: %-20.20sType: %-20.20sFrame: %-8dBegin: %-8dEnd: %-8dNext: %-8d",
                       ruGetNodeName( node ),
                       animName.c_str(),
                       ca->GetCurrentFrame(),
                       ca->GetBeginFrame(),
                       ca->GetEndFrame(),
                       ca->GetNextFrame() ).c_str(), 100, y, 700, 200, pGUI->mFont, ruVector3( 200, 0, 0 ), 0 );

    for( int i = 0; i < ruGetNodeCountChildren( node ); i++ )
        DrawAnimationDebugInfo( ruGetNodeChild( node, i ), y );
}

void Enemy::CreateAnimations()
{
    // Animations
    animIdle = ruAnimation( 0, 15, 0.08, true );
    animRun = ruAnimation( 16, 34, 0.08, true );
    animAttack = ruAnimation( 35, 46, 0.035, true );
    animWalk = ruAnimation( 47, 58, 0.045, true );
}

void Enemy::CreateSounds()
{
    hitFleshWithAxeSound = ruLoadSound3D( "data/sounds/armor_axe_flesh.ogg" );
    ruAttachSound( hitFleshWithAxeSound, ruFindInObjectByName( model, "AttackHand" ));

    breathSound = ruLoadSound3D( "data/sounds/breath1.ogg" );
    ruAttachSound( breathSound, body );
    ruSetSoundVolume( breathSound, 0.25f );
    ruSetRolloffFactor( breathSound, 20 );
    ruSetSoundReferenceDistance( breathSound, 2.8 );

    screamSound = ruLoadSound3D( "data/sounds/scream_creepy_1.ogg" );
    ruSetSoundVolume( screamSound, 1.0f );
    ruAttachSound( screamSound, body );
    ruSetRolloffFactor( screamSound, 20 );
    ruSetSoundReferenceDistance( screamSound, 4 );

    footstepsSounds[ 0 ] = ruLoadSound3D( "data/sounds/step1.ogg" );
    footstepsSounds[ 1 ] = ruLoadSound3D( "data/sounds/step2.ogg" );
    footstepsSounds[ 2 ] = ruLoadSound3D( "data/sounds/step3.ogg" );
    footstepsSounds[ 3 ] = ruLoadSound3D( "data/sounds/step4.ogg" );
    for( int i = 0; i < 4; i++ )
    {
        ruAttachSound( footstepsSounds[i], body );
        ruSetSoundVolume( footstepsSounds[i], 0.75f );
        ruSetRolloffFactor( footstepsSounds[i], 10 );
        ruSetSoundReferenceDistance( footstepsSounds[i], 5 );
    }
}

int Enemy::GetVertexIndexNearestTo( ruVector3 position )
{
    if( currentPath.size() == 0 )
        return 0;;
    int nearestIndex = 0;
    for( int i = 0; i < currentPath.size(); i++ )
    {
        if( ( currentPath[i]->position - position ).Length2() < ( currentPath[nearestIndex]->position - position ).Length2() )
            nearestIndex = i;
    }
    return nearestIndex;
}

void Enemy::FindBodyparts()
{
    rightLeg = ruFindInObjectByName( model, "RightLeg" );
    leftLeg = ruFindInObjectByName( model, "LeftLeg" );
    rightLegDown = ruFindInObjectByName( model, "RightLegDown" );
    leftLegDown = ruFindInObjectByName( model, "LeftLegDown" );
    torsoBone = ruFindInObjectByName( model, "Torso" );
    attackHand = ruFindInObjectByName( model, "AttackHand" );
    head = ruFindInObjectByName( model, "HeadBone" );
}

void Enemy::Serialize( TextFileStream & out )
{
    out.WriteVector3( ruGetNodePosition( body ));
}

void Enemy::Deserialize( TextFileStream & in )
{
    ruSetNodePosition( body, in.ReadVector3( ));
}