#include "Player.h"
#include "Menu.h"
#include "Door.h"
#include "utils.h"

Player * pPlayer = 0;

Player::Player() : Actor( 0.7f, 0.2f ) {
    mLocalization.ParseFile( localizationPath + "player.loc" );

    // Stamina vars
    mMaxStamina = 100.0f;
    mStamina = mMaxStamina;

    mFov = SmoothFloat( 75.0f, 75.0f, 80.0f );

    // Run vars
    mRunSpeedMult = 2.5f;

    mObjectThrown = false;
    mSmoothCamera = true;
	
    mPitch = SmoothFloat( 0.0f, -89.9f, 89.9f );
    mYaw = SmoothFloat( 0.0f );
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
	
    LoadGUIElements();
    CreateCamera();
    CreateFlashLight();
    LoadSounds();
    CompleteObjective();
    SetDirtFootsteps();

    ruSetNodeName( mBody, "Player" );

    mGUIActionText = ruCreateGUIText( "Action text", ruGetResolutionWidth() / 2 - 256, ruGetResolutionHeight() - 200, 512, 128, pGUI->mFont, ruVector3( 255, 0, 0 ), 1 );

    float scale = 2;
    int w = 512.0f / scale;
    int h = 256.0f / scale;
    mGUIBackground = ruCreateGUIRect( 0, ruGetResolutionHeight() - h, w, h, mStatusBar, ruVector3( 255, 255, 255 ), mStaminaAlpha );
    for( int i = 0; i < mGUISegmentCount; i++ ) {
        mGUIStaminaBarSegment[i] = ruCreateGUIRect( 44 + i * ( 8 + 2 ), ruGetResolutionHeight() - 3 * 15, 8, 16, pGUI->staminaBarImg, ruVector3( 255, 255, 255 ), mStaminaAlpha );
        mGUIHealthBarSegment[i] = ruCreateGUIRect( 44 + i * ( 8 + 2 ), ruGetResolutionHeight() - 4 * 26, 8, 16, pGUI->lifeBarImg, ruVector3( 255, 255, 255 ), mHealthAlpha );
    }
}

void Player::DrawStatusBar() {
    if( mMoved ) {
        mStaminaAlpha.SetTarget( 255 );
        mHealthAlpha.SetTarget( 255 );
    } else {
        mStaminaAlpha.SetTarget( 50 );
        mHealthAlpha.SetTarget( 50 );
    }

    mStaminaAlpha.ChaseTarget( 8.0f * g_dt );
    mHealthAlpha.ChaseTarget( 8.0f * g_dt );

    ruSetGUINodeAlpha( mGUIBackground, mStaminaAlpha );
    int segCount = mStamina / 5;
    for( int i = 0; i < mGUISegmentCount; i++ ) {
        if( i < segCount ) {
            ruSetGUINodeVisible( mGUIStaminaBarSegment[i], true );
            ruSetGUINodeAlpha( mGUIStaminaBarSegment[i], mStaminaAlpha );
        } else {
            ruSetGUINodeVisible( mGUIStaminaBarSegment[i], false );
        }
    }
    segCount = mHealth / 5;
    for( int i = 0; i < mGUISegmentCount; i++ ) {
        if( i < segCount ) {
            ruSetGUINodeVisible( mGUIHealthBarSegment[i], true );
            ruSetGUINodeAlpha( mGUIHealthBarSegment[i], mHealthAlpha );
        } else {
            ruSetGUINodeVisible( mGUIHealthBarSegment[i], false );
        }
    }
}

bool Player::IsCanJump( ) {    
    return ruCastRay( ruGetNodePosition( mBody ) + ruVector3( 0, 0.1, 0 ), ruGetNodePosition( mBody ) - ruVector3( 0, mBodyHeight * 2, 0 ), 0 ).IsValid();
}

bool Player::UseStamina( float required ) {
    if( mStamina - required < 0 ) {
        return false;
    }

    mStamina -= required;

    return true;
}

