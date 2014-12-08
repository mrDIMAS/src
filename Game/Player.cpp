#include "Player.h"
#include "Menu.h"
#include "Door.h"
#include "utils.h"

Player * player = 0;

/*
========
Player::Player
========
*/
Player::Player() : Actor( 1.0f, 0.2f ) {
    localization.ParseFile( localizationPath + "player.loc" );

    // Stamina vars
    maxStamina = 100.0f;
    stamina = maxStamina;

    fov = SmoothFloat( 75.0f, 75.0f, 80.0f );

    // Run vars
    runSpeedMult = 2.5f;

    objectThrown = false;
    smoothCamera = true;

    // Life vars
    maxLife = 100;
    life = maxLife;

    pitch = SmoothFloat( 0.0f, -89.9f, 89.9f );
    yaw = SmoothFloat( 0.0f );
    damagePitchOffset = SmoothFloat( 0.0f );
	stealthOffset = SmoothFloat( 0.0f, -0.45f, 0.0f );
	headAngle = SmoothFloat( 0.0f, -12.50f, 12.50f );
    // State vars
    dead = false;
    landed = false;
    locked = false;
	stealthMode = false;

    sheetInHands = nullptr;

    // Camera bob vars

    cameraBob = ruVector3( 0, headHeight, 0 );
    runBobCoeff = 1.0f;
    cameraBobCoeff = 0;

    // Effects vars
    frameColor = ruVector3( 1.0f, 1.0f, 1.0f );

    // Control vars
    mouseSens = 0.5f;
    keyMoveForward = KEY_W;
    keyMoveBackward = KEY_S;
    keyStrafeLeft = KEY_A;
    keyStrafeRight = KEY_D;
    keyJump = KEY_Space;
    keyFlashLight = KEY_F;
    keyRun = KEY_LeftShift;
    keyInventory = KEY_Tab;
    keyUse = KEY_R;
	keyStealth = KEY_C;
	keyLookLeft = KEY_Q;
	keyLookRight = KEY_E;
    // GUI vars
    staminaAlpha = SmoothFloat( 255.0, 0.0f, 255.0f );
    healthAlpha = SmoothFloat( 255.0, 0.0f, 255.0f );

    currentWay = nullptr;

	stealthSign = ruGetTexture( "data/textures/effects/eye.png" );

    LoadGUIElements();
    CreateCamera();
    CreateFlashLight();
    LoadSounds();
    CompleteObjective();
    SetDirtFootsteps();

	ruSetNodeName( body, "Player" );
}

/*
========
Player::SetPlaceDescription
========
*/
void Player::SetPlaceDescription( string desc ) {
    placeDesc = desc;
    placeDescTimer = 240;
}

/*
========
Player::DrawStatusBar
========
*/
void Player::DrawStatusBar() {
    if( moved ) {
        staminaAlpha.SetTarget( 255 );
        healthAlpha.SetTarget( 255 );
    } else {
        staminaAlpha.SetTarget( 50 );
        healthAlpha.SetTarget( 50 );
    }

    staminaAlpha.ChaseTarget( 8.0f * g_dt );
    healthAlpha.ChaseTarget( 8.0f * g_dt );

    float scale = 2;
    int w = 512.0f / scale;
    int h = 256.0f / scale;

    ruDrawGUIRect( 0, ruGetResolutionHeight() - h, w, h, statusBar, ruVector3( 255, 255, 255 ), staminaAlpha );

    int segCount = stamina / 5;
    for( int i = 0; i < segCount; i++ ) {
        ruDrawGUIRect( 44 + i * ( 8 + 2 ), ruGetResolutionHeight() - 3 * 15, 8, 16, gui->staminaBarImg, ruVector3( 255, 255, 255 ), staminaAlpha );
    }

    segCount = life / 5;
    for( int i = 0; i < segCount; i++ ) {
        ruDrawGUIRect( 44 + i * ( 8 + 2 ), ruGetResolutionHeight() - 4 * 26, 8, 16, gui->lifeBarImg, ruVector3( 255, 255, 255 ), healthAlpha );
    }
}

/*
========
Player::CanJump
========
*/
bool Player::CanJump( ) {
	ruNodeHandle legBump = ruCastRay( ruGetNodePosition( body ) + ruVector3( 0, 0.1, 0 ), ruGetNodePosition( body ) - ruVector3( 0, capsuleHeight, 0 ), 0 );
	if( legBump.IsValid() ) {
		return true;
	} else {
		return false;
	}
}

