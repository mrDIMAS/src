#include "Player.h"
#include "Menu.h"
#include "Door.h"
#include "utils.h"

Player * pPlayer = 0;

/*
========
Player::Player
========
*/
Player::Player() : Actor( 1.0f, 0.2f )
{
    mLocalization.ParseFile( localizationPath + "player.loc" );

    // Stamina vars
    mMaxStamina = 100.0f;
    mStamina = mMaxStamina;

    mFov = SmoothFloat( 75.0f, 75.0f, 80.0f );

    // Run vars
    mRunSpeedMult = 2.5f;

    mObjectThrown = false;
    mSmoothCamera = true;

    // Life vars
    mMaxLife = 100;
    mLife = mMaxLife;

    mPitch = SmoothFloat( 0.0f, -89.9f, 89.9f );
    mYaw = SmoothFloat( 0.0f );
    mStealthOffset = SmoothFloat( 0.0f, -0.45f, 0.0f );
    mHeadAngle = SmoothFloat( 0.0f, -12.50f, 12.50f );
    // State vars
    mDead = false;
    mLanded = false;
    mStealthMode = false;

    mpSheetInHands = nullptr;

    // Camera bob vars

    mCameraShakeOffset = ruVector3( 0, mHeadHeight, 0 );
    mRunCameraShakeCoeff = 1.0f;
    mCameraBobCoeff = 0;

    // Effects vars
    mFrameColor = ruVector3( 1.0f, 1.0f, 1.0f );

    // Control vars
    mouseSens = 0.5f;
    mKeyMoveForward = KEY_W;
    mKeyMoveBackward = KEY_S;
    mKeyStrafeLeft = KEY_A;
    mKeyStrafeRight = KEY_D;
    mKeyJump = KEY_Space;
    mKeyFlashLight = KEY_F;
    mKeyRun = KEY_LeftShift;
    mKeyInventory = KEY_Tab;
    mKeyUse = KEY_R;
    mKeyStealth = KEY_C;
    mKeyLookLeft = KEY_Q;
    mKeyLookRight = KEY_E;
    // GUI vars
    mStaminaAlpha = SmoothFloat( 255.0, 0.0f, 255.0f );
    mHealthAlpha = SmoothFloat( 255.0, 0.0f, 255.0f );

    mpCurrentWay = nullptr;

    mStealthSign = ruGetTexture( "data/textures/effects/eye.png" );

    LoadGUIElements();
    CreateCamera();
    CreateFlashLight();
    LoadSounds();
    CompleteObjective();
    SetDirtFootsteps();

    ruSetNodeName( mBody, "Player" );
}

/*
========
Player::SetPlaceDescription
========
*/
void Player::SetPlaceDescription( string desc )
{
    mPlaceDesc = desc;
    mPlaceDescTimer = 240;
}

/*
========
Player::DrawStatusBar
========
*/
void Player::DrawStatusBar()
{
    if( mMoved ) {
        mStaminaAlpha.SetTarget( 255 );
        mHealthAlpha.SetTarget( 255 );
    } else {
        mStaminaAlpha.SetTarget( 50 );
        mHealthAlpha.SetTarget( 50 );
    }

    mStaminaAlpha.ChaseTarget( 8.0f * g_dt );
    mHealthAlpha.ChaseTarget( 8.0f * g_dt );

    float scale = 2;
    int w = 512.0f / scale;
    int h = 256.0f / scale;

    ruDrawGUIRect( 0, ruGetResolutionHeight() - h, w, h, mStatusBar, ruVector3( 255, 255, 255 ), mStaminaAlpha );

    int segCount = mStamina / 5;
    for( int i = 0; i < segCount; i++ )
        ruDrawGUIRect( 44 + i * ( 8 + 2 ), ruGetResolutionHeight() - 3 * 15, 8, 16, pGUI->staminaBarImg, ruVector3( 255, 255, 255 ), mStaminaAlpha );

    segCount = mLife / 5;
    for( int i = 0; i < segCount; i++ )
        ruDrawGUIRect( 44 + i * ( 8 + 2 ), ruGetResolutionHeight() - 4 * 26, 8, 16, pGUI->lifeBarImg, ruVector3( 255, 255, 255 ), mHealthAlpha );
}

/*
========
Player::CanJump
========
*/
bool Player::IsCanJump( )
{
    ruNodeHandle legBump = ruCastRay( ruGetNodePosition( mBody ) + ruVector3( 0, 0.1, 0 ), ruGetNodePosition( mBody ) - ruVector3( 0, bodyHeight, 0 ), 0 );
    if( legBump.IsValid() )
        return true;
    else
        return false;
}

/*
========
Player::GotItemAnyOfType
========
*/
int Player::IsGotItemAnyOfType( int type )
{
    for( auto item : mInventory.mItemList ) {
        if( item->mType == type )
            return true;
    }
    return false;
}

/*
========
Player::UseStamina
========
*/
bool Player::UseStamina( float required )
{
    if( mStamina - required < 0 )
        return false;

    mStamina -= required;

    return true;
}