void Player::Damage( float dmg ) {
    Actor::Damage( dmg );
    if( mHealth <= 0.0f ) {
        if( !mDead ) {
            ruSetAngularFactor( mBody, ruVector3( 1.0f, 1.0f, 1.0f ));
            ruSetNodeFriction( mBody, 1.0f );
            ruSetNodeAnisotropicFriction( mBody, ruVector3( 1.0f, 1.0f, 1.0f ));
            ruMoveNode( mBody, ruVector3( 1.0f, 1.0f, 1.0f ));
        }
        mDead = true;
        mpFlashlight->SwitchOff();
        mpCamera->FadePercent( 5 );
        mpCamera->SetFadeColor( ruVector3( 70.0f, 0.0f, 0.0f ) );
    }
}

void Player::AddItem( Item * pItem ) {
    if( !pItem ) {
        return;
    }

    if( mInventory.Contains( pItem )) {
        return;
    }

    ruFreeze( pItem->mObject );
    //ruDetachNode( pItem->mObject );
    pItem->MarkAsGrabbed();

    ruSetNodePosition( pItem->mObject, ruVector3( 10000, 10000, 10000 )); // far far away

    mInventory.AddItem( pItem );
}

void Player::UpdateInventory() {
	if( !mpSheetInHands ) {
		if( ruIsKeyHit( mKeyInventory ) ) {
			mInventory.Open( !mInventory.IsOpened() );
		}

		mInventory.Update();
	}
}

void Player::SetObjective( string text ) {
    string objectiveText = mLocalization.GetString( "currentObjective" );
    objectiveText += text;

    mGoal.SetText( objectiveText );

    mObjectiveDone = false;
}

void Player::CompleteObjective() {
    SetObjective( mLocalization.GetString( "objectiveUnknown" ));

    mObjectiveDone = true;
}

