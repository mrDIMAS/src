#include "Precompiled.h"

#include "Player.h"
#include "Menu.h"
#include "Door.h"
#include "utils.h"
#include "Enemy.h"
#include "SaveWriter.h"
#include "BareHands.h"

Player * pPlayer = 0;

extern double gFixedTick;

Player::Player() : Actor( 0.7f, 0.2f ), mStepLength( 0.0f ), mCameraTrembleTime( 0.0f ), mFlashlightLocked( false ), mCurrentUsableObject( nullptr ), 
	mLandedSoundEmitted( true ) {
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

	mLastHealth = mHealth;

    // GUI vars
    mStaminaAlpha = SmoothFloat( 255.0, 0.0f, 255.0f );
    mHealthAlpha = SmoothFloat( 255.0, 0.0f, 255.0f );

    mpCurrentWay = nullptr;

    LoadGUIElements();
    CreateCamera();
    LoadSounds();
    CompleteObjective();

    mBody.SetName( "Player" );

    mGUIActionText = ruCreateGUIText( "Action text", ruEngine::GetResolutionWidth() / 2 - 256, ruEngine::GetResolutionHeight() - 200, 512, 128, pGUI->mFont, ruVector3( 255, 0, 0 ), 1 );

    float scale = 2;
    int w = 512.0f / scale;
    int h = 256.0f / scale;
    mGUIBackground = ruCreateGUIRect( 0, ruEngine::GetResolutionHeight() - h, w, h, mStatusBar, ruVector3( 255, 255, 255 ), mStaminaAlpha );
    for( int i = 0; i < mGUISegmentCount; i++ ) {
        mGUIStaminaBarSegment[i] = ruCreateGUIRect( 44 + i * ( 8 + 2 ), ruEngine::GetResolutionHeight() - 3 * 15, 8, 16, pGUI->staminaBarImg, ruVector3( 255, 255, 255 ), mStaminaAlpha );
        mGUIHealthBarSegment[i] = ruCreateGUIRect( 44 + i * ( 8 + 2 ), ruEngine::GetResolutionHeight() - 4 * 26, 8, 16, pGUI->lifeBarImg, ruVector3( 255, 255, 255 ), mHealthAlpha );
    }

	mGUIYouDiedFont = ruCreateGUIFont( 40, "data/fonts/font1.otf" );
	mGUIYouDied = ruCreateGUIText( mLocalization.GetString( "youDied" ), (ruEngine::GetResolutionWidth() - 300) / 2, ruEngine::GetResolutionHeight() / 2, 300, 50, mGUIYouDiedFont, ruVector3( 255, 0, 0 ), 1, 255 );
	ruSetGUINodeVisible( mGUIYouDied, false );

	mInAir = false;

	// hack
	pMainMenu->SyncPlayerControls();

	mAutoSaveTimer = ruCreateTimer();

	mPainSound.push_back( ruSound::Load2D( "data/sounds/player/grunt1.ogg" ));
	mPainSound.push_back( ruSound::Load2D( "data/sounds/player/grunt2.ogg" ));
	mPainSound.push_back( ruSound::Load2D( "data/sounds/player/grunt3.ogg" ));

	for( auto & ps : mPainSound ) {
		ps.SetVolume( 0.7 ); 
	}


	mSoundMaterialList.push_back( new SoundMaterial( "data/materials/stone.smat", mpCamera->mNode ));
	mSoundMaterialList.push_back( new SoundMaterial( "data/materials/metal.smat", mpCamera->mNode ));
	mSoundMaterialList.push_back( new SoundMaterial( "data/materials/wood.smat", mpCamera->mNode ));
	mSoundMaterialList.push_back( new SoundMaterial( "data/materials/gravel.smat", mpCamera->mNode ));
	mSoundMaterialList.push_back( new SoundMaterial( "data/materials/muddyrock.smat", mpCamera->mNode ));
	mSoundMaterialList.push_back( new SoundMaterial( "data/materials/rock.smat", mpCamera->mNode ));
	mSoundMaterialList.push_back( new SoundMaterial( "data/materials/grass.smat", mpCamera->mNode ));
	mSoundMaterialList.push_back( new SoundMaterial( "data/materials/soil.smat", mpCamera->mNode ));
	mSoundMaterialList.push_back( new SoundMaterial( "data/materials/chain.smat", mpCamera->mNode ));

	AddUsableObject( new BareHands );
	AddUsableObject( new Flashlight );
}