/*
========
Player::Damage
========
*/
void Player::Damage( float dmg )
{
    mLife -= dmg;
    if( mLife < 0.0f ) {
        if( !mDead ) {
            ruSetAngularFactor( mBody, ruVector3( 1.0f, 1.0f, 1.0f ));
            ruSetNodeFriction( mBody, 1.0f );
            ruSetNodeAnisotropicFriction( mBody, ruVector3( 1.0f, 1.0f, 1.0f ));
            ruMoveNode( mBody, ruVector3( 1.0f, 1.0f, 1.0f ));
        }
        mDead = true;
        mLife = 0.0f;
        mpFlashlight->SwitchOff();
        mpCamera->FadePercent( 5 );
        mpCamera->SetFadeColor( ruVector3( 70.0f, 0.0f, 0.0f ) );
    }
}

/*
========
Player::AddItem
========
*/
void Player::AddItem( Item * itm )
{
    if( !itm )
        return;

    for( auto item : mInventory.mItemList )
        if( item == itm )
            return;

    ruFreeze( itm->mObject );
    ruDetachNode( itm->mObject );
    itm->mInInventory = true;

    ruSetNodePosition( itm->mObject, ruVector3( 10000, 10000, 10000 )); // far far away

    mInventory.mItemList.push_back( itm );
}

/*
========
Player::UpdateInventory
========
*/
void Player::UpdateInventory()
{
    if( ruIsKeyHit( mKeyInventory ) )
        mInventory.opened = !mInventory.opened;

    mInventory.Update();
}

/*
========
Player::SetObjective
========
*/
void Player::SetObjective( string text )
{
    string objectiveText = mLocalization.GetString( "currentObjective" );
    objectiveText += text;

    mGoal.SetText( objectiveText );

    mObjectiveDone = false;
}

/*
========
Player::CompleteObjective
========
*/
void Player::CompleteObjective()
{
    SetObjective( mLocalization.GetString( "objectiveUnknown" ));

    mObjectiveDone = true;
}

/*
========
Player::UpdateMouseLook
========
*/
void Player::UpdateMouseLook()
{
    if( mpCurrentWay )
        mSmoothCamera = false;
    else
        mSmoothCamera = true;

    if( !mInventory.opened ) {
        float mouseSpeed =  mouseSens / 2.0f;
        if( mpCurrentWay ) {
            if( !mpCurrentWay->IsFreeLook() )
                mouseSpeed = 0.0f;
        }
        mPitch.SetTarget( mPitch.GetTarget() + ruGetMouseYSpeed() * mouseSpeed );
        mYaw.SetTarget( mYaw.GetTarget() - ruGetMouseXSpeed() * mouseSpeed );
    }

    if( mSmoothCamera ) {
        mPitch.ChaseTarget( 14.0f * g_dt );
        mYaw.ChaseTarget( 14.0f * g_dt );
    } else {
        mYaw = mYaw.GetTarget();
        mPitch = mPitch.GetTarget();
    }


    ruSetNodeRotation( mpCamera->mNode, ruQuaternion( ruVector3( 1, 0, 0 ), mPitch ) );
    ruSetNodeRotation( mBody, ruQuaternion( ruVector3( 0, 1, 0 ), mYaw ) );

    mHeadAngle.SetTarget( 0.0f );
    if( ruIsKeyDown( mKeyLookLeft )) {
        ruVector3 rayBegin = ruGetNodePosition( mBody ) + ruVector3( bodyWidth / 2, 0, 0 );
        ruVector3 rayEnd = rayBegin + ruGetNodeRightVector( mBody ) * 10.0f;
        ruVector3 hitPoint;
        ruNodeHandle leftIntersection = ruCastRay( rayBegin, rayEnd, &hitPoint );
        bool canLookLeft = true;
        if( leftIntersection.IsValid() ) {
            float dist2 = ( hitPoint - ruGetNodePosition( mBody )).Length2();
            if( dist2 < 0.4 )
                canLookLeft = false;
        }
        if( canLookLeft )
            mHeadAngle.SetTarget( mHeadAngle.GetMin() );
    }

    if( ruIsKeyDown( mKeyLookRight )) {
        ruVector3 rayBegin = ruGetNodePosition( mBody ) - ruVector3( bodyWidth / 2, 0, 0 );
        ruVector3 rayEnd = rayBegin - ruGetNodeRightVector( mBody ) * 10.0f;
        ruVector3 hitPoint;
        ruNodeHandle rightIntersection = ruCastRay( rayBegin, rayEnd, &hitPoint );
        bool canLookRight = true;
        if( rightIntersection.IsValid() ) {
            float dist2 = ( hitPoint - ruGetNodePosition( mBody )).Length2();
            if( dist2 < 0.4 )
                canLookRight = false;
        }
        if( canLookRight )
            mHeadAngle.SetTarget( mHeadAngle.GetMax() );
    }
    mHeadAngle.ChaseTarget( 17.0f * g_dt );
    ruSetNodeRotation( mHead, ruQuaternion( ruVector3( 0, 0, 1 ), mHeadAngle ));
}