void Player::UpdateMouseLook() {
    if( mpCurrentWay ) {
        mSmoothCamera = false;
    } else {
        mSmoothCamera = true;
    }

    if( !mInventory.IsOpened() ) {
        float mouseSpeed =  mouseSens / 2.0f;
        if( mpCurrentWay ) {
            if( !mpCurrentWay->IsFreeLook() ) {
                mouseSpeed = 0.0f;
            }
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
        ruVector3 rayBegin = ruGetNodePosition( mBody ) + ruVector3( mBodyWidth / 2, 0, 0 );
        ruVector3 rayEnd = rayBegin + ruGetNodeRightVector( mBody ) * 10.0f;
        ruVector3 hitPoint;
        ruNodeHandle leftIntersection = ruCastRay( rayBegin, rayEnd, &hitPoint );
        bool canLookLeft = true;
        if( leftIntersection.IsValid() ) {
            float dist2 = ( hitPoint - ruGetNodePosition( mBody )).Length2();
            if( dist2 < 0.4 ) {
                canLookLeft = false;
            }
        }
        if( canLookLeft ) {
            mHeadAngle.SetTarget( mHeadAngle.GetMin() );
        }
    }

    if( ruIsKeyDown( mKeyLookRight )) {
        ruVector3 rayBegin = ruGetNodePosition( mBody ) - ruVector3( mBodyWidth / 2.0f, 0.0f, 0.0f );
        ruVector3 rayEnd = rayBegin - ruGetNodeRightVector( mBody ) * 10.0f;
        ruVector3 hitPoint;
        ruNodeHandle rightIntersection = ruCastRay( rayBegin, rayEnd, &hitPoint );
        bool canLookRight = true;
        if( rightIntersection.IsValid() ) {
            float dist2 = ( hitPoint - ruGetNodePosition( mBody )).Length2();
            if( dist2 < 0.4f ) {
                canLookRight = false;
            }
        }
        if( canLookRight ) {
            mHeadAngle.SetTarget( mHeadAngle.GetMax() );
        }
    }
    mHeadAngle.ChaseTarget( 17.0f * g_dt );
    ruSetNodeRotation( mHead, ruQuaternion( ruVector3( 0.0f, 0.0f, 1.0f ), mHeadAngle ));
}

void Player::UpdateJumping() {
    // do ray test, to determine collision with objects above camera
    ruNodeHandle headBumpObject = ruCastRay( ruGetNodePosition( mBody ) + ruVector3( 0.0f, mBodyHeight * 0.98f, 0.0f ), ruGetNodePosition( mBody ) + ruVector3( 0, 1.02 * mBodyHeight, 0 ), nullptr );

    if( ruIsKeyHit( mKeyJump ) ) {
        if( IsCanJump() ) {
            mJumpTo = ruVector3( 0.0f, 350.0f, 0.0f );
            mLanded = false;
        }
    }

    mGravity = mGravity.Lerp( mJumpTo, 40.0f * g_dt );

    if( mGravity.y >= mJumpTo.y ) {
        mLanded = true;
    }

    if( mLanded || headBumpObject.IsValid() ) {
        mJumpTo = ruVector3( 0, -450.0f, 0.0f );
        if( IsCanJump() ) {
            mJumpTo = ruVector3( 0.0f, 0.0f, 0.0f );
        }
    };
}

void Player::UpdateMoving() {
    for( auto pWay : Way::msWayList ) {
        if( !pWay->IsPlayerInside() )  {
            if( pWay->IsEnterPicked() ) {
                SetActionText( StringBuilder() <<  GetKeyName( mKeyUse ) << mLocalization.GetString( "crawlIn" ));
                if( IsUseButtonHit() ) {
                    pWay->Enter();
                }
            }
        }
    }

    for( auto pDoor : Door::msDoorList ) {
        pDoor->DoInteraction();
        if( pDoor->IsPickedByPlayer() ) {
			if( !pDoor->IsLocked() ) {
				SetActionText( StringBuilder() << GetKeyName( mKeyUse ) << mLocalization.GetString( "openClose" ));
				if( IsUseButtonHit() ) {
					pDoor->SwitchState();
				}
			}
        }
    }

    if( mpCurrentWay ) {
        mStealthMode = false;
        mpCurrentWay->DoEntering();
        if( mpCurrentWay->IsPlayerInside() ) {
            mMoved = false;
            if( ruIsKeyDown( mKeyMoveForward )) {
                mpCurrentWay->SetDirection( Way::Direction::Forward );
                mMoved = true;
            }
            if( ruIsKeyDown( mKeyMoveBackward )) {
                mpCurrentWay->SetDirection( Way::Direction::Backward );
                mMoved = true;
            }
            if( mMoved ) {
                mpCurrentWay->DoPlayerCrawling();
                if( !mpCurrentWay->IsPlayerInside() ) {
                    mpCurrentWay = nullptr;
                }
            } else {
                StopInstant();
                mMoved = false;
            }
        }
    } else {
        ruVector3 look = ruGetNodeLookVector( mBody );
        ruVector3 right = ruGetNodeRightVector( mBody );

        mSpeedTo = ruVector3( 0, 0, 0 );

		bool moveBack = false;

        if( ruIsKeyDown( mKeyMoveForward )) {
            mSpeedTo = mSpeedTo + look;
        }
        if( ruIsKeyDown( mKeyMoveBackward )) {
            mSpeedTo = mSpeedTo - look;
			moveBack = true;
        }
        if( ruIsKeyDown( mKeyStrafeLeft )) {
            mSpeedTo = mSpeedTo + right;
        }
        if( ruIsKeyDown( mKeyStrafeRight )) {
            mSpeedTo = mSpeedTo - right;
        }

        mMoved = mSpeedTo.Length2() > 0;

        if( mMoved ) {
            mSpeedTo = mSpeedTo.Normalize();
        }

        UpdateJumping();

        mRunCameraShakeCoeff = 1.0f;
        mFov.SetTarget( mFov.GetMin() );

        mRunning = false;
        if( ruIsKeyDown( mKeyRun ) && mMoved && !mNodeInHands.IsValid()) {
            if( mStamina > 0 ) {
                mSpeedTo = mSpeedTo * mRunSpeedMult;
                mStamina -= 8.0f * g_dt ;
                mFov.SetTarget( mFov.GetMax() );
                mRunCameraShakeCoeff = 1.425f;
                mRunning = true;
            }
        } else {
            if( mStamina < mMaxStamina ) {
                mStamina += 16.0f * g_dt;
            }
        }

		if( moveBack ) {
			mSpeedTo = mSpeedTo * 0.4f;
		}

		if( ruIsKeyHit( mKeyStealth )) {
			Crouch( !IsCrouch() );
			mStealthMode = IsCrouch();
		}

		UpdateCrouch();

        mSpeedTo = mSpeedTo * ( mStealthMode ? 0.4f : 1.0f ) ;

        mFov.ChaseTarget( 4.0f * g_dt );
        ruSetCameraFOV( mpCamera->mNode, mFov );

        mSpeed = mSpeed.Lerp( mSpeedTo + mGravity, 10.0f * g_dt );
        Step( mSpeed * ruVector3( 100, 1, 100 ), g_dt );
    }

    UpdateCameraShake();
}

void Player::ComputeStealth() {
    bool inLight = false;
 
    for( int i = 0; i < ruGetWorldPointLightCount(); i++ ) {
        if( ruIsLightSeePoint( ruGetWorldPointLight( i ), ruGetNodePosition( mBody ) )) {
            inLight = true;
            break;
        }
    }

    if( !inLight ) {
        for( int i = 0; i < ruGetWorldSpotLightCount(); i++ ) {
            if( ruIsLightSeePoint( ruGetWorldSpotLight( i ), ruGetNodePosition( mBody ) )) {
                inLight = true;
                break;
            }
        }
    }

    if( mpFlashlight->IsOn() ) {
        inLight = true;
    }

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
        if( mStealthMode ) {
            ruSetSoundVolume( snd, 0.15f );
        } else {
            ruSetSoundVolume( snd, 0.4f );
        }
    }

    int alpha = ( 255 * ( ( mStealthFactor > 1.05f ) ? 1.0f : ( mStealthFactor + 0.05f ) ) );
    if (alpha > 255 ) {
        alpha = 255;
    }
    ruVector3 color = ( mStealthFactor < 1.05f ) ? ruVector3( 255, 255, 255 ) : ruVector3( 255, 0, 0 );
	ruSetGUINodeAlpha( mGUIStealthSign, alpha );
	ruSetGUINodeColor( mGUIStealthSign, color );
}