Player::~Player() {
	for( auto & ps : mPainSound ) {
		ps.Free();
	}

	for( auto uo : mUsableObjectList ) {
		delete uo;
	}

	mUsableObjectList.clear();

	delete mpCamera;

	ruFreeGUINode( mGUIActionText );
	for( int i = 0; i < mGUISegmentCount; i++ ) {
		ruFreeGUINode( mGUIHealthBarSegment[i] );
		ruFreeGUINode( mGUIStaminaBarSegment[i] );
	}
	ruFreeGUINode( mGUIBackground );
	ruFreeGUINode( mGUIStealthSign );

	mLighterCloseSound.Free();
	mLighterOpenSound.Free();
	mItemPickupSound.Free();
	mHeartBeatSound.Free();
	mBreathSound.Free();
	if( mDeadSound.IsValid() ) {
		mDeadSound.Free();
	}

	for( auto sndMat : mSoundMaterialList ) {
		delete sndMat;
	}

	ruFreeGUINode( mGUICursorPickUp );
	ruFreeGUINode( mGUICursorPut );
	ruFreeGUINode( mGUICrosshair );
	ruFreeGUINode( mGUIYouDied );

	mGUIYouDiedFont.Free();
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
	ruSceneNode node = ruCastRay( mBody.GetPosition() + ruVector3( 0, 0.1, 0 ), mBody.GetPosition() - ruVector3( 0, mBodyHeight * 2, 0 ), 0 );
	if( node.IsValid() ) {
		if( node == mNodeInHands ) {
			mNodeInHands.Invalidate();
			return false;
		} else {
			return true;
		}
	}
    return false;
}

bool Player::UseStamina( float required ) {
    if( mStamina - required < 0 ) {
        return false;
    }

    mStamina -= required;

    return true;
}

void Player::Damage( float dmg, bool headJitter ) {
    Actor::Damage( dmg );
	mDamageBackgroundAlpha = 60;
	if( headJitter ) {
		mPitch.SetTarget( mPitch.GetTarget() + frandom( 20, 40 ) );
		mYaw.SetTarget( mYaw.GetTarget() + frandom( -40, 40 ) );
	}
	if( mLastHealth - mHealth > 5 ) {
		mPainSound[ rand() % mPainSound.size() ].Play();
		mLastHealth = mHealth;
	}
    if( mHealth <= 0.0f ) {
        if( !mDead ) {
            mBody.SetAngularFactor( ruVector3( 1.0f, 1.0f, 1.0f ));
            mBody.SetFriction( 1.0f );
            mBody.SetAnisotropicFriction( ruVector3( 1.0f, 1.0f, 1.0f ));
            mBody.Move( ruVector3( 1.0f, 1.0f, 1.0f ));
        }
		mDeadSound = ruSound::Load2D( "data/sounds/dead.ogg" );
		mDeadSound.Play();
        mDead = true;
        mpCamera->FadePercent( 0 );
        mpCamera->SetFadeColor( ruVector3( 70.0f, 0.0f, 0.0f ) );
    }	
}