/*
========
Player::UpdateJumping
========
*/
void Player::UpdateJumping()
{
    // do ray test, to determine collision with objects above camera
    ruNodeHandle headBumpObject = ruCastRay( ruGetNodePosition( mBody ) + ruVector3( 0, bodyHeight * 0.98, 0 ), ruGetNodePosition( mBody ) + ruVector3( 0, 1.02 * bodyHeight, 0 ), nullptr );

    if( ruIsKeyHit( mKeyJump ) ) {
        if( IsCanJump() ) {
            mJumpTo = ruVector3( 0, 150, 0 );
            mLanded = false;
        }
    }

    mGravity = mGravity.Lerp( mJumpTo, 40.0f * g_dt );

    if( mGravity.y >= mJumpTo.y )
        mLanded = true;

    if( mLanded || headBumpObject.IsValid() ) {
        mJumpTo = ruVector3( 0, -150, 0 );
        if( IsCanJump() )
            mJumpTo = ruVector3( 0, 0, 0 );
    };
}

/*
========
Player::UpdateMoving
========
*/
void Player::UpdateMoving()
{
    for( auto cw : Way::sWayList ) {
        if( cw->IsEnterPicked() ) {
            if( !cw->IsPlayerInside() )
                DrawTip( Format( mLocalization.GetString( "crawlIn" ), GetKeyName( mKeyUse )));

            if( IsUseButtonHit() )
                cw->Enter();
        }
    }

    for( auto door : Door::all ) {
        door->DoInteraction();

        if( door->IsPickedByPlayer() ) {
            DrawTip( Format( mLocalization.GetString( "openClose" ), GetKeyName( mKeyUse )));

            if( IsUseButtonHit() )
                door->SwitchState();
        }
    }

    if( ruIsKeyHit( mKeyStealth ))
        mStealthMode = !mStealthMode;

    if( mpCurrentWay ) {
        mStealthMode = false;

        mpCurrentWay->DoEntering();

        if( !mpCurrentWay->IsFreeLook() )
            mpCurrentWay->LookAtTarget();

        if( mpCurrentWay->IsPlayerInside() ) {
            bool move = false;

            if( ruIsKeyDown( mKeyMoveForward )) {
                mpCurrentWay->SetDirection( Way::Direction::Forward );
                move = true;
            }
            if( ruIsKeyDown( mKeyMoveBackward )) {
                mpCurrentWay->SetDirection( Way::Direction::Backward );
                move = true;
            }

            if( move ) {
                mpCurrentWay->DoPlayerCrawling();

                mMoved = true;

                if( !mpCurrentWay->IsPlayerInside() )
                    mpCurrentWay = nullptr;
            } else {
                ruMoveNode( pPlayer->mBody, ruVector3( 0, 0, 0 ));

                mMoved = false;
            }
        }
    } else {
        ruVector3 look = ruGetNodeLookVector( mBody );
        ruVector3 right = ruGetNodeRightVector( mBody );

        mSpeedTo = ruVector3( 0, 0, 0 );

        if( ruIsKeyDown( mKeyMoveForward ))
            mSpeedTo = mSpeedTo + look;
        if( ruIsKeyDown( mKeyMoveBackward ))
            mSpeedTo = mSpeedTo - look;
        if( ruIsKeyDown( mKeyStrafeLeft ))
            mSpeedTo = mSpeedTo + right;
        if( ruIsKeyDown( mKeyStrafeRight ))
            mSpeedTo = mSpeedTo - right;

        mMoved = mSpeedTo.Length2() > 0;

        if( mMoved )
            mSpeedTo = mSpeedTo.Normalize();

        UpdateJumping();

        mRunCameraShakeCoeff = 1.0f;
        mFov.SetTarget( mFov.GetMin() );

        mRunning = false;
        if( ruIsKeyDown( mKeyRun ) && mMoved ) {
            if( mStamina > 0 ) {
                mSpeedTo = mSpeedTo * mRunSpeedMult;
                mStamina -= 8.0f * g_dt ;
                mFov.SetTarget( mFov.GetMax() );
                mRunCameraShakeCoeff = 1.425f;
                mRunning = true;
            }
        } else {
            if( mStamina < mMaxStamina )
                mStamina += 16.0f * g_dt;
        }

        mSpeedTo = mSpeedTo * ( mStealthMode ? 0.4f : 1.0f ) ;

        mFov.ChaseTarget( 4.0f * g_dt );
        ruSetCameraFOV( mpCamera->mNode, mFov );

        mSpeed = mSpeed.Lerp( mSpeedTo + mGravity, 10.0f * g_dt );
        Move( mSpeed * ruVector3( 100, 1, 100 ), g_dt );
    }

    UpdateCameraShake();
}

