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

	mWhispersSoundVolume = 0.0f;
	mWhispersSoundVolumeTo = 0.0f;

    mpSheetInHands = nullptr;

    // Camera bob vars
    mCameraShakeOffset = ruVector3( 0, mHeadHeight, 0 );
    mRunCameraShakeCoeff = 1.0f;
    mCameraBobCoeff = 0;

    // Effects vars
    mFrameColor = ruVector3( 1.0f, 1.0f, 1.0f );

    // Control vars
    mouseSens = 0.5f;
    mKeyMoveForward = ruInput::Key::W;
    mKeyMoveBackward = ruInput::Key::S;
    mKeyStrafeLeft = ruInput::Key::A;
    mKeyStrafeRight = ruInput::Key::D;
    mKeyJump = ruInput::Key::Space;
    mKeyFlashLight = ruInput::Key::F;
    mKeyRun = ruInput::Key::LeftShift;
    mKeyInventory = ruInput::Key::Tab;
    mKeyUse = ruInput::Key::R;
    mKeyStealth = ruInput::Key::C;
    mKeyLookLeft = ruInput::Key::Q;
    mKeyLookRight = ruInput::Key::E;

	mLastHealth = mHealth;

    // GUI vars
    //mStaminaAlpha = SmoothFloat( 255.0, 0.0f, 255.0f );
    //mHealthAlpha = SmoothFloat( 255.0, 0.0f, 255.0f );

    mpCurrentWay = nullptr;

    LoadGUIElements();
    CreateCamera();
    LoadSounds();
    CompleteObjective();

    mBody->SetName( "Player" );

    mGUIActionText = ruText::Create( "Action text", ruEngine::GetResolutionWidth() / 2 - 256, ruEngine::GetResolutionHeight() - 200, 512, 128, pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center );

    float scale = 2;
    int w = 512.0f / scale;
    int h = 256.0f / scale;
    mGUIBackground = ruRect::Create( 0, ruEngine::GetResolutionHeight() - h, w, h, mStatusBar, pGUIProp->mBackColor );
    for( int i = 0; i < mGUISegmentCount; i++ ) {
        mGUIStaminaBarSegment[i] = ruRect::Create( 44 + i * ( 8 + 2 ), ruEngine::GetResolutionHeight() - 3 * 15, 8, 16, ruTexture::Request( "data/gui/fatigue.png" ), pGUIProp->mForeColor );
        mGUIHealthBarSegment[i] = ruRect::Create( 44 + i * ( 8 + 2 ), ruEngine::GetResolutionHeight() - 4 * 26, 8, 16, ruTexture::Request( "data/gui/life.png" ), pGUIProp->mForeColor );
    }

	mGUIYouDiedFont = ruFont::LoadFromFile( 40, "data/fonts/font1.otf" );
	mGUIYouDied = ruText::Create( mLocalization.GetString( "youDied" ), (ruEngine::GetResolutionWidth() - 300) / 2, ruEngine::GetResolutionHeight() / 2, 300, 50, mGUIYouDiedFont, ruVector3( 255, 0, 0 ), ruTextAlignment::Center, 255 );
	mGUIYouDied->SetVisible( false );

	mInAir = false;

	// hack
	pMainMenu->SyncPlayerControls();

	mAutoSaveTimer = ruTimer::Create();

	mPainSound.push_back( ruSound::Load2D( "data/sounds/player/grunt1.ogg" ));
	mPainSound.push_back( ruSound::Load2D( "data/sounds/player/grunt2.ogg" ));
	mPainSound.push_back( ruSound::Load2D( "data/sounds/player/grunt3.ogg" ));

	mWhispersSound = ruSound::Load2D( "data/sounds/whispers.ogg" );
	mWhispersSound.SetVolume( 0.085f );
	mWhispersSound.SetLoop( true );
	//mWhispersSound.Play();

	for( auto & ps : mPainSound ) {
		ps.SetVolume( 0.7 ); 
	}

	mNodeInHands = nullptr;

	mSoundMaterialList.push_back( new SoundMaterial( "data/materials/stone.smat", mpCamera->mCamera ));
	mSoundMaterialList.push_back( new SoundMaterial( "data/materials/metal.smat", mpCamera->mCamera ));
	mSoundMaterialList.push_back( new SoundMaterial( "data/materials/wood.smat", mpCamera->mCamera ));
	mSoundMaterialList.push_back( new SoundMaterial( "data/materials/gravel.smat", mpCamera->mCamera ));
	mSoundMaterialList.push_back( new SoundMaterial( "data/materials/muddyrock.smat", mpCamera->mCamera ));
	mSoundMaterialList.push_back( new SoundMaterial( "data/materials/rock.smat", mpCamera->mCamera ));
	mSoundMaterialList.push_back( new SoundMaterial( "data/materials/grass.smat", mpCamera->mCamera ));
	mSoundMaterialList.push_back( new SoundMaterial( "data/materials/soil.smat", mpCamera->mCamera ));
	mSoundMaterialList.push_back( new SoundMaterial( "data/materials/chain.smat", mpCamera->mCamera ));

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
	mGUIActionText->Free( );
	for( int i = 0; i < mGUISegmentCount; i++ ) {
		mGUIHealthBarSegment[i]->Free( );
		mGUIStaminaBarSegment[i]->Free( );
	}
	mGUIBackground->Free( );
	mGUIStealthSign->Free( );
	mLighterCloseSound.Free();
	mLighterOpenSound.Free();
	mItemPickupSound.Free();
	mHeartBeatSound.Free();
	mBreathSound.Free();
	mWhispersSound.Free();
	if( mDeadSound.IsValid() ) {
		mDeadSound.Free();
	}
	for( auto sndMat : mSoundMaterialList ) {
		delete sndMat;
	}
	mGUICursorPickUp->Free( );
	mGUICursorPut->Free( );
	mGUICrosshair->Free( );
	mGUIYouDied->Free( );
	mGUIYouDiedFont->Free();
	mAutoSaveTimer->Free();
}