/*
========
Player::GotItemAnyOfType
========
*/
int Player::GotItemAnyOfType( int type ) {
    for( auto item : inventory.items ) {
        if( item->type == type ) {
            return true;
        }
	}
    return false;
}

/*
========
Player::UseStamina
========
*/
bool Player::UseStamina( float required ) {
    if( stamina - required < 0 ) {
        return false;
    }

    stamina -= required;

    return true;
}

/*
========
Player::Damage
========
*/
void Player::Damage( float dmg ) {
    life -= dmg;

    damagePitchOffset.SetTarget( 20.0f );

    if( life < 0 ) {
        if( !dead ) {
            ruSetAngularFactor( body, ruVector3( 1, 1, 1 ));
            ruSetNodeFriction( body, 1 );
            ruSetNodeAnisotropicFriction( body, ruVector3( 1, 1, 1 ));
            ruMoveNode( body, ruVector3( 1,1,1));
        }

        dead = true;
        life = 0;

        flashlight->SwitchOff();

        camera->FadePercent( 5 );
        camera->SetFadeColor( ruVector3( 70, 0, 0 ) );
    }
}

/*
========
Player::AddItem
========
*/
void Player::AddItem( Item * itm ) {
    if( !itm ) {
        return;
    }

    for( auto item : inventory.items )
        if( item == itm ) {
            return;
        }

    ruFreeze( itm->object );
    ruDetachNode( itm->object );
    itm->inInventory = true;

    ruSetNodePosition( itm->object, ruVector3( 10000, 10000, 10000 )); // far far away

    inventory.items.push_back( itm );
}

/*
========
Player::UpdateInventory
========
*/
void Player::UpdateInventory() {
    if( ruIsKeyHit( keyInventory ) && !locked ) {
        inventory.opened = !inventory.opened;
    }

    inventory.Update();
}

/*
========
Player::SetObjective
========
*/
void Player::SetObjective( string text ) {
    string objectiveText = localization.GetString( "currentObjective" );
    objectiveText += text;

    goal.SetText( objectiveText );

    objectiveDone = false;
}

/*
========
Player::CompleteObjective
========
*/
void Player::CompleteObjective() {
    SetObjective( localization.GetString( "objectiveUnknown" ));

    objectiveDone = true;
}

/*
========
Player::UpdateMouseLook
========
*/
void Player::UpdateMouseLook() {
    if( currentWay ) {
        smoothCamera = false;
    } else {
        smoothCamera = true;
    }

    if( !inventory.opened ) {
        float mouseSpeed =  mouseSens / 2.0f;
        if( currentWay ) {
            if( !currentWay->IsFreeLook() ) {
                mouseSpeed = 0.0f;
            }
        }
        pitch.SetTarget( pitch.GetTarget() + ruGetMouseYSpeed() * mouseSpeed );
        yaw.SetTarget( yaw.GetTarget() - ruGetMouseXSpeed() * mouseSpeed );
    }
    
    damagePitchOffset.ChaseTarget( 20.0f * g_dt );

    if( damagePitchOffset >= 19.0f ) {
        damagePitchOffset.SetTarget( 0.0f );
    }

    if( !locked ) {
        if( smoothCamera ) {
            pitch.ChaseTarget( 14.0f * g_dt );
            yaw.ChaseTarget( 14.0f * g_dt );
        } else {
            yaw = yaw.GetTarget();
            pitch = pitch.GetTarget();
        }
    }

    //SetRotation( camera->cameraNode, Quaternion( Vector3( 1, 0, 0 ), pitch + damagePitchOffset ) );
	ruSetNodeRotation( camera->cameraNode, ruQuaternion( ruVector3( 1, 0, 0 ), pitch ) );
    ruSetNodeRotation( body, ruQuaternion( ruVector3( 0, 1, 0 ), yaw ) );

	headAngle.SetTarget( 0.0f );
	if( ruIsKeyDown( keyLookLeft )) {
		headAngle.SetTarget( headAngle.GetMin() );
	}
	if( ruIsKeyDown( keyLookRight )) {
		headAngle.SetTarget( headAngle.GetMax() );
	}
	headAngle.ChaseTarget( 17.0f * g_dt );
	ruSetNodeRotation( head, ruQuaternion( ruVector3( 0, 0, 1 ), headAngle ));
}