/*
========
Player::ComputeStealth
========
*/
void Player::ComputeStealth()
{
    bool inLight = false;
    ruVector3 pos = ruGetNodePosition( mBody );
    ruNodeHandle affectLight;
    for( int i = 0; i < ruGetWorldPointLightCount(); i++ ) {
        if( ruIsLightSeePoint( ruGetWorldPointLight( i ), pos )) {
            inLight = true;
            affectLight = ruGetWorldPointLight( i );
            break;
        }
    }
    if( !inLight ) {
        for( int i = 0; i < ruGetWorldSpotLightCount(); i++ ) {
            if( ruIsLightSeePoint( ruGetWorldSpotLight( i ), pos )) {
                inLight = true;
                affectLight = ruGetWorldSpotLight( i );
                break;
            }
        }
    }

    if( mpFlashlight->on )
        inLight = true;

    mStealthFactor = 0.0f;

    if( inLight ) {
        mStealthFactor += mStealthMode ? 0.25f : 0.5f;
        mStealthFactor += mMoved ? 0.5f : 0.0f;
        mStealthFactor += mRunning ? 1.0f : 0.0f;
    } else {
        mStealthFactor += mStealthMode ? 0.0f : 0.25f;
        mStealthFactor += mMoved ? 0.1f : 0.0f;
        mStealthFactor += mRunning ? 0.25f : 0.0f;
    }


    for( auto snd : mFootstepList ) {
        if( mStealthMode )
            ruSetSoundVolume( snd, 0.15f );
        else
            ruSetSoundVolume( snd, 0.4f );
    }

    int alpha = ( 255 * ( ( mStealthFactor > 1.05f ) ? 1.0f : ( mStealthFactor + 0.05f ) ) );
    if (alpha > 255 )
        alpha = 255;
    ruVector3 color = ( mStealthFactor < 1.05f ) ? ruVector3( 255, 255, 255 ) : ruVector3( 255, 0, 0 );
    ruDrawGUIRect( ruGetResolutionWidth() / 2 - 32, 200, 64, 32, mStealthSign, color, alpha );
}
/*
========
Player::Update
========
*/
void Player::Update( )
{
    UpdateFright();
    mpCamera->Update();

    if( pMainMenu->mVisible )
        return;

    DrawGUIElements();
    DrawStatusBar();
    UpdateFlashLight();

    if( mDead )
        return;
    mTip.AnimateAndDraw();
    UpdateMouseLook();
    UpdateMoving();
    ComputeStealth();
    UpdatePicking();
    UpdateItemsHandling();
    UpdateEnvironmentDamaging();
    UpdateInventory();
    DrawSheetInHands();
    DescribePickedObject();
}

/*
========
Player::LoadGUIElements
========
*/
void Player::LoadGUIElements()
{
    mObjectDragUpCursor = ruGetTexture( "data/gui/up.png" );
    mObjectDragDownCursor = ruGetTexture( "data/gui/down.png" );
    mItemPickupSound = ruLoadSound2D( "data/sounds/menuhit.ogg" );
    mStatusBar = ruGetTexture( "data/gui/statusbar.png" );
}

/*
========
Player::CreateCamera
========
*/
void Player::CreateCamera()
{
    mHeadHeight = 2.4;

    mHead = ruCreateSceneNode();
    ruAttachNode( mHead, mBody );
    ruSetNodePosition( mHead, ruVector3( 0, -2.0f, 0.0f ));
    mpCamera = new GameCamera( mFov );
    ruAttachNode( mpCamera->mNode, mHead );
    mCameraOffset = ruVector3( 0, mHeadHeight, 0 );
    mCameraShakeOffset = ruVector3( 0, mHeadHeight, 0 );

    // Pick
    mPickPoint = ruCreateSceneNode();
    ruAttachNode( mPickPoint, mpCamera->mNode );
    ruSetNodePosition( mPickPoint, ruVector3( 0, 0, 0.1 ));

    mItemPoint = ruCreateSceneNode();
    ruAttachNode( mItemPoint, mpCamera->mNode );
    ruSetNodePosition( mItemPoint, ruVector3( 0, 0, 1.0f ));
}

/*
========
Player::SetRockFootsteps
========
*/
void Player::SetRockFootsteps()
{
    for( auto s : mFootstepList )
        ruFreeSound( s );

    mFootstepList.clear();

    mFootstepList.push_back( ruLoadSound3D( "data/sounds/stonestep1.ogg" ) );
    mFootstepList.push_back( ruLoadSound3D( "data/sounds/stonestep2.ogg" ) );
    mFootstepList.push_back( ruLoadSound3D( "data/sounds/stonestep3.ogg" ) );
    mFootstepList.push_back( ruLoadSound3D( "data/sounds/stonestep4.ogg" ) );
    mFootstepList.push_back( ruLoadSound3D( "data/sounds/stonestep5.ogg" ) );
    mFootstepList.push_back( ruLoadSound3D( "data/sounds/stonestep6.ogg" ) );
    mFootstepList.push_back( ruLoadSound3D( "data/sounds/stonestep7.ogg" ) );
    mFootstepList.push_back( ruLoadSound3D( "data/sounds/stonestep8.ogg" ) );
    mFootstepList.push_back( ruLoadSound3D( "data/sounds/stonestep9.ogg" ) );
    mFootstepList.push_back( ruLoadSound3D( "data/sounds/stonestep10.ogg" ) );
    mFootstepList.push_back( ruLoadSound3D( "data/sounds/stonestep11.ogg" ) );
    mFootstepList.push_back( ruLoadSound3D( "data/sounds/stonestep12.ogg" ) );

    for( auto s : mFootstepList ) {
        ruAttachSound( s, mBody );
        ruSetSoundVolume( s, 0.45f );
    }

    mFootstepsType = FootstepsType::Rock;
}