void Player::AddItem( Item::Type type ) {
	mInventory.AddItem( type );	
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


    mpCamera->mNode.SetRotation( ruQuaternion( ruVector3( 1, 0, 0 ), mPitch ) );
    mBody.SetRotation( ruQuaternion( ruVector3( 0, 1, 0 ), mYaw ) );

    mHeadAngle.SetTarget( 0.0f );
    if( ruIsKeyDown( mKeyLookLeft )) {
        ruVector3 rayBegin = mBody.GetPosition() + ruVector3( mBodyWidth / 2, 0, 0 );
        ruVector3 rayEnd = rayBegin + mBody.GetRightVector() * 10.0f;
        ruVector3 hitPoint;
        ruSceneNode leftIntersection = ruCastRay( rayBegin, rayEnd, &hitPoint );
        bool canLookLeft = true;
        if( leftIntersection.IsValid() ) {
            float dist2 = ( hitPoint -  mBody.GetPosition() ).Length2();
            if( dist2 < 0.4 ) {
                canLookLeft = false;
            }
        }
        if( canLookLeft ) {
            mHeadAngle.SetTarget( mHeadAngle.GetMin() );
        }
    }

    if( ruIsKeyDown( mKeyLookRight )) {
        ruVector3 rayBegin = mBody.GetPosition() - ruVector3( mBodyWidth / 2.0f, 0.0f, 0.0f );
        ruVector3 rayEnd = rayBegin - mBody.GetRightVector() * 10.0f;
        ruVector3 hitPoint;
        ruSceneNode rightIntersection = ruCastRay( rayBegin, rayEnd, &hitPoint );
        bool canLookRight = true;
        if( rightIntersection.IsValid() ) {
            float dist2 = ( hitPoint - mBody.GetPosition() ).Length2();
            if( dist2 < 0.4f ) {
                canLookRight = false;
            }
        }
        if( canLookRight ) {
            mHeadAngle.SetTarget( mHeadAngle.GetMax() );
        }
    }
    mHeadAngle.ChaseTarget( 17.0f * g_dt );
    mHead.SetRotation( ruQuaternion( ruVector3( 0.0f, 0.0f, 1.0f ), mHeadAngle ));
}