void Player::Update( ) {
    ruSetGUINodeVisible( mGUIActionText, false );    
    mpCamera->Update();
    if( !pMainMenu->IsVisible() ) {
		mGoal.AnimateAndRender();
		DrawStatusBar();
		if( !mDead ) {
			ruSetGUINodeVisible( mGUIStealthSign, mStealthMode );
			UpdateFright();
			UpdateFlashLight();
			mTip.AnimateAndDraw();
			UpdateMouseLook();
			UpdateMoving();
			ComputeStealth();
			UpdatePicking();
			UpdateItemsHandling();
			ManageEnvironmentDamaging();
			UpdateInventory();
			DrawSheetInHands();
			UpdateCursor();
		}
	}
}

void Player::LoadGUIElements() {
    mItemPickupSound = ruLoadSound2D( "data/sounds/menuhit.ogg" );
    mStatusBar = ruGetTexture( "data/gui/statusbar.png" );

	mGUICursorPickUp = ruCreateGUIRect( (ruGetResolutionWidth() - 32) / 2, (ruGetResolutionHeight() - 32) / 2, 32, 32, ruGetTexture( "data/gui/up.png" ) );
	mGUICursorPut = ruCreateGUIRect( (ruGetResolutionWidth() - 32) / 2, (ruGetResolutionHeight() - 32) / 2, 32, 32, ruGetTexture( "data/gui/down.png" ) );
	mGUICrosshair = ruCreateGUIRect( (ruGetResolutionWidth() - 32) / 2, (ruGetResolutionHeight() - 32) / 2, 32, 32, ruGetTexture( "data/gui/crosshair.png" ) );
	mGUIStealthSign = ruCreateGUIRect( ruGetResolutionWidth() / 2 - 32, 200, 64, 32, ruGetTexture( "data/textures/effects/eye.png" ));
}