/*
========
Player::SetDirtFootsteps
========
*/
void Player::SetDirtFootsteps()
{
    for( auto s : mFootstepList )
        ruFreeSound( s );

    mFootstepList.clear();

    mFootstepList.push_back( ruLoadSound3D( "data/sounds/dirt1.ogg" ) );
    mFootstepList.push_back( ruLoadSound3D( "data/sounds/dirt2.ogg" ) );
    mFootstepList.push_back( ruLoadSound3D( "data/sounds/dirt3.ogg" ) );
    mFootstepList.push_back( ruLoadSound3D( "data/sounds/dirt4.ogg" ) );

    for( auto s : mFootstepList ) {
        ruAttachSound( s, mBody );
        ruSetSoundVolume( s, 0.45f );
    }

    mFootstepsType = FootstepsType::Dirt;
}

/*
========
Player::LoadSounds
========
*/
void Player::LoadSounds()
{
    mLighterCloseSound = ruLoadSound3D( "data/sounds/lighter_close.ogg" );
    mLighterOpenSound = ruLoadSound3D( "data/sounds/lighter_open.ogg" );

    ruAttachSound( mLighterCloseSound, mpCamera->mNode );
    ruAttachSound( mLighterOpenSound, mpCamera->mNode );

    mHeartBeatSound = ruLoadSound2D( "data/sounds/heart.ogg" );
    mBreathSound = ruLoadSound2D( "data/sounds/breath.ogg" );

    ruSetSoundReferenceDistance( mHeartBeatSound, 100.0f );
    ruSetSoundReferenceDistance( mBreathSound, 100.0f );

    mBreathVolume = SmoothFloat( 0.1f );
    mHeartBeatVolume = SmoothFloat( 0.15f );
    mHeartBeatPitch = SmoothFloat( 1.0f );
    mBreathPitch = SmoothFloat( 1.0f );
}

/*
========
Player::DoFright
========
*/
void Player::DoFright()
{
    mBreathVolume.SetTarget( 0.1f );
    mBreathVolume.Set( 0.25f );
    mHeartBeatVolume.SetTarget( 0.15f );
    mHeartBeatVolume.Set( 0.45f );
    mHeartBeatPitch.Set( 2.0f );
    mHeartBeatPitch.SetTarget( 1.0f );
    mBreathPitch.Set( 1.5f );
    mBreathPitch.SetTarget( 1.0f );
}

/*
========
Player::UpdateFright
========
*/
void Player::UpdateFright()
{
    mBreathVolume.ChaseTarget( 0.075f );
    mHeartBeatVolume.ChaseTarget( 0.075f );
    mHeartBeatPitch.ChaseTarget( 0.0025f );
    mBreathPitch.ChaseTarget( 0.0025f );

    /*
    SetVolume( breathSound, breathVolume );
    SetVolume( heartBeatSound, heartBeatVolume );

    PlaySoundSource( heartBeatSound, true );
    PlaySoundSource( breathSound, true );

    SetPitch( breathSound, breathPitch );
    SetPitch( heartBeatSound, heartBeatPitch );*/
}

/*
========
Player::UpdateCameraShake
========
*/
void Player::UpdateCameraShake()
{
    static int stepPlayed = 0;

    if( mMoved ) {
        mCameraBobCoeff += 7.5 * mRunCameraShakeCoeff * g_dt;

        float xOffset = sinf( mCameraBobCoeff ) * ( mRunCameraShakeCoeff * mRunCameraShakeCoeff ) * 0.075f;
        float yOffset = abs( xOffset );

        if( yOffset < 0.02 && !ruIsSoundPlaying( mFootstepList[ stepPlayed ] ) ) {
            stepPlayed = rand() % mFootstepList.size();

            ruPlaySound( mFootstepList[ stepPlayed ]);
        }

        mCameraShakeOffset = ruVector3( xOffset, yOffset + mHeadHeight, 0 );
    } else
        mCameraBobCoeff = 0;
    if( mStealthMode )
        mStealthOffset.SetTarget( mStealthOffset.GetMin() );
    else
        mStealthOffset.SetTarget( mStealthOffset.GetMax() );
    mStealthOffset.ChaseTarget( 0.15f );
    mCameraOffset = mCameraOffset.Lerp( mCameraShakeOffset, 0.25f );
    ruSetNodePosition( mpCamera->mNode, mCameraOffset + ruVector3( 0.0f, mStealthOffset, 0.0f ) );
}