void Player::UpdateJumping() {
    // do ray test, to determine collision with objects above camera
    ruSceneNode headBumpObject = ruCastRay( mBody.GetPosition() + ruVector3( 0.0f, mBodyHeight * 0.98f, 0.0f ), mBody.GetPosition() + ruVector3( 0, 1.02 * mBodyHeight, 0 ), nullptr );

    if( ruIsKeyHit( mKeyJump ) ) {
        if( IsCanJump() ) {
            mJumpTo = ruVector3( 0.0f, 350.0f, 0.0f );
            mLanded = false;
			mLandedSoundEmitted = false;
        }
    }

    mGravity = mGravity.Lerp( mJumpTo, 39.0f * g_dt );

    if( mGravity.y >= mJumpTo.y ) {
        mLanded = true;
    }

	if( !mLandedSoundEmitted ) {
		if( IsCanJump() && mLanded ) {
			// two foot
			EmitStepSound();
			EmitStepSound();
			mLandedSoundEmitted = true;
		}
	}



    if( mLanded || headBumpObject.IsValid() ) {
        mJumpTo = ruVector3( 0, -400.0f, 0.0f );
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
			} else {
				SetActionText( mLocalization.GetString( "doorLocked" ));
			}
        }
    }

    if( mpCurrentWay ) {
        mStealthMode = false;
        mpCurrentWay->DoEntering();
		mAirPosition = mBody.GetPosition();
		Crouch( false );
		mRunning = false;
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
            }
        }
		if( ruIsKeyHit( mKeyJump )) {
			mpCurrentWay->LeaveInstantly();
			mpCurrentWay = nullptr;
			Unfreeze();
			mBody.SetVelocity( ruVector3( -mBody.GetLookVector() ));
		}
    } else {
        ruVector3 look = mBody.GetLookVector();
        ruVector3 right = mBody.GetRightVector();

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
        if( !IsCrouch() && ruIsKeyDown( mKeyRun ) && mMoved && !mNodeInHands.IsValid()) {
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
		if( !(ruGetWorldPointLight( i ) == mFakeLight)) {
			if( ruIsLightSeePoint( ruGetWorldPointLight( i ), mBody.GetPosition() )) {
				inLight = true;
				break;
			}
		}
    }

    if( !inLight ) {
        for( int i = 0; i < ruGetWorldSpotLightCount(); i++ ) {
            if( ruIsLightSeePoint( ruGetWorldSpotLight( i ), mBody.GetPosition() )) {
                inLight = true;
                break;
            }
        }
    }

	if( pPlayer->GetFlashLight() ) {
		if( pPlayer->GetFlashLight()->IsOn() ) {
			inLight = true;
		}
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
	mDamageBackgroundAlpha--;
	if( mDamageBackgroundAlpha < 0 ) {
		mDamageBackgroundAlpha = 0;
	}

	if( pPlayer->GetFlashLight() ) {
		if( pPlayer->GetFlashLight()->IsOn() ) {
			mFakeLight.Hide();
		} else {
			mFakeLight.Show();
		}
	} else {
		mFakeLight.Show();
	}

	ruSetGUINodeAlpha( mGUIDamageBackground, mDamageBackgroundAlpha );
    if( !pMainMenu->IsVisible() ) {
		mGoal.AnimateAndRender();
		DrawStatusBar();
		if( !mDead ) {		
			ruSetGUINodeVisible( mGUIYouDied, false );
			ruSetGUINodeVisible( mGUIStealthSign, mStealthMode );
			UpdateFright();
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
			UpdateUsableObjects();

			if( !mpCurrentWay ) { // prevent damaging from ladders
				if( !IsCanJump() && !mInAir ) { // in air
					mAirPosition = mBody.GetPosition();
					mInAir = true;
				} else if( IsCanJump() && mInAir ) { // landing 
					ruVector3 curPos = mBody.GetPosition();
					float heightDelta = fabsf( curPos.y - mAirPosition.y );
					if( heightDelta > 2.0f ) {
						Damage( heightDelta * 10 );
					}
					mInAir = false;
				}
			}

			// this must be placed in other place :)
			if( ruGetElapsedTimeInSeconds( mAutoSaveTimer ) >= 30 ) {
				SaveWriter( "autosave.save").SaveWorldState();
				ruRestartTimer( mAutoSaveTimer );
			}
		} else {
			ruSetGUINodeVisible( mGUIYouDied, true );
		}

		if( mDeadSound.IsValid() ) {
			if( !mDeadSound.IsPlaying() ) {
				pMainMenu->Show();
			}
		}

	} else {
		if( mDeadSound.IsValid() ) {
			mDeadSound.Pause();
		}
		ruSetGUINodeVisible( mGUIYouDied, false );
	}
}

void Player::LoadGUIElements() {
    mItemPickupSound = ruSound::Load2D( "data/sounds/menuhit.ogg" );
    mStatusBar = ruGetTexture( "data/gui/statusbar.png" );

	mGUICursorPickUp = ruCreateGUIRect( (ruEngine::GetResolutionWidth() - 32) / 2, (ruEngine::GetResolutionHeight() - 32) / 2, 32, 32, ruGetTexture( "data/gui/up.png" ) );
	mGUICursorPut = ruCreateGUIRect( (ruEngine::GetResolutionWidth() - 32) / 2, (ruEngine::GetResolutionHeight() - 32) / 2, 32, 32, ruGetTexture( "data/gui/down.png" ) );
	mGUICrosshair = ruCreateGUIRect( (ruEngine::GetResolutionWidth() - 32) / 2, (ruEngine::GetResolutionHeight() - 32) / 2, 32, 32, ruGetTexture( "data/gui/crosshair.png" ) );
	mGUIStealthSign = ruCreateGUIRect( ruEngine::GetResolutionWidth() / 2 - 32, 200, 64, 32, ruGetTexture( "data/textures/effects/eye.png" ));

	mDamageBackgroundAlpha = 0;
	mGUIDamageBackground = ruCreateGUIRect( 0, 0, ruEngine::GetResolutionWidth(), ruEngine::GetResolutionHeight(), ruGetTexture("data/textures/effects/damageBackground.tga" ),ruVector3( 200, 0, 0 ), mDamageBackgroundAlpha );
}

void Player::CreateCamera() {
    mHeadHeight = 2.1;

    mHead = ruSceneNode::Create();
    mHead.Attach( mBody );
    mHead.SetPosition( ruVector3( 0, -2.0f, 0.0f ));
    mpCamera = new GameCamera( mFov );
    mpCamera->mNode.Attach( mHead );
    mCameraOffset = ruVector3( 0, mHeadHeight, 0 );
    mCameraShakeOffset = ruVector3( 0, mHeadHeight, 0 );

    // Pick
    mPickPoint = ruSceneNode::Create();
    mPickPoint.Attach( mpCamera->mNode );
    mPickPoint.SetPosition( ruVector3( 0, 0, 0.1 ));

    mItemPoint = ruSceneNode::Create();
    mItemPoint.Attach( mpCamera->mNode );
    mItemPoint.SetPosition( ruVector3( 0, 0, 1.0f ));

	mFakeLight = ruCreateLight();
	mFakeLight.Attach( mpCamera->mNode );
	ruSetLightRange( mFakeLight, 2 );
	ruSetLightColor( mFakeLight, ruVector3( 25, 25, 25 ));
}

void Player::LoadSounds() {
    mLighterCloseSound = ruSound::Load3D( "data/sounds/lighter_close.ogg" );
    mLighterOpenSound = ruSound::Load3D( "data/sounds/lighter_open.ogg" );

    mLighterCloseSound.Attach( mpCamera->mNode );
    mLighterOpenSound.Attach( mpCamera->mNode );

    mHeartBeatSound = ruSound::Load2D( "data/sounds/heart.ogg" );
    mBreathSound = ruSound::Load2D( "data/sounds/breath.ogg" );

    mHeartBeatSound.SetReferenceDistance( 100.0f );
    mBreathSound.SetReferenceDistance( 100.0f );

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
}

void Player::UpdateCameraShake() {
    static int stepPlayed = 0;

	if( mMoved ) {
		const float mBodyWidthDelta = mBodyWidth / 60.0f;
		mStepLength += mRunning ? mBodyWidthDelta * 2.0f : mBodyWidthDelta;


        mCameraBobCoeff += 11.5 * mRunCameraShakeCoeff * g_dt;

		float yOffset = 0.045f * sinf( mCameraBobCoeff ) * ( mRunCameraShakeCoeff * mRunCameraShakeCoeff );
        float xOffset = 0.045f * cosf( mCameraBobCoeff / 2 ) * ( mRunCameraShakeCoeff * mRunCameraShakeCoeff );        

        if( mStepLength > mBodyWidth / 2.0f ) {
			if( mpCurrentWay ) {
				if( mpCurrentWay->GetEnterZone().GetTextureCount() > 0 ) {
					for( auto sMat : mSoundMaterialList ) {
						ruSound snd = sMat->GetRandomSoundAssociatedWith( mpCurrentWay->GetEnterZone().GetTexture( 0 ).GetName() );
						if( snd.IsValid() ) {
							snd.Play( true );
						}
					}
				}
			} else {
				EmitStepSound();
			}
			mStepLength = 0.0f;
        }

        mCameraShakeOffset = ruVector3( xOffset, yOffset + mHeadHeight, 0.0f );
    } else {
        mCameraBobCoeff = 0.0f;
    }

	if( mCameraTrembleTime > 0 ) {
		const float trembleVol = 0.085;
		mCameraTrembleOffset = ruVector3( frandom( -trembleVol, trembleVol ), frandom( -trembleVol, trembleVol ),  frandom( -trembleVol, trembleVol ));
		mCameraTrembleTime -= gFixedTick;
	} else { 
		mCameraTrembleOffset = ruVector3( 0.0f, 0.0f, 0.0f );
	}
    mCameraOffset = mCameraOffset.Lerp( mCameraShakeOffset, 0.25f ) + mCameraTrembleOffset;
    mpCamera->mNode.SetPosition( mCameraOffset );
}

void Player::DrawSheetInHands() {
    if( mpSheetInHands ) {
		SetActionText( StringBuilder() << mpSheetInHands->GetDescription() << mLocalization.GetString( "sheetOpen" ));
        mpSheetInHands->SetVisible( true );
        mpSheetInHands->Draw();
        if( ruIsMouseHit( MB_Right ) ||  ( mpSheetInHands->mObject.GetPosition() - mBody.GetPosition() ).Length2() > 2 ) {
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
				Sheet * pSheet = Sheet::GetSheetPointerByNode( mNearestPickedNode );

				if( mpSheetInHands ) {
					CloseCurrentSheet();
				} else {
					if( pSheet ) {
						mpSheetInHands = pSheet;
						mpSheetInHands->mObject.Hide();
						Sheet::msPaperFlipSound.Play();
					}
				}
			}
		}
	}

    if( mNodeInHands.IsValid() ) {
        ruVector3 ppPos = mItemPoint.GetPosition();
        ruVector3 objectPos = mNodeInHands.GetPosition();
        ruVector3 dir = ppPos - objectPos;
        if( ruIsMouseDown( MB_Left ) ) {
            mNodeInHands.Move( dir * 6 );

            mNodeInHands.SetAngularVelocity( ruVector3( 0, 0, 0 ));

            if( ruIsMouseDown( MB_Right ) ) {
                if( UseStamina( mNodeInHands.GetMass() )) {
                    mNodeInHands.Move(( ppPos - mpCamera->mNode.GetPosition() ).Normalize() * 6 );
                }

                mObjectThrown = true;
                mNodeInHands.Invalidate();
            }
        } else {
            mNodeInHands.SetAngularVelocity( ruVector3( 1, 1, 1 ));

            mNodeInHands.Invalidate();
        }
    }

    if( !ruIsMouseDown( MB_Left )) {
        mObjectThrown = false;
    }
}