void Player::CreateCamera() {
    mHeadHeight = 2.1;

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

void Player::SetRockFootsteps() {
    for( auto s : mFootstepList ) {
        ruFreeSound( s );
    }

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

void Player::SetDirtFootsteps() {
    for( auto s : mFootstepList ) {
        ruFreeSound( s );
    }

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

void Player::LoadSounds() {
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

void Player::DoFright() {
    mBreathVolume.SetTarget( 0.1f );
    mBreathVolume.Set( 0.25f );
    mHeartBeatVolume.SetTarget( 0.15f );
    mHeartBeatVolume.Set( 0.45f );
    mHeartBeatPitch.Set( 2.0f );
    mHeartBeatPitch.SetTarget( 1.0f );
    mBreathPitch.Set( 1.5f );
    mBreathPitch.SetTarget( 1.0f );
}

void Player::UpdateFright() {
    mBreathVolume.ChaseTarget( 0.075f );
    mHeartBeatVolume.ChaseTarget( 0.075f );
    mHeartBeatPitch.ChaseTarget( 0.0025f );
    mBreathPitch.ChaseTarget( 0.0025f );
	/*
    ruSetSoundVolume( breathSound, breathVolume );
    ruSetSoundVolume( heartBeatSound, heartBeatVolume );

    ruPlaySound( heartBeatSound, true );
    ruPlaySound( breathSound, true );

	ruSetSoundsPitch( breathSound, breathPitch );
	ruSetSoundsPitch( heartBeatSound, heartBeatPitch );*/
}

void Player::UpdateCameraShake() {
    static int stepPlayed = 0;

    if( mMoved ) {
        mCameraBobCoeff += 7.5 * mRunCameraShakeCoeff * g_dt;

        float xOffset = sinf( mCameraBobCoeff ) * ( mRunCameraShakeCoeff * mRunCameraShakeCoeff ) * 0.045f;
        float yOffset = abs( xOffset );

        if( yOffset < 0.02 && !ruIsSoundPlaying( mFootstepList[ stepPlayed ] ) ) {
            stepPlayed = rand() % mFootstepList.size();

            ruPlaySound( mFootstepList[ stepPlayed ]);
        }

        mCameraShakeOffset = ruVector3( xOffset, yOffset + mHeadHeight, 0 );
    } else {
        mCameraBobCoeff = 0;
    }
    mCameraOffset = mCameraOffset.Lerp( mCameraShakeOffset, 0.25f );
    ruSetNodePosition( mpCamera->mNode, mCameraOffset );
}

void Player::DrawSheetInHands() {
    if( mpSheetInHands ) {
		SetActionText( StringBuilder() << mpSheetInHands->GetDescription() << mLocalization.GetString( "sheetOpen" ));
        mpSheetInHands->SetVisible( true );
        mpSheetInHands->Draw();
        if( ruIsMouseHit( MB_Right ) ||  ( ruGetNodePosition( mpSheetInHands->mObject) - ruGetNodePosition( mBody )).Length2() > 2 ) {
            CloseCurrentSheet();
        }
    }
}

void Player::UpdateCursor() {
	if( mInventory.IsOpened() )	{
		ruSetGUINodeVisible( mGUICursorPut, false );
		ruSetGUINodeVisible( mGUICrosshair, false );
		ruSetGUINodeVisible( mGUICursorPickUp, false );	
	} else {
		if( mNearestPickedNode.IsValid() ) {
			if( IsObjectHasNormalMass( mNearestPickedNode )) {
				ruSetGUINodeVisible( mGUICursorPickUp, true );		
				ruSetGUINodeVisible( mGUICrosshair, false );
			} else {			
				ruSetGUINodeVisible( mGUICursorPut, false );
				ruSetGUINodeVisible( mGUICrosshair, true );
			}		
		} else {
			ruSetGUINodeVisible( mGUICursorPickUp, false );
			if( mNodeInHands.IsValid() ) {
				ruSetGUINodeVisible( mGUICursorPut, true );
				ruSetGUINodeVisible( mGUICrosshair, false );
			} else {
				ruSetGUINodeVisible( mGUICursorPut, false );
				ruSetGUINodeVisible( mGUICrosshair, true );
			}
		}
	}
}

void Player::UpdateItemsHandling() {
	if( !mInventory.IsOpened() ) {
		if( mNearestPickedNode.IsValid() ) {
			if( IsUseButtonHit() ) {
				Item * pItem = Item::GetItemPointerByNode( mNearestPickedNode );

				if( pItem ) {
					AddItem( pItem );

					ruPlaySound( mItemPickupSound );
				}

				Sheet * pSheet = Sheet::GetSheetPointerByNode( mNearestPickedNode );

				if( mpSheetInHands ) {
					CloseCurrentSheet();
				} else {
					if( pSheet ) {
						mpSheetInHands = pSheet;
						ruHideNode( mpSheetInHands->mObject );
						ruPlaySound( Sheet::msPaperFlipSound );
					}
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
                if( UseStamina( ruGetNodeMass( mNodeInHands )  )) {
                    ruMoveNode( mNodeInHands, ( ppPos - ruGetNodePosition( mpCamera->mNode )).Normalize() * 6 );
                }

                mObjectThrown = true;
                mNodeInHands.Invalidate();
            }
        } else {
            ruSetNodeAngularVelocity( mNodeInHands, ruVector3( 1, 1, 1 ));

            mNodeInHands.Invalidate();
        }
    }

    if( !ruIsMouseDown( MB_Left )) {
        mObjectThrown = false;
    }
}

void Player::RepairInventory() {
	mInventory.Repair();
}

void Player::UpdatePicking() {
    ruVector3 pickPosition;

    mPickedNode = ruRayPick( ruGetResolutionWidth() / 2, ruGetResolutionHeight() / 2, &pickPosition );

    mNearestPickedNode.Invalidate();

    if( mPickedNode.IsValid() && !mNodeInHands.IsValid()  ) {
        mNodeInHands.Invalidate();

        ruVector3 ppPos = ruGetNodePosition( mPickPoint );
        ruVector3 dir = ppPos - pickPosition;

        Item * pItem = Item::GetItemPointerByNode( mPickedNode );
        Sheet * pSheet = Sheet::GetSheetPointerByNode( mPickedNode );

        if( dir.Length2() < 1.5f ) {
            mNearestPickedNode = mPickedNode;
			string pickedObjectDesc;
            if( pItem ) {
                pickedObjectDesc = StringBuilder() << pItem->GetName() << "- [" << GetKeyName( mKeyUse).c_str() << "] " << mLocalization.GetString( "itemPick" );
                SetActionText( pickedObjectDesc );
            } else if( pSheet ) {
                pickedObjectDesc = StringBuilder() << pSheet->GetDescription() << "- [" << GetKeyName( mKeyUse ) << "] " << mLocalization.GetString( "sheetPick" );
                SetActionText( pickedObjectDesc );
            } else {
                if( IsObjectHasNormalMass( mPickedNode ) && !ruIsNodeFrozen( mPickedNode )) {
                    SetActionText( mLocalization.GetString( "objectPick" ) );
                }
            }

            if( ruIsMouseDown( MB_Left ) ) {
                if( IsObjectHasNormalMass( mPickedNode )) {
                    if( !ruIsNodeFrozen( mPickedNode ) && !mObjectThrown ) {
                        mNodeInHands = mPickedNode;
                    }
                }
            }
        }
    }
}


void Player::CreateFlashLight() {
    mpFlashlight = new Flashlight();
    mpFlashlight->Attach( mpCamera->mNode );
    mpFlashLightItem = mpFlashlight->CreateAppropriateItem();
    mInventory.AddItem( mpFlashLightItem );
}

void Player::UpdateFlashLight() {
    mpFlashlight->Update();

    mpFlashLightItem->SetContent( mpFlashlight->GetCharge() );

    if( ruIsKeyHit( mKeyFlashLight ) && !mNodeInHands.IsValid() ) {
        mpFlashlight->Switch();
    }
}

void Player::FreeHands() {
    mNodeInHands.Invalidate();
}

void Player::ChargeFlashLight() {
    mpFlashlight->Fuel();
}

Player::~Player() {
    delete mpFlashlight;
    delete mpCamera;
    delete mpFlashLightItem;
}

void Player::SetMetalFootsteps() {
    for( auto s : mFootstepList ) {
        ruFreeSound( s );
    }

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

void Player::SetFootsteps( FootstepsType ft ) {
    if( ft == FootstepsType::Dirt ) {
        SetDirtFootsteps();
    }
    if( ft == FootstepsType::Rock ) {
        SetRockFootsteps();
    }
    if( ft == FootstepsType::Metal ) {
        SetMetalFootsteps();
    }
}

bool Player::IsUseButtonHit() {
    return ruIsKeyHit( mKeyUse );
}

bool Player::IsObjectHasNormalMass( ruNodeHandle node ) {
    return ruGetNodeMass( node ) > 0 && ruGetNodeMass( node ) < 40;
}

void Player::DeserializeWith( TextFileStream & in ) {
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

    mpCurrentWay = Way::GetByObject( ruFindByName( in.ReadString() ));
    if( mpCurrentWay ) {
        ruFreeze( pPlayer->mBody );
    }

    in.ReadBoolean( mLanded );
    in.ReadFloat( mStamina );
    in.ReadFloat( mHealth );
    in.ReadFloat( mMaxHealth );
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

    mStaminaAlpha.Deserialize( in );
    mHealthAlpha.Deserialize( in );

    in.ReadBoolean( mMoved );
    in.ReadBoolean( mObjectiveDone );

    mBreathVolume.Deserialize( in );
    mHeartBeatVolume.Deserialize( in );
    mHeartBeatPitch.Deserialize( in );
    mBreathPitch.Deserialize( in );

    mpSheetInHands = Sheet::GetSheetPointerByNode( ruFindByName( in.ReadString() ));

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

    mpCamera->FadePercent( 100 );
    mpCamera->SetFadeColor( ruVector3( 255, 255, 255 ) );
    ruSetNodeFriction( mBody, 0 );
}

void Player::SerializeWith( TextFileStream & out ) {
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
    out.WriteFloat( mHealth );
    out.WriteFloat( mMaxHealth );
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
}

void Player::CloseCurrentSheet() {
    mpSheetInHands->SetVisible( false );
    ruShowNode( mpSheetInHands->mObject );
    mpSheetInHands = nullptr;
    ruPlaySound( Sheet::msPaperFlipSound );
}

void Player::SetTip( const string & text ) {
    mTip.SetNewText( text );
}

Parser * Player::GetLocalization() {
    return &mLocalization;
}

Flashlight * Player::GetFlashLight() {
    return mpFlashlight;
}

Inventory * Player::GetInventory() {
    return &mInventory;
}

void Player::SetActionText( const string & text ) {
    ruSetGUINodeText( mGUIActionText, text );
    ruSetGUINodeVisible( mGUIActionText, true );
}

void Player::SetHUDVisible( bool state ) {
    for( int i = 0; i < mGUISegmentCount; i++ ) {
        ruSetGUINodeVisible( mGUIHealthBarSegment[i], state );
        ruSetGUINodeVisible( mGUIStaminaBarSegment[i], state );
    }
    ruSetGUINodeVisible( mGUIBackground, state );
    ruSetGUINodeVisible( mGUIActionText, state );
	ruSetGUINodeVisible( mGUIStealthSign, state );	
    mGoal.SetVisible( state );
	if( !state ) {
		mTip.SetVisible( state );
		if( mpSheetInHands ) {
			CloseCurrentSheet();
		}
		mInventory.SetVisible( state );
		ruSetGUINodeVisible( mGUICursorPickUp, state );		
		ruSetGUINodeVisible( mGUICrosshair, state );		
		ruSetGUINodeVisible( mGUICursorPut, state );
	}
}

void Player::ManageEnvironmentDamaging()
{
	/*
	for( int i = 0; i < ruGetContactCount( mBody ); i++ ) {
		ruContact contact = ruGetContact( mBody, i );
		if( contact.body.IsValid()) {
			if( !(contact.body == mNodeInHands)) {
				if( ruGetNodeLinearVelocity( contact.body ).Length2() >= 2.0f ) {
					Damage( contact.impulse / 5 );
				}
			}
		}
	}*/
}