/*
========
Player::UpdateJumping
========
*/
void Player::UpdateJumping() {	
	// do ray test, to determine collision with objects above camera
	ruNodeHandle headBumpObject = ruCastRay( ruGetNodePosition( body ) + ruVector3( 0, capsuleHeight * 0.98, 0 ), ruGetNodePosition( body ) + ruVector3( 0, 1.02 * capsuleHeight, 0 ), nullptr );
	

    if( ruIsKeyHit( keyJump ) && !locked ) {
       if( CanJump() ) {
            jumpTo = ruVector3( 0, 150, 0 );
            landed = false;
       }
    }

    gravity = gravity.Lerp( jumpTo, 40.0f * g_dt );

    if( gravity.y >= jumpTo.y ) {
        landed = true;
    }

    if( landed || headBumpObject.IsValid() ) {
        jumpTo = ruVector3( 0, -150, 0 );
        if( CanJump() ) {
            jumpTo = ruVector3( 0, 0, 0 );
        }
    };
}

/*
========
Player::UpdateMoving
========
*/
void Player::UpdateMoving() {
    for( auto cw : Way::all ) {
        if( cw->IsEnterPicked() ) {
            if( !cw->IsPlayerInside() ) {
                DrawTip( Format( localization.GetString( "crawlIn" ), GetKeyName( keyUse ).c_str()));
            }

            if( IsUseButtonHit() ) {
                cw->Enter();
            }
        }
    }

    for( auto door : Door::all ) {
        door->DoInteraction();

        if( door->IsPickedByPlayer() ) {
            DrawTip( Format( localization.GetString( "openClose" ), GetKeyName( keyUse ).c_str()));

            if( IsUseButtonHit() ) {
                door->SwitchState();
            }
        }
    }

	if( ruIsKeyHit( keyStealth )) {
		stealthMode = !stealthMode;
	}

    if( currentWay ) {
		stealthMode = false;

        currentWay->DoEntering();

        if( !currentWay->IsFreeLook() ) {
            currentWay->LookAtTarget();
        }

        if( currentWay->IsPlayerInside() ) {
            bool move = false;

            if( ruIsKeyDown( keyMoveForward )) {
                currentWay->SetDirection( Way::Direction::Forward );
                move = true;
            }
            if( ruIsKeyDown( keyMoveBackward )) {
                currentWay->SetDirection( Way::Direction::Backward );
                move = true;
            }

            if( move ) {
                currentWay->DoPlayerCrawling();

                moved = true;

                if( !currentWay->IsPlayerInside() ) {
                    currentWay = nullptr;
                }
            } else {
                ruMoveNode( player->body, ruVector3( 0, 0, 0 ));

                moved = false;
            }
        }
    } else {
        ruVector3 look = ruGetNodeLookVector( body );
        ruVector3 right = ruGetNodeRightVector( body );

        speedTo = ruVector3( 0, 0, 0 );

        if( !locked ) {
            if( ruIsKeyDown( keyMoveForward )) {
                speedTo = speedTo + look;
            }
            if( ruIsKeyDown( keyMoveBackward )) {
                speedTo = speedTo - look;
            }
            if( ruIsKeyDown( keyStrafeLeft )) {
                speedTo = speedTo + right;
            }
            if( ruIsKeyDown( keyStrafeRight )) {
                speedTo = speedTo - right;
            }
        }

        moved = speedTo.Length2() > 0;

        if( moved ) {
            speedTo = speedTo.Normalize();
        }

        UpdateJumping();

        runBobCoeff = 1.0f;
        fov.SetTarget( fov.GetMin() );

		running = false;
        if( ruIsKeyDown( keyRun ) && moved ) {
            if( stamina > 0 ) {
                speedTo = speedTo * runSpeedMult;
                stamina -= 8.0f * g_dt ;
                fov.SetTarget( fov.GetMax() );
                runBobCoeff = 1.425f;
				running = true;
            }
        } else {
            if( stamina < maxStamina ) {
                stamina += 16.0f * g_dt;
            }
        }

		speedTo = speedTo * ( stealthMode ? 0.4f : 1.0f ) ;

        fov.ChaseTarget( 4.0f * g_dt );
        ruSetCameraFOV( camera->cameraNode, fov );

        speed = speed.Lerp( speedTo + gravity, 10.0f * g_dt );
		Move( speed * ruVector3( 100, 1, 100 ), g_dt );
    }

    UpdateCameraBob();
}