/*
========
Player::DrawSheetInHands
========
*/
void Player::DrawSheetInHands()
{
    if( mpSheetInHands ) {
        mpSheetInHands->Draw();

        mPickedObjectDesc = mpSheetInHands->desc;

        mPickedObjectDesc += mLocalization.GetString( "sheetOpen" );

        if( ruIsMouseHit( MB_Right ) ||  ( ruGetNodePosition( mpSheetInHands->mObject) - ruGetNodePosition( mBody )).Length2() > 2 )
            CloseCurrentSheet();
    }
}

/*
========
Player::DescribePickedObject
========
*/
void Player::DescribePickedObject()
{
    // Change cursor first
    if( mNearestPickedNode.IsValid() ) {
        if( IsObjectHasNormalMass( mNearestPickedNode ))
            ruDrawGUIRect( ruGetResolutionWidth() / 2 - 16, ruGetResolutionHeight() / 2 - 16, 32, 32, mObjectDragUpCursor, ruVector3( 255, 255, 255 ), 180 );
    } else {
        if( mNodeInHands.IsValid() )
            ruDrawGUIRect( ruGetResolutionWidth() / 2 - 16, ruGetResolutionHeight() / 2 - 16, 32, 32, mObjectDragDownCursor, ruVector3( 255, 255, 255 ), 180 );
        else
            ruDrawGUIText( "+", ruGetResolutionWidth() / 2 - 16, ruGetResolutionHeight() / 2 - 16, 32, 32, pGUI->mFont, ruVector3( 255, 0, 0 ), 1, 180 );
    }
    // Then describe object
    ruDrawGUIText( mPickedObjectDesc.c_str(), ruGetResolutionWidth() / 2 - 256, ruGetResolutionHeight() - 200, 512, 128, pGUI->mFont, ruVector3( 255, 0, 0 ), 1 );
}

/*
========
Player::UpdateEnvironmentDamaging
========
*/
void Player::UpdateEnvironmentDamaging()
{
    /*
    for( int i = 0; i < GetContactCount( body ); i++ ) {
        Contact contact = GetContact( body, i );

        if( contact.impulse > 30 ) {
            Damage( contact.impulse / 5 );
        }
    }*/
}

/*
========
Player::UpdateItemsHandling
========
*/
void Player::UpdateItemsHandling()
{
    if( mNearestPickedNode.IsValid() ) {
        if( IsUseButtonHit() ) {
            Item * itm = Item::GetByObject( mNearestPickedNode );

            if( itm ) {
                AddItem( itm );

                ruPlaySound( mItemPickupSound );
            }

            Sheet * sheet = Sheet::GetSheetByObject( mNearestPickedNode );

            if( mpSheetInHands )
                CloseCurrentSheet();
            else {
                if( sheet ) {
                    mpSheetInHands = sheet;
                    ruHideNode( mpSheetInHands->mObject );
                    ruPlaySound( Sheet::paperFlip );
                }
            }
        }
    }

    if( mNodeInHands.IsValid() ) {
        ruVector3 ppPos = ruGetNodePosition( mItemPoint );
        ruVector3 objectPos = ruGetNodePosition( mNodeInHands );
        ruVector3 dir = ppPos - objectPos;
        if( ruIsMouseDown( MB_Left ) ) {
            ruMoveNode( mNodeInHands,  dir * 6 );

            ruSetNodeAngularVelocity( mNodeInHands, ruVector3( 0, 0, 0 ));

            if( ruIsMouseDown( MB_Right ) ) {
                if( UseStamina( ruGetNodeMass( mNodeInHands )  ))
                    ruMoveNode( mNodeInHands, ( ppPos - ruGetNodePosition( mpCamera->mNode )).Normalize() * 6 );

                mObjectThrown = true;
                mNodeInHands.Invalidate();
            }
        } else {
            ruSetNodeAngularVelocity( mNodeInHands, ruVector3( 1, 1, 1 ));

            mNodeInHands.Invalidate();
        }
    }

    if( !ruIsMouseDown( MB_Left ))
        mObjectThrown = false;
}

/*
========
Player::UpdatePicking
========
*/
void Player::UpdatePicking()
{
    ruVector3 pickPosition;

    mPickedNode = ruRayPick( ruGetResolutionWidth() / 2, ruGetResolutionHeight() / 2, &pickPosition );

    mNearestPickedNode.Invalidate();

    if( mPickedNode.IsValid() && !mNodeInHands.IsValid()  ) {
        mNodeInHands.Invalidate();

        ruVector3 ppPos = ruGetNodePosition( mPickPoint );
        ruVector3 dir = ppPos - pickPosition;

        Item * itm = Item::GetByObject( mPickedNode );
        Sheet * sheet = Sheet::GetSheetByObject( mPickedNode );

        if( dir.Length2() < 1.5f ) {
            mNearestPickedNode = mPickedNode;

            if( itm ) {
                mPickedObjectDesc = itm->mName;
                mPickedObjectDesc += Format( mLocalization.GetString( "itemPick" ), GetKeyName( mKeyUse) );
            } else if( sheet ) {
                mPickedObjectDesc = sheet->desc;
                mPickedObjectDesc += Format( mLocalization.GetString( "sheetPick" ), GetKeyName( mKeyUse ));
            } else {
                if( IsObjectHasNormalMass( mPickedNode ) && !ruIsNodeFrozen( mPickedNode ))
                    DrawTip( mLocalization.GetString( "objectPick" ) );
            }

            if( ruIsMouseDown( MB_Left ) ) {
                if( IsObjectHasNormalMass( mPickedNode )) {
                    if( !ruIsNodeFrozen( mPickedNode ) && !mObjectThrown )
                        mNodeInHands = mPickedNode;
                }
            }
        }

        if( !itm && !sheet )
            mPickedObjectDesc = " ";
    }
}