void Player::UpdatePicking() {
    ruVector3 pickPosition;

    mPickedNode = ruRayPick( ruEngine::GetResolutionWidth() / 2, ruEngine::GetResolutionHeight() / 2, &pickPosition );

    mNearestPickedNode.Invalidate();

	for( auto pEnemy : Enemy::msEnemyList ) {
		if( mPickedNode == pEnemy->GetBody() ) {
			mPickedNode.Invalidate();
		}
	}

    if( mPickedNode.IsValid() && !mNodeInHands.IsValid()  ) {
        mNodeInHands.Invalidate();

        ruVector3 ppPos = mPickPoint.GetPosition();
        ruVector3 dir = ppPos - pickPosition;

        InteractiveObject * pIO = InteractiveObject::FindByObject( mPickedNode );
        Sheet * pSheet = Sheet::GetSheetPointerByNode( mPickedNode );

        if( dir.Length2() < 1.5f ) {
            mNearestPickedNode = mPickedNode;
			string pickedObjectDesc;
            if( pIO ) {
                pickedObjectDesc = StringBuilder() << pIO->GetPickDescription() << "- [" << GetKeyName( mKeyUse).c_str() << "] " << mLocalization.GetString( "itemPick" );
                SetActionText( pickedObjectDesc );
            } else if( pSheet ) {
                pickedObjectDesc = StringBuilder() << pSheet->GetDescription() << "- [" << GetKeyName( mKeyUse ) << "] " << mLocalization.GetString( "sheetPick" );
                SetActionText( pickedObjectDesc );
            } else {
                if( IsObjectHasNormalMass( mPickedNode ) && !mPickedNode.IsFrozen()) {
                    SetActionText( mLocalization.GetString( "objectPick" ) );
                }
            }

            if( ruIsMouseDown( MB_Left ) ) {
                if( IsObjectHasNormalMass( mPickedNode )) {
                    if( !mPickedNode.IsFrozen() && !mObjectThrown ) {
                        mNodeInHands = mPickedNode;
                    }
                }
            }
        }
    }
}