/*
========
Player::ComputeStealth
========
*/
void Player::ComputeStealth() {
	bool inLight = false;
	ruVector3 pos = ruGetNodePosition( body );
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

	if( flashlight->on ) {
		inLight = true;
	}

	stealthFactor = 0.0f;
		
	if( inLight ) {
		stealthFactor += stealthMode ? 0.25f : 0.5f;
		stealthFactor += moved ? 0.5f : 0.0f;
		stealthFactor += running ? 1.0f : 0.0f;
	} else {
		stealthFactor += stealthMode ? 0.0f : 0.25f;
		stealthFactor += moved ? 0.1f : 0.0f;
		stealthFactor += running ? 0.25f : 0.0f;
	}


	for( auto snd : footsteps ) {
		if( stealthMode ) {
			ruSetSoundVolume( snd, 0.15f );
		} else {
			ruSetSoundVolume( snd, 0.4f );
		}
	}

	int alpha = ( 255 * ( ( stealthFactor > 1.05f ) ? 1.0f : ( stealthFactor + 0.05f ) ) );
	if (alpha > 255 )
		alpha = 255;
	ruVector3 color = ( stealthFactor < 1.05f ) ? ruVector3( 255, 255, 255 ) : ruVector3( 255, 0, 0 );
	ruDrawGUIRect( ruGetResolutionWidth() / 2 - 32, 200, 64, 32, stealthSign, color, alpha );
	//DrawGUIText( Format( "%f, %d ", stealthFactor, alpha ).c_str(), 100, 300,100,100, gui->font, Vector3( 255, 0, 222), 0 );
}
/*
========
Player::Update
========
*/
void Player::Update( ) {
    UpdateFright();
    camera->Update();

    if( menu->visible ) {
        return;
    }

    DrawGUIElements();
    DrawStatusBar();
    UpdateFlashLight();

    if( dead ) {
        return;
	}	
    tip.AnimateAndDraw();
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
void Player::LoadGUIElements() {
    upCursor = ruGetTexture( "data/gui/up.png" );
    downCursor = ruGetTexture( "data/gui/down.png" );
    pickupSound = ruLoadSound2D( "data/sounds/menuhit.ogg" );
    statusBar = ruGetTexture( "data/gui/statusbar.png" );
}

/*
========
Player::CreateCamera
========
*/
void Player::CreateCamera() {
	headHeight = 2.4;

	head = ruCreateSceneNode();
	ruAttachNode( head, body );
	ruSetNodePosition( head, ruVector3( 0, -2.0f, 0.0f ));
    camera = new GameCamera( fov );
    ruAttachNode( camera->cameraNode, head );
	cameraOffset = ruVector3( 0, headHeight, 0 );
    cameraBob = ruVector3( 0, headHeight, 0 );

    // Pick
    pickPoint = ruCreateSceneNode();
    ruAttachNode( pickPoint, camera->cameraNode );
    ruSetNodePosition( pickPoint, ruVector3( 0, 0, 0.1 ));

    itemPoint = ruCreateSceneNode();
    ruAttachNode( itemPoint, camera->cameraNode );
    ruSetNodePosition( itemPoint, ruVector3( 0, 0, 1.0f ));
}

/*
========
Player::SetRockFootsteps
========
*/
void Player::SetRockFootsteps() {
    for( auto s : footsteps ) {
        ruFreeSound( s );
    }

    footsteps.clear();

    footsteps.push_back( ruLoadSound3D( "data/sounds/stonestep1.ogg" ) );
    footsteps.push_back( ruLoadSound3D( "data/sounds/stonestep2.ogg" ) );
    footsteps.push_back( ruLoadSound3D( "data/sounds/stonestep3.ogg" ) );
    footsteps.push_back( ruLoadSound3D( "data/sounds/stonestep4.ogg" ) );
	footsteps.push_back( ruLoadSound3D( "data/sounds/stonestep5.ogg" ) );
	footsteps.push_back( ruLoadSound3D( "data/sounds/stonestep6.ogg" ) );
	footsteps.push_back( ruLoadSound3D( "data/sounds/stonestep7.ogg" ) );
	footsteps.push_back( ruLoadSound3D( "data/sounds/stonestep8.ogg" ) );
	footsteps.push_back( ruLoadSound3D( "data/sounds/stonestep9.ogg" ) );
	footsteps.push_back( ruLoadSound3D( "data/sounds/stonestep10.ogg" ) );
	footsteps.push_back( ruLoadSound3D( "data/sounds/stonestep11.ogg" ) );
	footsteps.push_back( ruLoadSound3D( "data/sounds/stonestep12.ogg" ) );

    for( auto s : footsteps ) {
        ruAttachSound( s, body );
        ruSetSoundVolume( s, 0.45f );
    }

    footstepsType = FootstepsType::Rock;
}

/*
========
Player::SetDirtFootsteps
========
*/
void Player::SetDirtFootsteps() {
    for( auto s : footsteps ) {
        ruFreeSound( s );
    }

    footsteps.clear();

    footsteps.push_back( ruLoadSound3D( "data/sounds/dirt1.ogg" ) );
    footsteps.push_back( ruLoadSound3D( "data/sounds/dirt2.ogg" ) );
    footsteps.push_back( ruLoadSound3D( "data/sounds/dirt3.ogg" ) );
    footsteps.push_back( ruLoadSound3D( "data/sounds/dirt4.ogg" ) );

    for( auto s : footsteps ) {
        ruAttachSound( s, body );
        ruSetSoundVolume( s, 0.45f );
    }

    footstepsType = FootstepsType::Dirt;
}

/*
========
Player::LoadSounds
========
*/
void Player::LoadSounds() {
    lighterCloseSound = ruLoadSound3D( "data/sounds/lighter_close.ogg" );
    lighterOpenSound = ruLoadSound3D( "data/sounds/lighter_open.ogg" );

    ruAttachSound( lighterCloseSound, camera->cameraNode );
    ruAttachSound( lighterOpenSound, camera->cameraNode );

    heartBeatSound = ruLoadSound2D( "data/sounds/heart.ogg" );
    breathSound = ruLoadSound2D( "data/sounds/breath.ogg" );

    ruSetSoundReferenceDistance( heartBeatSound, 100.0f );
    ruSetSoundReferenceDistance( breathSound, 100.0f );

    breathVolume = SmoothFloat( 0.1f );
    heartBeatVolume = SmoothFloat( 0.15f );
    heartBeatPitch = SmoothFloat( 1.0f );
    breathPitch = SmoothFloat( 1.0f );
}

/*
========
Player::DoFright
========
*/
void Player::DoFright() {
    breathVolume.SetTarget( 0.1f );
    breathVolume.Set( 0.25f );
    heartBeatVolume.SetTarget( 0.15f );
    heartBeatVolume.Set( 0.45f );    
    heartBeatPitch.Set( 2.0f );
    heartBeatPitch.SetTarget( 1.0f );
    breathPitch.Set( 1.5f );
    breathPitch.SetTarget( 1.0f );
}

/*
========
Player::UpdateFright
========
*/
void Player::UpdateFright() {
    breathVolume.ChaseTarget( 0.075f );
    heartBeatVolume.ChaseTarget( 0.075f );
    heartBeatPitch.ChaseTarget( 0.0025f );
    breathPitch.ChaseTarget( 0.0025f );

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
Player::UpdateCameraBob
========
*/
void Player::UpdateCameraBob() {
    static int stepPlayed = 0;

    if( moved ) {
        cameraBobCoeff += 7.5 * runBobCoeff * g_dt;

        float xOffset = sinf( cameraBobCoeff ) * ( runBobCoeff * runBobCoeff ) * 0.075f;
        float yOffset = abs( xOffset );

        if( yOffset < 0.02 && !ruIsSoundPlaying( footsteps[ stepPlayed ] ) ) {
            stepPlayed = rand() % footsteps.size();

            ruPlaySound( footsteps[ stepPlayed ]);
        }

        cameraBob = ruVector3( xOffset, yOffset + headHeight, 0 );
    } else {
        cameraBobCoeff = 0;
    }
	if( stealthMode ) {
		stealthOffset.SetTarget( stealthOffset.GetMin() );
	} else {
		stealthOffset.SetTarget( stealthOffset.GetMax() );
	}
	stealthOffset.ChaseTarget( 0.15f );
    cameraOffset = cameraOffset.Lerp( cameraBob, 0.25f );
    ruSetNodePosition( camera->cameraNode, cameraOffset + ruVector3( 0.0f, stealthOffset, 0.0f ) );
}

/*
========
Player::DrawSheetInHands
========
*/
void Player::DrawSheetInHands() {
    if( sheetInHands ) {
        sheetInHands->Draw();

        pickedObjectDesc = sheetInHands->desc;

        pickedObjectDesc += localization.GetString( "sheetOpen" );

        if( ruIsMouseHit( MB_Right ) ||  ( ruGetNodePosition( sheetInHands->object) - ruGetNodePosition( body )).Length2() > 2 ) {
			CloseCurrentSheet();
        }
    }
}

/*
========
Player::DescribePickedObject
========
*/
void Player::DescribePickedObject() {
    // Change cursor first
    if( nearestPicked.IsValid() ) {
        if( IsObjectHasNormalMass( nearestPicked )) {
            ruDrawGUIRect( ruGetResolutionWidth() / 2 - 16, ruGetResolutionHeight() / 2 - 16, 32, 32, upCursor, ruVector3( 255, 255, 255 ), 180 );
        }
    } else {
        if( objectInHands.IsValid() ) {
            ruDrawGUIRect( ruGetResolutionWidth() / 2 - 16, ruGetResolutionHeight() / 2 - 16, 32, 32, downCursor, ruVector3( 255, 255, 255 ), 180 );
        } else {
            ruDrawGUIText( "+", ruGetResolutionWidth() / 2 - 16, ruGetResolutionHeight() / 2 - 16, 32, 32, gui->font, ruVector3( 255, 0, 0 ), 1, 180 );
        }
    }
    // Then describe object
    ruDrawGUIText( pickedObjectDesc.c_str(), ruGetResolutionWidth() / 2 - 256, ruGetResolutionHeight() - 200, 512, 128, gui->font, ruVector3( 255, 0, 0 ), 1 );
}

/*
========
Player::UpdateEnvironmentDamaging
========
*/
void Player::UpdateEnvironmentDamaging() {
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
void Player::UpdateItemsHandling() {
	if( nearestPicked.IsValid() ) {
		if( IsUseButtonHit() ) {
			Item * itm = Item::GetByObject( nearestPicked );

			if( itm ) {
				AddItem( itm );

				ruPlaySound( pickupSound );
			}

			Sheet * sheet = Sheet::GetSheetByObject( nearestPicked );

			if( sheetInHands ) {
				CloseCurrentSheet();
			} else {
				if( sheet ) {
					sheetInHands = sheet;
					ruHideNode( sheetInHands->object );
					ruPlaySound( Sheet::paperFlip );
				}
			}
		}
	}

    if( objectInHands.IsValid() ) {
        ruVector3 ppPos = ruGetNodePosition( itemPoint );
        ruVector3 objectPos = ruGetNodePosition( objectInHands );
        ruVector3 dir = ppPos - objectPos;
        if( ruIsMouseDown( MB_Left ) ) {
            ruMoveNode( objectInHands,  dir * 6 );

            ruSetNodeAngularVelocity( objectInHands, ruVector3( 0, 0, 0 ));

            if( ruIsMouseDown( MB_Right ) ) {
                if( UseStamina( ruGetNodeMass( objectInHands )  )) {
                    ruMoveNode( objectInHands, ( ppPos - ruGetNodePosition( camera->cameraNode )).Normalize() * 6 );
                }

                objectThrown = true;
                objectInHands.Invalidate();
            }
        } else {
            ruSetNodeAngularVelocity( objectInHands, ruVector3( 1, 1, 1 ));

            objectInHands.Invalidate();
        }
    }

    if( !ruIsMouseDown( MB_Left )) {
        objectThrown = false;
    }
}

/*
========
Player::UpdatePicking
========
*/
void Player::UpdatePicking() {
    ruVector3 pickPosition;

    picked = ruRayPick( ruGetResolutionWidth() / 2, ruGetResolutionHeight() / 2, &pickPosition );

    nearestPicked.Invalidate();

    if( picked.IsValid() && !objectInHands.IsValid() && !locked  ) {
        objectInHands.Invalidate();

        ruVector3 ppPos = ruGetNodePosition( pickPoint );
        ruVector3 dir = ppPos - pickPosition;

        Item * itm = Item::GetByObject( picked );
        Sheet * sheet = Sheet::GetSheetByObject( picked );

        if( dir.Length2() < 1.5f ) {
            nearestPicked = picked;

            if( itm ) {
                pickedObjectDesc = itm->name;
                pickedObjectDesc += Format( localization.GetString( "itemPick" ), GetKeyName( keyUse).c_str() );
            } else if( sheet ) {
                pickedObjectDesc = sheet->desc;
                pickedObjectDesc += Format( localization.GetString( "sheetPick" ), GetKeyName( keyUse ).c_str() );
            } else {
                if( IsObjectHasNormalMass( picked ) && !ruIsNodeFrozen( picked )) {
                    DrawTip( localization.GetString( "objectPick" ) );
                }
            }

            if( ruIsMouseDown( MB_Left ) ) {
                if( IsObjectHasNormalMass( picked )) {
					if( !ruIsNodeFrozen( picked ) && !objectThrown ) {
						objectInHands = picked;
					}
				}
            }
        }

        if( !itm && !sheet ) {
            pickedObjectDesc = " ";
        }
    }
}

/*
========
Player::CreateFlashLight
========
*/
void Player::CreateFlashLight() {
    flashlight = new Flashlight();

    flashlight->Attach( camera->cameraNode );

    flashLightItem = new Item( flashlight->model, Item::Flashlight );
    inventory.items.push_back( flashLightItem );
}

/*
========
Player::UpdateFlashLight
========
*/
void Player::UpdateFlashLight() {
    flashlight->Update();

    flashLightItem->content = flashlight->charge;

    if( ruIsKeyHit( keyFlashLight ) && !locked ) {
        flashlight->Switch();
    }
}

/*
========
Player::DrawGUIElements
========
*/
void Player::DrawGUIElements() {
    int alpha = placeDescTimer < 50 ? 255.0f * (float)placeDescTimer / 50.0f : 255;
    ruDrawGUIText( placeDesc.c_str(), ruGetResolutionWidth() - 300, ruGetResolutionHeight() - 200, 200, 200, gui->font, ruVector3( 255, 255, 255 ), 1, alpha );

    if( placeDescTimer ) {
        placeDescTimer--;
    }

    goal.AnimateAndRender();
}

/*
========
Player::FreeHands
========
*/
void Player::FreeHands() {
    objectInHands.Invalidate();
}

/*
========
Player::ChargeFlashLight
========
*/
void Player::ChargeFlashLight( Item * fuel ) {
    flashlight->Fuel();
}

/*
========
Player::~Player
========
*/
Player::~Player() {
    delete flashlight;
    delete camera;
    delete flashLightItem;
}

/*
========
Player::SetMetalFootsteps
========
*/
void Player::SetMetalFootsteps() {
    for( auto s : footsteps ) {
        ruFreeSound( s );
    }

    footsteps.clear();

    footsteps.push_back( ruLoadSound3D( "data/sounds/footsteps/FootStep_shoe_metal_step1.ogg" ) );
    footsteps.push_back( ruLoadSound3D( "data/sounds/footsteps/FootStep_shoe_metal_step2.ogg" ) );
    footsteps.push_back( ruLoadSound3D( "data/sounds/footsteps/FootStep_shoe_metal_step3.ogg" ) );
    footsteps.push_back( ruLoadSound3D( "data/sounds/footsteps/FootStep_shoe_metal_step4.ogg" ) );

    for( auto s : footsteps ) {
        ruAttachSound( s, body );
        ruSetSoundVolume( s, 0.55f );
    }

    footstepsType = FootstepsType::Metal;
}

/*
========
Player::SetFootsteps
========
*/
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

/*
========
Player::DrawTip
========
*/
void Player::DrawTip( string text ) {
    ruDrawGUIText( text.c_str(), ruGetResolutionWidth() / 2 - 256, ruGetResolutionHeight() - 200, 512, 128, gui->font, ruVector3( 255, 0, 0 ), 1 );
}

/*
========
Player::IsUseButtonHit
========
*/
bool Player::IsUseButtonHit() {
    return ruIsKeyHit( keyUse );
}

/*
========
Player::IsObjectHasNormalMass
========
*/
bool Player::IsObjectHasNormalMass( ruNodeHandle node ) {
    return ruGetNodeMass( node ) > 0 && ruGetNodeMass( node ) < 40;
}

/*
========
Player::DeserializeWith
========
*/
void Player::DeserializeWith( TextFileStream & in ) {
    ruSetNodeLocalPosition( body, in.ReadVector3() );

    in.ReadBoolean( locked );
    in.ReadBoolean( smoothCamera );
    in.ReadFloat( runBobCoeff );
    damagePitchOffset.Deserialize( in );

    footstepsType = (FootstepsType)in.ReadInteger();

    pitch.Deserialize( in );
    yaw.Deserialize( in );
    in.ReadVector3( speed );
    in.ReadVector3( speedTo );
    in.ReadVector3( gravity );
    in.ReadVector3( jumpTo );

    currentWay = Way::GetByObject( ruFindByName( in.Readstring().c_str() ));
    if( currentWay ) {
        ruFreeze( player->body );
    }

    in.ReadBoolean( landed );
    in.ReadFloat( stamina );
    in.ReadFloat( life );
    in.ReadFloat( maxLife );
    in.ReadFloat( maxStamina );
    in.ReadFloat( runSpeedMult );
    fov.Deserialize( in );
    in.ReadFloat( cameraBobCoeff );

    in.ReadVector3( cameraOffset );
    in.ReadVector3( cameraBob );

    in.ReadFloat( headHeight );

    in.ReadBoolean( objectThrown );
    in.ReadBoolean( dead );

    in.ReadVector3( frameColor );

    in.ReadBoolean( moved );

    in.ReadInteger( placeDescTimer );

    staminaAlpha.Deserialize( in );
    healthAlpha.Deserialize( in );

    in.ReadBoolean( moved );
    in.ReadBoolean( objectiveDone );

    breathVolume.Deserialize( in );
    heartBeatVolume.Deserialize( in );
    heartBeatPitch.Deserialize( in );
    breathPitch.Deserialize( in );

    sheetInHands = Sheet::GetByObject( ruFindByName( in.Readstring().c_str() ));

    in.ReadInteger( keyMoveForward );
    in.ReadInteger( keyMoveBackward );
    in.ReadInteger( keyStrafeLeft );
    in.ReadInteger( keyStrafeRight );
    in.ReadInteger( keyJump );
    in.ReadInteger( keyFlashLight );
    in.ReadInteger( keyRun );
    in.ReadInteger( keyInventory );
    in.ReadInteger( keyUse );

	stealthMode = in.ReadBoolean();

    flashlight->DeserializeWith( in );

    tip.Deserialize( in );
	//headAngle.Deserialize( in );

    camera->FadePercent( 100 );
    camera->SetFadeColor( ruVector3( 255, 255, 255 ) );
    ruSetNodeFriction( body, 0 );
}

/*
========
Player::SerializeWith
========
*/
void Player::SerializeWith( TextFileStream & out ) {    
    ruUnfreeze( body );
    out.WriteVector3( ruGetNodeLocalPosition( body ));
    ruSetAngularFactor( body, ruVector3( 0, 0, 0 ));

    out.WriteBoolean( locked );
    out.WriteBoolean( smoothCamera );
    out.WriteFloat( runBobCoeff );
    damagePitchOffset.Serialize( out );
    out.WriteInteger((int)footstepsType );
    pitch.Serialize( out );
    yaw.Serialize( out );
    out.WriteVector3( speed );
    out.WriteVector3( speedTo );
    out.WriteVector3( gravity );
    out.WriteVector3( jumpTo );

    out.Writestring( currentWay ? ruGetNodeName( currentWay->GetEnterZone()) : "undefinedWay" );

    out.WriteBoolean( landed );
    out.WriteFloat( stamina );
    out.WriteFloat( life );
    out.WriteFloat( maxLife );
    out.WriteFloat( maxStamina );
    out.WriteFloat( runSpeedMult );
    fov.Serialize( out );
    out.WriteFloat( cameraBobCoeff );

    out.WriteVector3( cameraOffset );
    out.WriteVector3( cameraBob );

    out.WriteFloat( headHeight );

    out.WriteBoolean( objectThrown );
    out.WriteBoolean( dead );

    out.WriteVector3( frameColor );

    out.WriteBoolean( moved );

    out.WriteInteger( placeDescTimer );

    staminaAlpha.Serialize( out );
    healthAlpha.Serialize( out );
    
    out.WriteBoolean( moved );
    out.WriteBoolean( objectiveDone );

    breathVolume.Serialize( out );
    heartBeatVolume.Serialize( out );
    heartBeatPitch.Serialize( out );
    breathPitch.Serialize( out );

    out.Writestring( sheetInHands ? ruGetNodeName( sheetInHands->object ) : "undefinedSheet" );

    out.WriteInteger( keyMoveForward );
    out.WriteInteger( keyMoveBackward );
    out.WriteInteger( keyStrafeLeft );
    out.WriteInteger( keyStrafeRight );
    out.WriteInteger( keyJump );
    out.WriteInteger( keyFlashLight );
    out.WriteInteger( keyRun );
    out.WriteInteger( keyInventory );
    out.WriteInteger( keyUse );

	out.WriteBoolean( stealthMode );

    flashlight->SerializeWith( out );

    tip.Serialize( out );
	//headAngle.Serialize( out );
}