/*
========
Player::CreateFlashLight
========
*/
void Player::CreateFlashLight()
{
    mpFlashlight = new Flashlight();

    mpFlashlight->Attach( mpCamera->mNode );

    mpFlashLightItem = new Item( mpFlashlight->model, Item::Flashlight );
    mInventory.mItemList.push_back( mpFlashLightItem );
}

/*
========
Player::UpdateFlashLight
========
*/
void Player::UpdateFlashLight()
{
    mpFlashlight->Update();

    mpFlashLightItem->mContent = mpFlashlight->charge;

    if( ruIsKeyHit( mKeyFlashLight ) )
        mpFlashlight->Switch();
}

/*
========
Player::DrawGUIElements
========
*/
void Player::DrawGUIElements()
{
    int alpha = mPlaceDescTimer < 50 ? 255.0f * (float)mPlaceDescTimer / 50.0f : 255;
    ruDrawGUIText( mPlaceDesc.c_str(), ruGetResolutionWidth() - 300, ruGetResolutionHeight() - 200, 200, 200, pGUI->mFont, ruVector3( 255, 255, 255 ), 1, alpha );

    if( mPlaceDescTimer )
        mPlaceDescTimer--;

    mGoal.AnimateAndRender();
}

/*
========
Player::FreeHands
========
*/
void Player::FreeHands()
{
    mNodeInHands.Invalidate();
}

/*
========
Player::ChargeFlashLight
========
*/
void Player::ChargeFlashLight( Item * fuel )
{
    mpFlashlight->Fuel();
}

/*
========
Player::~Player
========
*/
Player::~Player()
{
    delete mpFlashlight;
    delete mpCamera;
    delete mpFlashLightItem;
}

/*
========
Player::SetMetalFootsteps
========
*/
void Player::SetMetalFootsteps()
{
    for( auto s : mFootstepList )
        ruFreeSound( s );

    mFootstepList.clear();

    mFootstepList.push_back( ruLoadSound3D( "data/sounds/footsteps/FootStep_shoe_metal_step1.ogg" ) );
    mFootstepList.push_back( ruLoadSound3D( "data/sounds/footsteps/FootStep_shoe_metal_step2.ogg" ) );
    mFootstepList.push_back( ruLoadSound3D( "data/sounds/footsteps/FootStep_shoe_metal_step3.ogg" ) );
    mFootstepList.push_back( ruLoadSound3D( "data/sounds/footsteps/FootStep_shoe_metal_step4.ogg" ) );

    for( auto s : mFootstepList ) {
        ruAttachSound( s, mBody );
        ruSetSoundVolume( s, 0.55f );
    }

    mFootstepsType = FootstepsType::Metal;
}

/*
========
Player::SetFootsteps
========
*/
void Player::SetFootsteps( FootstepsType ft )
{
    if( ft == FootstepsType::Dirt )
        SetDirtFootsteps();
    if( ft == FootstepsType::Rock )
        SetRockFootsteps();
    if( ft == FootstepsType::Metal )
        SetMetalFootsteps();
}

/*
========
Player::DrawTip
========
*/
void Player::DrawTip( string text )
{
    ruDrawGUIText( text.c_str(), ruGetResolutionWidth() / 2 - 256, ruGetResolutionHeight() - 200, 512, 128, pGUI->mFont, ruVector3( 255, 0, 0 ), 1 );
}

/*
========
Player::IsUseButtonHit
========
*/
bool Player::IsUseButtonHit()
{
    return ruIsKeyHit( mKeyUse );
}

/*
========
Player::IsObjectHasNormalMass
========
*/
bool Player::IsObjectHasNormalMass( ruNodeHandle node )
{
    return ruGetNodeMass( node ) > 0 && ruGetNodeMass( node ) < 40;
}