void Player::FreeHands() {
    mNodeInHands.Invalidate();
}



bool Player::IsUseButtonHit() {
    return ruIsKeyHit( mKeyUse );
}

bool Player::IsObjectHasNormalMass( ruSceneNode node ) {
    return node.GetMass() > 0 && node.GetMass() < 40;
}


void Player::Resurrect() {
	mHealth = 100.0f;
	mDead = false;
	mpCamera->SetFadeColor( ruVector3( 255, 255, 255 ));
	mBody.SetAngularFactor( ruVector3( 0, 0, 0 ));
	mBody.SetRotation( ruQuaternion( 0, 0, 0 ));
}

void Player::Deserialize( SaveFile & in ) {	
	int count = in.ReadInteger( );
	cout << count << endl;
	for( int i = 0; i < count; i++ ) {
		AddUsableObject( UsableObject::Deserialize( in ) );
	}
	int currentUO = in.ReadInteger();
	mCurrentUsableObject = mUsableObjectList[ currentUO ];
	mCurrentUsableObject->Appear();
	
    mBody.SetLocalPosition( in.ReadVector3() );

    in.ReadBoolean( mSmoothCamera );
    in.ReadFloat( mRunCameraShakeCoeff );

    mPitch.Deserialize( in );
    mYaw.Deserialize( in );
    in.ReadVector3( mSpeed );
    in.ReadVector3( mSpeedTo );
    in.ReadVector3( mGravity );
    in.ReadVector3( mJumpTo );

    mpCurrentWay = Way::GetByObject( ruSceneNode::FindByName( in.ReadString() ));
    if( mpCurrentWay ) {
        pPlayer->mBody.Freeze();
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

    mpSheetInHands = Sheet::GetSheetPointerByNode( ruSceneNode::FindByName( in.ReadString() ));

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

    mTip.Deserialize( in );
	
	in.ReadBoolean( mFlashlightLocked );

	in.ReadFloat( mLastHealth );


	mpCamera->FadePercent( 100 );
	mpCamera->SetFadeColor( ruVector3( 255, 255, 255 ) );
	mBody.SetFriction( 0 );

	mInventory.Deserialize( in );
}

void Player::Serialize( SaveFile & out ) {	
	out.WriteInteger( static_cast<int>( mUsableObjectList.size()));
	for( auto uo : mUsableObjectList ) {
		uo->Serialize( out );
	}
	int currentUO_N = -1;
	int i = 0;
	for( auto uo : mUsableObjectList ) {
		if( uo == mCurrentUsableObject ) {
			currentUO_N = i;
		}
		i++;
	}
	out.WriteInteger( currentUO_N );

    mBody.Unfreeze();
    out.WriteVector3( mBody.GetLocalPosition() );
    mBody.SetAngularFactor( ruVector3( 0, 0, 0 ));

    out.WriteBoolean( mSmoothCamera );
    out.WriteFloat( mRunCameraShakeCoeff );
    mPitch.Serialize( out );
    mYaw.Serialize( out );
    out.WriteVector3( mSpeed );
    out.WriteVector3( mSpeedTo );
    out.WriteVector3( mGravity );
    out.WriteVector3( mJumpTo );

    out.WriteString( mpCurrentWay ? mpCurrentWay->GetEnterZone().GetName() : "undefinedWay" );

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

    out.WriteString( mpSheetInHands ? mpSheetInHands->mObject.GetName() : "undefinedSheet" );

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

    mTip.Serialize( out );

	out.WriteBoolean( mFlashlightLocked );

	out.WriteFloat( mLastHealth );

	mInventory.Serialize( out );
}

void Player::CloseCurrentSheet() {
    mpSheetInHands->SetVisible( false );
    mpSheetInHands->mObject.Show();
    mpSheetInHands = nullptr;
    Sheet::msPaperFlipSound.Play();
}

void Player::SetTip( const string & text ) {
    mTip.SetNewText( text );
}

Parser * Player::GetLocalization() {
    return &mLocalization;
}

Flashlight * Player::GetFlashLight() {
	for( auto uo : mUsableObjectList ) {
		if( typeid( *uo ) == typeid( Flashlight )) {
			return dynamic_cast<Flashlight*>( uo );
		}
	}
    return nullptr;
}

Weapon * Player::GetWeapon() {
	for( auto uo : mUsableObjectList ) {
		if( typeid( *uo ) == typeid( Weapon )) {
			return dynamic_cast<Weapon*>( uo );
		}
	}
	return nullptr;
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

void Player::ManageEnvironmentDamaging() {
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

void Player::UpdateUsableObjects() {
	if( mCurrentUsableObject ) {
		for( auto & usableObject : mUsableObjectList ) {
			usableObject->GetModel().Hide();
		}
		mCurrentUsableObject->GetModel().Show();	
		if( ruGetMouseWheelSpeed() < 0 ) {
			mCurrentUsableObject->Prev();
		} else if( ruGetMouseWheelSpeed() > 0 ) {
			mCurrentUsableObject->Next();	
		}
		mCurrentUsableObject->Update();
	}
}

bool Player::IsDead() {
	return mHealth <= 0.0f;
}

void Player::SetPosition( ruVector3 position ) {
	Actor::SetPosition( position );
	mAirPosition = mBody.GetPosition(); // prevent death from 'accidental' landing :)
}

void Player::TrembleCamera( float time ) {
	mCameraTrembleTime = time;
}

void Player::TurnOffFakeLight() {
	ruSetLightRange( mFakeLight, 0.001f );
}

float Player::GetHealth() {
	return mHealth;
}

void Player::SetHealth( float health ) {
	mHealth = health;
}

void Player::LockFlashlight( bool state ) {
	mFlashlightLocked = state;
}

bool Player::AddUsableObject( UsableObject * usObj ) {
	bool alreadyGotObjectOfThisType = false;
	UsableObject * existingUsableObject = nullptr;
	for( auto uo : mUsableObjectList ) {
		if( typeid( *uo ) == typeid( *usObj ) ) {
			alreadyGotObjectOfThisType = true;
			existingUsableObject = uo;
			break;
		}
	}
	if( alreadyGotObjectOfThisType ) {
		if( dynamic_cast<Weapon*>( existingUsableObject )) {
			 dynamic_cast<Weapon*>( existingUsableObject )->LoadBullet();
		}

		if( dynamic_cast<Syringe*>( existingUsableObject )) {
			dynamic_cast<Syringe*>( existingUsableObject )->AddDoze();
		}

		delete usObj;

		// object is not added
		return false;
	} else {
		if( mCurrentUsableObject == nullptr ) {
			mCurrentUsableObject = usObj;
		}
		 
		// attach to camera
		usObj->GetModel().Attach( mpCamera->mNode );
		

		// register in inventory
		mInventory.AddItem( usObj->GetItemType() );

		// link last object with new to correct switching
		if( mUsableObjectList.size() > 0 ) {
			mUsableObjectList.at( mUsableObjectList.size() - 1 )->Link( usObj );
		}

		// add it to list
		mUsableObjectList.push_back( usObj );

		// object added
		return true;
	}
}