void Player::DrawStatusBar() {
    int segCount = mStamina / 5;
    for( int i = 0; i < mGUISegmentCount; i++ ) {
        if( i < segCount ) {
            mGUIStaminaBarSegment[i]->SetVisible( true );
        } else {
            mGUIStaminaBarSegment[i]->SetVisible( false );
        }
    }
    segCount = mHealth / 5;
    for( int i = 0; i < mGUISegmentCount; i++ ) {
        if( i < segCount ) {
            mGUIHealthBarSegment[i]->SetVisible( true );
        } else {
            mGUIHealthBarSegment[i]->SetVisible( false );
        }
    }
}

bool Player::IsCanJump( ) {    
	shared_ptr<ruSceneNode> node = ruPhysics::CastRay( mBody->GetPosition() + ruVector3( 0, 0.1, 0 ), mBody->GetPosition() - ruVector3( 0, mBodyHeight * 2, 0 ), 0 );
	if( node ) {
		if( node == mNodeInHands ) {
			mNodeInHands = nullptr;
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
            mBody->SetAngularFactor( ruVector3( 1.0f, 1.0f, 1.0f ));
            mBody->SetFriction( 1.0f );
            mBody->SetAnisotropicFriction( ruVector3( 1.0f, 1.0f, 1.0f ));
            mBody->Move( ruVector3( 1.0f, 1.0f, 1.0f ));
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
		if( ruInput::IsKeyHit( mKeyInventory ) ) {
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
        mPitch.SetTarget( mPitch.GetTarget() + ruInput::GetMouseYSpeed() * mouseSpeed );
        mYaw.SetTarget( mYaw.GetTarget() - ruInput::GetMouseXSpeed() * mouseSpeed );
    }

    if( mSmoothCamera ) {
        mPitch.ChaseTarget( 14.0f * g_dt );
        mYaw.ChaseTarget( 14.0f * g_dt );
    } else {
        mYaw = mYaw.GetTarget();
        mPitch = mPitch.GetTarget();
    }


    mpCamera->mCamera->SetRotation( ruQuaternion( ruVector3( 1, 0, 0 ), mPitch ) );
    mBody->SetRotation( ruQuaternion( ruVector3( 0, 1, 0 ), mYaw ) );

    mHeadAngle.SetTarget( 0.0f );
    if( ruInput::IsKeyDown( mKeyLookLeft )) {
        ruVector3 rayBegin = mBody->GetPosition() + ruVector3( mBodyWidth / 2, 0, 0 );
        ruVector3 rayEnd = rayBegin + mBody->GetRightVector() * 10.0f;
        ruVector3 hitPoint;
        shared_ptr<ruSceneNode> leftIntersection = ruPhysics::CastRay( rayBegin, rayEnd, &hitPoint );
        bool canLookLeft = true;
        if( leftIntersection ) {
            float dist2 = ( hitPoint - mBody->GetPosition() ).Length2();
            if( dist2 < 0.4 ) {
                canLookLeft = false;
            }
        }
        if( canLookLeft ) {
            mHeadAngle.SetTarget( mHeadAngle.GetMin() );
        }
    }

    if( ruInput::IsKeyDown( mKeyLookRight )) {
        ruVector3 rayBegin = mBody->GetPosition() - ruVector3( mBodyWidth / 2.0f, 0.0f, 0.0f );
        ruVector3 rayEnd = rayBegin - mBody->GetRightVector() * 10.0f;
        ruVector3 hitPoint;
        shared_ptr<ruSceneNode> rightIntersection = ruPhysics::CastRay( rayBegin, rayEnd, &hitPoint );
        bool canLookRight = true;
        if( rightIntersection ) {
            float dist2 = ( hitPoint - mBody->GetPosition() ).Length2();
            if( dist2 < 0.4f ) {
                canLookRight = false;
            }
        }
        if( canLookRight ) {
            mHeadAngle.SetTarget( mHeadAngle.GetMax() );
        }
    }
    mHeadAngle.ChaseTarget( 17.0f * g_dt );
    mHead->SetRotation( ruQuaternion( ruVector3( 0.0f, 0.0f, 1.0f ), mHeadAngle ));
}

void Player::UpdateJumping() {
    // do ray test, to determine collision with objects above camera
    shared_ptr<ruSceneNode> headBumpObject = ruPhysics::CastRay( mBody->GetPosition() + ruVector3( 0.0f, mBodyHeight * 0.98f, 0.0f ), mBody->GetPosition() + ruVector3( 0, 1.02 * mBodyHeight, 0 ), nullptr );

    if( ruInput::IsKeyHit( mKeyJump ) ) {
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



    if( mLanded || headBumpObject ) {
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
                SetActionText( StringBuilder() << ruInput::GetKeyName( mKeyUse ) << mLocalization.GetString( "crawlIn" ));
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
				SetActionText( StringBuilder() << ruInput::GetKeyName( mKeyUse ) << mLocalization.GetString( "openClose" ));
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
		mAirPosition = mBody->GetPosition();
		Crouch( false );
		mRunning = false;
        if( mpCurrentWay->IsPlayerInside() ) {
            mMoved = false;
            if( ruInput::IsKeyDown( mKeyMoveForward )) {
                mpCurrentWay->SetDirection( Way::Direction::Forward );
                mMoved = true;
            }
            if( ruInput::IsKeyDown( mKeyMoveBackward )) {
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
		if( ruInput::IsKeyHit( mKeyJump )) {
			mpCurrentWay->LeaveInstantly();
			mpCurrentWay = nullptr;
			Unfreeze();
			mBody->SetVelocity( ruVector3( -mBody->GetLookVector() ));
		}
    } else {
        ruVector3 look = mBody->GetLookVector();
        ruVector3 right = mBody->GetRightVector();

        mSpeedTo = ruVector3( 0, 0, 0 );

		bool moveBack = false;

        if( ruInput::IsKeyDown( mKeyMoveForward )) {
            mSpeedTo = mSpeedTo + look;
        }
        if( ruInput::IsKeyDown( mKeyMoveBackward )) {
            mSpeedTo = mSpeedTo - look;
			moveBack = true;
        }
        if( ruInput::IsKeyDown( mKeyStrafeLeft )) {
            mSpeedTo = mSpeedTo + right;
        }
        if( ruInput::IsKeyDown( mKeyStrafeRight )) {
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
        if( !IsCrouch() && ruInput::IsKeyDown( mKeyRun ) && mMoved && !mNodeInHands ) {
            if( mStamina > 0 ) {
                mSpeedTo = mSpeedTo * mRunSpeedMult;
                mStamina -= 8.0f * g_dt ;
                mFov.SetTarget( mFov.GetMax() );
                mRunCameraShakeCoeff = 1.425f;
                mRunning = true;
            }
        } else {
            if( mStamina < mMaxStamina ) {
				if( mInLight ) {
					mStamina += 16.0f * g_dt;
				}
            }
        }

		if( moveBack ) {
			mSpeedTo = mSpeedTo * 0.4f;
		}

		if( ruInput::IsKeyHit( mKeyStealth )) {
			Crouch( !IsCrouch() );
			mStealthMode = IsCrouch();
		}

		UpdateCrouch();

        mSpeedTo = mSpeedTo * ( mStealthMode ? 0.4f : 1.0f ) ;

        mFov.ChaseTarget( 4.0f * g_dt );
        mpCamera->mCamera->SetFOV( mFov );

        mSpeed = mSpeed.Lerp( mSpeedTo + mGravity, 10.0f * g_dt );
        Step( mSpeed * ruVector3( 100, 1, 100 ), g_dt );
    }

    UpdateCameraShake();
}

void ComputeGreyScaleFactor( shared_ptr<ruLight>light, ruVector3 pos ) {
	if( light->IsSeePoint( pos )) {	
		float factor = (pos - light->GetPosition()).Length() / (1.25f * light->GetRange());
		if( factor > 1.0f ) factor = 1.0f;
		if( factor < 0.0f ) factor = 0.0f;
		light->SetGreyscaleFactor( factor );
	} else {
		light->SetGreyscaleFactor( 1.0f );
	}
}

void Player::ComputeStealth() {
	for( int i = 0; i < ruPointLight::GetCount(); i++ ) {
		shared_ptr<ruLight>light = ruPointLight::Get( i );	
		if( !(light == mFakeLight) ) {
			ComputeGreyScaleFactor( light, mBody->GetPosition());
		}
	}
	for( int i = 0; i < ruSpotLight::GetCount(); i++ ) {
		shared_ptr<ruLight>light = ruSpotLight::Get( i );	
		ComputeGreyScaleFactor( light, mBody->GetPosition());
	}
	mFakeLight->SetGreyscaleFactor( 1.0f );


	
	mInLight = false;

    for( int i = 0; i < ruPointLight::GetCount(); i++ ) {
		if( !(ruPointLight::Get( i ) == mFakeLight)) {
			if( ruPointLight::Get( i )->IsSeePoint( mBody->GetPosition() )) {
				mInLight = true;
				break;
			}
		}
    }

    if( !mInLight ) {
        for( int i = 0; i < ruSpotLight::GetCount(); i++ ) {
            if( ruSpotLight::Get( i )->IsSeePoint( mBody->GetPosition() )) {
                mInLight = true;
                break;
            }
        }
    }

	if( pPlayer->GetFlashLight() ) {
		if( pPlayer->GetFlashLight()->IsOn() ) {
			mInLight = true;
		}
	}

	mWhispersSound.Play();

	if( mInLight ) {
		mWhispersSoundVolume -= 0.001f;
		if( mWhispersSoundVolume < 0.0f ) {
			mWhispersSoundVolume = 0.0f;
		}
	} else {
		mWhispersSoundVolume += 0.00004f;
		if( mWhispersSoundVolume > 0.085f ) {
			mWhispersSoundVolume = 0.085f;
		}
	}

	mWhispersSound.SetVolume( mWhispersSoundVolume );

    mStealthFactor = 0.0f;

    if( mInLight ) {
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
	mGUIStealthSign->SetAlpha( alpha );
	mGUIStealthSign->SetColor( color );
}


void Player::Update( ) {
    mGUIActionText->SetVisible( false );    
    mpCamera->Update();
	mDamageBackgroundAlpha--;
	if( mDamageBackgroundAlpha < 0 ) {
		mDamageBackgroundAlpha = 0;
	}

	if( pPlayer->GetFlashLight() ) {
		if( pPlayer->GetFlashLight()->IsOn() ) {
			mFakeLight->Hide();
		} else {
			mFakeLight->Show();
		}
	} else {
		mFakeLight->Show();
	}

	mGUIDamageBackground->SetAlpha( mDamageBackgroundAlpha );
    if( !pMainMenu->IsVisible() ) {
		mGoal.AnimateAndRender();
		DrawStatusBar();
		if( !mDead ) {		
			mGUIYouDied->SetVisible( false );
			mGUIStealthSign->SetVisible( mStealthMode );
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
					mAirPosition = mBody->GetPosition();
					mInAir = true;
				} else if( IsCanJump() && mInAir ) { // landing 
					ruVector3 curPos = mBody->GetPosition();
					float heightDelta = fabsf( curPos.y - mAirPosition.y );
					if( heightDelta > 2.0f ) {
						Damage( heightDelta * 10 );
					}
					mInAir = false;
				}
			}

			// this must be placed in other place :)
			if( mAutoSaveTimer->GetElapsedTimeInSeconds() >= 30 ) {
				SaveWriter( "autosave.save").SaveWorldState();
				mAutoSaveTimer->Restart();
			}
		} else {
			mGUIYouDied->SetVisible( true );
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
		mGUIYouDied->SetVisible( false );
	}
}

void Player::LoadGUIElements() {
    mItemPickupSound = ruSound::Load2D( "data/sounds/menuhit.ogg" );
    mStatusBar = ruTexture::Request( "data/gui/statusbar.tga" );

	mGUICursorPickUp = ruRect::Create( (ruEngine::GetResolutionWidth() - 32) / 2, (ruEngine::GetResolutionHeight() - 32) / 2, 32, 32, ruTexture::Request( "data/gui/up.tga" ) );
	mGUICursorPut = ruRect::Create( (ruEngine::GetResolutionWidth() - 32) / 2, (ruEngine::GetResolutionHeight() - 32) / 2, 32, 32, ruTexture::Request( "data/gui/down.tga" ) );
	mGUICrosshair = ruRect::Create( (ruEngine::GetResolutionWidth() - 32) / 2, (ruEngine::GetResolutionHeight() - 32) / 2, 32, 32, ruTexture::Request( "data/gui/crosshair.tga" ) );
	mGUIStealthSign = ruRect::Create( ruEngine::GetResolutionWidth() / 2 - 32, 200, 64, 32, ruTexture::Request( "data/textures/effects/eye.png" ));

	mDamageBackgroundAlpha = 0;
	mGUIDamageBackground = ruRect::Create( 0, 0, ruEngine::GetResolutionWidth(), ruEngine::GetResolutionHeight(), ruTexture::Request("data/textures/effects/damageBackground.tga" ),ruVector3( 200, 0, 0 ), mDamageBackgroundAlpha );
}

void Player::CreateCamera() {
    mHeadHeight = 2.1;

    mHead = ruSceneNode::Create();
    mHead->Attach( mBody );
    mHead->SetPosition( ruVector3( 0, -2.0f, 0.0f ));
    mpCamera = new GameCamera( mFov );
    mpCamera->mCamera->Attach( mHead );
    mCameraOffset = ruVector3( 0, mHeadHeight, 0 );
    mCameraShakeOffset = ruVector3( 0, mHeadHeight, 0 );

    // Pick
    mPickPoint = ruSceneNode::Create();
    mPickPoint->Attach( mpCamera->mCamera );
    mPickPoint->SetPosition( ruVector3( 0, 0, 0.1 ));

    mItemPoint = ruSceneNode::Create();
    mItemPoint->Attach( mpCamera->mCamera );
    mItemPoint->SetPosition( ruVector3( 0, 0, 1.0f ));

	mFakeLight = ruPointLight::Create();
	mFakeLight->Attach( mpCamera->mCamera );
	mFakeLight->SetRange( 2 );
	mFakeLight->SetColor( ruVector3( 25, 25, 25 ));
}

void Player::LoadSounds() {
    mLighterCloseSound = ruSound::Load3D( "data/sounds/lighter_close.ogg" );
    mLighterOpenSound = ruSound::Load3D( "data/sounds/lighter_open.ogg" );

    mLighterCloseSound.Attach( mpCamera->mCamera );
    mLighterOpenSound.Attach( mpCamera->mCamera );

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
				if( mpCurrentWay->GetEnterZone()->GetTextureCount() > 0 ) {
					for( auto sMat : mSoundMaterialList ) {
						ruSound snd = sMat->GetRandomSoundAssociatedWith( mpCurrentWay->GetEnterZone()->GetTexture( 0 )->GetName() );
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
    mpCamera->mCamera->SetPosition( mCameraOffset );
}

void Player::DrawSheetInHands() {
    if( mpSheetInHands ) {
		SetActionText( StringBuilder() << mpSheetInHands->GetDescription() << mLocalization.GetString( "sheetOpen" ));
        mpSheetInHands->SetVisible( true );
        mpSheetInHands->Draw();
        if( ruInput::IsMouseHit( ruInput::MouseButton::Right ) || ( mpSheetInHands->mObject->GetPosition() - mBody->GetPosition() ).Length2() > 2 ) {
            CloseCurrentSheet();
        }
    }
}

void Player::UpdateCursor() {
	if( mInventory.IsOpened() )	{
		mGUICursorPut->SetVisible( false );
		mGUICrosshair->SetVisible( false );
		mGUICursorPickUp->SetVisible( false );	
	} else {
		if( mNearestPickedNode ) {
			if( IsObjectHasNormalMass( mNearestPickedNode )) {
				mGUICursorPickUp->SetVisible( true );		
				mGUICrosshair->SetVisible( false );
			} else {			
				mGUICursorPut->SetVisible( false );
				mGUICrosshair->SetVisible( true );
			}		
		} else {
			mGUICursorPickUp->SetVisible( false );
			if( mNodeInHands ) {
				mGUICursorPut->SetVisible( true );
				mGUICrosshair->SetVisible( false );
			} else {
				mGUICursorPut->SetVisible( false );
				mGUICrosshair->SetVisible( true );
			}
		}
	}
}

void Player::UpdateItemsHandling() {
	if( !mInventory.IsOpened() ) {
		if( mNearestPickedNode ) {
			if( IsUseButtonHit() ) {
				Sheet * pSheet = Sheet::GetSheetPointerByNode( mNearestPickedNode );

				if( mpSheetInHands ) {
					CloseCurrentSheet();
				} else {
					if( pSheet ) {
						mpSheetInHands = pSheet;
						mpSheetInHands->mObject->Hide();
						Sheet::msPaperFlipSound.Play();
					}
				}
			}
		}
	}

	if( mNodeInHands ) {
		if( mPitch < 70 ) {
			ruVector3 ppPos = mItemPoint->GetPosition();
			ruVector3 objectPos = mNodeInHands->GetPosition() + mPickCenterOffset ;
			ruVector3 dir = ppPos - objectPos;
			if( ruInput::IsMouseDown( ruInput::MouseButton::Left ) ) {
				mNodeInHands->Move( dir * 6 );

				mNodeInHands->SetAngularVelocity( ruVector3( 0, 0, 0 ));

				if( ruInput::IsMouseDown( ruInput::MouseButton::Right ) ) {
					if( UseStamina( mNodeInHands->GetMass() )) {
						mNodeInHands->Move(( ppPos - mpCamera->mCamera->GetPosition() ).Normalize() * 6 );
					}

					mObjectThrown = true;
					mNodeInHands = nullptr;
				}
			} else {
				mNodeInHands->SetAngularVelocity( ruVector3( 1, 1, 1 ));
				mNodeInHands = nullptr;
			}
		} else {
			mNodeInHands = nullptr;
		}
    }


    if( !ruInput::IsMouseDown( ruInput::MouseButton::Left )) {
        mObjectThrown = false;
    }
}

void Player::UpdatePicking() {
    ruVector3 pickPosition;

    mPickedNode = ruPhysics::RayPick( ruEngine::GetResolutionWidth() / 2, ruEngine::GetResolutionHeight() / 2, &pickPosition );

    mNearestPickedNode = nullptr;

	for( auto pEnemy : Enemy::msEnemyList ) {
		if( mPickedNode == pEnemy->GetBody() ) {
			mPickedNode = nullptr;
		}
	}

    if( mPickedNode && !mNodeInHands  ) {
        mNodeInHands = nullptr;

        ruVector3 ppPos = mPickPoint->GetPosition();
        ruVector3 dir = ppPos - pickPosition;

        InteractiveObject * pIO = InteractiveObject::FindByObject( mPickedNode );
        Sheet * pSheet = Sheet::GetSheetPointerByNode( mPickedNode );

        if( dir.Length2() < 1.5f ) {
            mNearestPickedNode = mPickedNode;
			string pickedObjectDesc;
            if( pIO ) {
                pickedObjectDesc = StringBuilder() << pIO->GetPickDescription() << "- [" << ruInput::GetKeyName( mKeyUse).c_str() << "] " << mLocalization.GetString( "itemPick" );
                SetActionText( pickedObjectDesc );
            } else if( pSheet ) {
                pickedObjectDesc = StringBuilder() << pSheet->GetDescription() << "- [" << ruInput::GetKeyName( mKeyUse ) << "] " << mLocalization.GetString( "sheetPick" );
                SetActionText( pickedObjectDesc );
            } else {
                if( IsObjectHasNormalMass( mPickedNode ) && !mPickedNode->IsFrozen()) {
                    SetActionText( mLocalization.GetString( "objectPick" ) );
                }
            }

            if( ruInput::IsMouseDown( ruInput::MouseButton::Left ) ) {
                if( IsObjectHasNormalMass( mPickedNode )) {
                    if( !mPickedNode->IsFrozen() && !mObjectThrown ) {
                        mNodeInHands = mPickedNode;
						mPickCenterOffset = pickPosition -  mPickedNode->GetPosition();
                    }
                }
            }
        }
    }
}

void Player::FreeHands() {
    mNodeInHands = nullptr;
}

bool Player::IsUseButtonHit() {
    return ruInput::IsKeyHit( mKeyUse );
}

bool Player::IsObjectHasNormalMass( shared_ptr<ruSceneNode> node ) {
    return node->GetMass() > 0 && node->GetMass() < 40;
}

void Player::Resurrect() {
	mHealth = 100.0f;
	mDead = false;
	mpCamera->SetFadeColor( ruVector3( 255, 255, 255 ));
	mBody->SetAngularFactor( ruVector3( 0, 0, 0 ));
	mBody->SetRotation( ruQuaternion( 0, 0, 0 ));
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
	
    mBody->SetLocalPosition( in.ReadVector3() );

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
        pPlayer->mBody->Freeze();
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

   // mStaminaAlpha.Deserialize( in );
//    mHealthAlpha.Deserialize( in );

    in.ReadBoolean( mMoved );
    in.ReadBoolean( mObjectiveDone );

    mBreathVolume.Deserialize( in );
    mHeartBeatVolume.Deserialize( in );
    mHeartBeatPitch.Deserialize( in );
    mBreathPitch.Deserialize( in );

    mpSheetInHands = Sheet::GetSheetPointerByNode( ruSceneNode::FindByName( in.ReadString() ));

    mKeyMoveForward = static_cast<ruInput::Key>( in.ReadInteger());
    mKeyMoveBackward = static_cast<ruInput::Key>( in.ReadInteger());
    mKeyStrafeLeft = static_cast<ruInput::Key>( in.ReadInteger());
    mKeyStrafeRight = static_cast<ruInput::Key>( in.ReadInteger());
    mKeyJump = static_cast<ruInput::Key>( in.ReadInteger());
    mKeyFlashLight = static_cast<ruInput::Key>( in.ReadInteger());
    mKeyRun = static_cast<ruInput::Key>( in.ReadInteger());
    mKeyInventory = static_cast<ruInput::Key>( in.ReadInteger());
    mKeyUse = static_cast<ruInput::Key>( in.ReadInteger());

    mStealthMode = in.ReadBoolean();

    mTip.Deserialize( in );
	
	in.ReadBoolean( mFlashlightLocked );

	in.ReadFloat( mLastHealth );


	mpCamera->FadePercent( 100 );
	mpCamera->SetFadeColor( ruVector3( 255, 255, 255 ) );
	mBody->SetFriction( 0 );

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

    mBody->Unfreeze();
    out.WriteVector3( mBody->GetLocalPosition() );
    mBody->SetAngularFactor( ruVector3( 0, 0, 0 ));

    out.WriteBoolean( mSmoothCamera );
    out.WriteFloat( mRunCameraShakeCoeff );
    mPitch.Serialize( out );
    mYaw.Serialize( out );
    out.WriteVector3( mSpeed );
    out.WriteVector3( mSpeedTo );
    out.WriteVector3( mGravity );
    out.WriteVector3( mJumpTo );

    out.WriteString( mpCurrentWay ? mpCurrentWay->GetEnterZone()->GetName() : "undefinedWay" );

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

   // mStaminaAlpha.Serialize( out );
//    mHealthAlpha.Serialize( out );

    out.WriteBoolean( mMoved );
    out.WriteBoolean( mObjectiveDone );

    mBreathVolume.Serialize( out );
    mHeartBeatVolume.Serialize( out );
    mHeartBeatPitch.Serialize( out );
    mBreathPitch.Serialize( out );

    out.WriteString( mpSheetInHands ? mpSheetInHands->mObject->GetName() : "undefinedSheet" );

    out.WriteInteger( static_cast<int>( mKeyMoveForward ));
    out.WriteInteger( static_cast<int>( mKeyMoveBackward ));
    out.WriteInteger( static_cast<int>( mKeyStrafeLeft ));
    out.WriteInteger( static_cast<int>( mKeyStrafeRight ));
    out.WriteInteger( static_cast<int>( mKeyJump ));
    out.WriteInteger( static_cast<int>( mKeyFlashLight ));
    out.WriteInteger( static_cast<int>( mKeyRun ));
    out.WriteInteger( static_cast<int>( mKeyInventory ));
    out.WriteInteger( static_cast<int>( mKeyUse ));

    out.WriteBoolean( mStealthMode );

    mTip.Serialize( out );

	out.WriteBoolean( mFlashlightLocked );

	out.WriteFloat( mLastHealth );

	mInventory.Serialize( out );
}

void Player::CloseCurrentSheet() {
    mpSheetInHands->SetVisible( false );
    mpSheetInHands->mObject->Show();
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
    mGUIActionText->SetText( text );
    mGUIActionText->SetVisible( true );
}

void Player::SetHUDVisible( bool state ) {
    for( int i = 0; i < mGUISegmentCount; i++ ) {
        mGUIHealthBarSegment[i]->SetVisible( state );
        mGUIStaminaBarSegment[i]->SetVisible( state );
    }
    mGUIBackground->SetVisible( state );
    mGUIActionText->SetVisible( state );
	mGUIStealthSign->SetVisible( state );	
    mGoal.SetVisible( state );
	if( !state ) {
		mTip.SetVisible( state );
		if( mpSheetInHands ) {
			CloseCurrentSheet();
		}
		mInventory.SetVisible( state );
		mGUICursorPickUp->SetVisible( state );		
		mGUICrosshair->SetVisible( state );		
		mGUICursorPut->SetVisible( state );
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
			usableObject->GetModel()->Hide();
		}
		mCurrentUsableObject->GetModel()->Show();	
		if( ruInput::GetMouseWheelSpeed() < 0 ) {
			mCurrentUsableObject->Prev();
		} else if( ruInput::GetMouseWheelSpeed() > 0 ) {
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
	mAirPosition = mBody->GetPosition(); // prevent death from 'accidental' landing :)
}

void Player::TrembleCamera( float time ) {
	mCameraTrembleTime = time;
}

void Player::TurnOffFakeLight() {
	mFakeLight->SetRange( 0.001f );
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
		usObj->GetModel()->Attach( mpCamera->mCamera );
		

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

void Player::DumpUsableObjects( vector<UsableObject*> & otherPlace )
{
	// this method is useful to transfer usable objects between levels
	for( auto uo : mUsableObjectList ) {
		uo->GetModel()->Detach();
	}
	otherPlace = mUsableObjectList;
	mUsableObjectList.clear();
}

void Player::Step( ruVector3 direction, float speed )
{
	// spring based step
	ruVector3 currentPosition = mBody->GetPosition();
	ruVector3 rayBegin = currentPosition;
	ruVector3 rayEnd = rayBegin - ruVector3( 0, 5, 0 );
	ruVector3 intPoint;
	shared_ptr<ruSceneNode> rayResult = ruPhysics::CastRay( rayBegin, rayEnd, &intPoint );
	ruVector3 pushpullVelocity = ruVector3( 0,0,0 );
	if( rayResult && !(rayResult == mBody)  ) {
		pushpullVelocity.y = -( currentPosition.y - intPoint.y - mSpringLength * mCrouchMultiplier  ) * 4.4f;
	}
	mBody->Move( direction * speed + pushpullVelocity );
}

void Player::EmitStepSound() {
	ruRayCastResultEx result = ruPhysics::CastRayEx( mBody->GetPosition() + ruVector3( 0, 0.1, 0 ), mBody->GetPosition() - ruVector3( 0, mBodyHeight * 2.2, 0 ));
	if( result.valid ) {
		for( auto sMat : mSoundMaterialList ) {
			ruSound snd = sMat->GetRandomSoundAssociatedWith( result.textureName );
			if( snd.IsValid() ) {
				snd.Play( true );
			}
		}
	}
}