/*
========
Player::DeserializeWith
========
*/
void Player::DeserializeWith( TextFileStream & in )
{
    ruSetNodeLocalPosition( mBody, in.ReadVector3() );

    in.ReadBoolean( mSmoothCamera );
    in.ReadFloat( mRunCameraShakeCoeff );

    mFootstepsType = (FootstepsType)in.ReadInteger();

    mPitch.Deserialize( in );
    mYaw.Deserialize( in );
    in.ReadVector3( mSpeed );
    in.ReadVector3( mSpeedTo );
    in.ReadVector3( mGravity );
    in.ReadVector3( mJumpTo );

    mpCurrentWay = Way::GetByObject( ruFindByName( in.Readstring().c_str() ));
    if( mpCurrentWay )
        ruFreeze( pPlayer->mBody );

    in.ReadBoolean( mLanded );
    in.ReadFloat( mStamina );
    in.ReadFloat( mLife );
    in.ReadFloat( mMaxLife );
    in.ReadFloat( mMaxStamina );
    in.ReadFloat( mRunSpeedMult );
    mFov.Deserialize( in );
    in.ReadFloat( mCameraBobCoeff );

    in.ReadVector3( mCameraOffset );
    in.ReadVector3( mCameraShakeOffset );

    in.ReadFloat( mHeadHeight );

    in.ReadBoolean( mObjectThrown );
    in.ReadBoolean( mDead );

    in.ReadVector3( mFrameColor );

    in.ReadBoolean( mMoved );

    in.ReadInteger( mPlaceDescTimer );

    mStaminaAlpha.Deserialize( in );
    mHealthAlpha.Deserialize( in );

    in.ReadBoolean( mMoved );
    in.ReadBoolean( mObjectiveDone );

    mBreathVolume.Deserialize( in );
    mHeartBeatVolume.Deserialize( in );
    mHeartBeatPitch.Deserialize( in );
    mBreathPitch.Deserialize( in );

    mpSheetInHands = Sheet::GetByObject( ruFindByName( in.Readstring().c_str() ));

    in.ReadInteger( mKeyMoveForward );
    in.ReadInteger( mKeyMoveBackward );
    in.ReadInteger( mKeyStrafeLeft );
    in.ReadInteger( mKeyStrafeRight );
    in.ReadInteger( mKeyJump );
    in.ReadInteger( mKeyFlashLight );
    in.ReadInteger( mKeyRun );
    in.ReadInteger( mKeyInventory );
    in.ReadInteger( mKeyUse );

    mStealthMode = in.ReadBoolean();

    mpFlashlight->DeserializeWith( in );

    mTip.Deserialize( in );
    //headAngle.Deserialize( in );

    mpCamera->FadePercent( 100 );
    mpCamera->SetFadeColor( ruVector3( 255, 255, 255 ) );
    ruSetNodeFriction( mBody, 0 );
}

/*
========
Player::SerializeWith
========
*/
void Player::SerializeWith( TextFileStream & out )
{
    ruUnfreeze( mBody );
    out.WriteVector3( ruGetNodeLocalPosition( mBody ));
    ruSetAngularFactor( mBody, ruVector3( 0, 0, 0 ));

    out.WriteBoolean( mSmoothCamera );
    out.WriteFloat( mRunCameraShakeCoeff );
    out.WriteInteger((int)mFootstepsType );
    mPitch.Serialize( out );
    mYaw.Serialize( out );
    out.WriteVector3( mSpeed );
    out.WriteVector3( mSpeedTo );
    out.WriteVector3( mGravity );
    out.WriteVector3( mJumpTo );

    out.WriteString( mpCurrentWay ? ruGetNodeName( mpCurrentWay->GetEnterZone()) : "undefinedWay" );

    out.WriteBoolean( mLanded );
    out.WriteFloat( mStamina );
    out.WriteFloat( mLife );
    out.WriteFloat( mMaxLife );
    out.WriteFloat( mMaxStamina );
    out.WriteFloat( mRunSpeedMult );
    mFov.Serialize( out );
    out.WriteFloat( mCameraBobCoeff );

    out.WriteVector3( mCameraOffset );
    out.WriteVector3( mCameraShakeOffset );

    out.WriteFloat( mHeadHeight );

    out.WriteBoolean( mObjectThrown );
    out.WriteBoolean( mDead );

    out.WriteVector3( mFrameColor );

    out.WriteBoolean( mMoved );

    out.WriteInteger( mPlaceDescTimer );

    mStaminaAlpha.Serialize( out );
    mHealthAlpha.Serialize( out );

    out.WriteBoolean( mMoved );
    out.WriteBoolean( mObjectiveDone );

    mBreathVolume.Serialize( out );
    mHeartBeatVolume.Serialize( out );
    mHeartBeatPitch.Serialize( out );
    mBreathPitch.Serialize( out );

    out.WriteString( mpSheetInHands ? ruGetNodeName( mpSheetInHands->mObject ) : "undefinedSheet" );

    out.WriteInteger( mKeyMoveForward );
    out.WriteInteger( mKeyMoveBackward );
    out.WriteInteger( mKeyStrafeLeft );
    out.WriteInteger( mKeyStrafeRight );
    out.WriteInteger( mKeyJump );
    out.WriteInteger( mKeyFlashLight );
    out.WriteInteger( mKeyRun );
    out.WriteInteger( mKeyInventory );
    out.WriteInteger( mKeyUse );

    out.WriteBoolean( mStealthMode );

    mpFlashlight->SerializeWith( out );

    mTip.Serialize( out );
    //headAngle.Serialize( out );
}

void Player::CloseCurrentSheet()
{
    ruShowNode( mpSheetInHands->mObject );
    mpSheetInHands = 0;
    ruPlaySound( Sheet::paperFlip );
}
