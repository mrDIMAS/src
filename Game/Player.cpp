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
Player::Player() {
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

    cameraBob = Vector3( 0, headHeight, 0 );
    runBobCoeff = 1.0f;
    cameraBobCoeff = 0;

    // Effects vars
    frameColor = Vector3( 1.0f, 1.0f, 1.0f );

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

	stealthSign = GetTexture( "data/textures/effects/eye.png" );

    LoadGUIElements();
    CreateBody();
    CreateCamera();
    CreateFlashLight();
    LoadSounds();
    CompleteObjective();
    SetDirtFootsteps();
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

    DrawGUIRect( 0, GetResolutionHeight() - h, w, h, statusBar, Vector3( 255, 255, 255 ), staminaAlpha );

    int segCount = stamina / 5;
    for( int i = 0; i < segCount; i++ ) {
        DrawGUIRect( 44 + i * ( 8 + 2 ), GetResolutionHeight() - 3 * 15, 8, 16, gui->staminaBarImg, Vector3( 255, 255, 255 ), staminaAlpha );
    }

    segCount = life / 5;
    for( int i = 0; i < segCount; i++ ) {
        DrawGUIRect( 44 + i * ( 8 + 2 ), GetResolutionHeight() - 4 * 26, 8, 16, gui->lifeBarImg, Vector3( 255, 255, 255 ), healthAlpha );
    }
}

/*
========
Player::CanJump
========
*/
bool Player::CanJump( ) {
	NodeHandle legBump = RayTest( GetPosition( body ) + Vector3( 0, 0.1, 0 ), GetPosition( body ) - Vector3( 0, capsuleHeight, 0 ), 0 );
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
            SetAngularFactor( body, Vector3( 1, 1, 1 ));
            SetFriction( body, 1 );
            SetAnisotropicFriction( body, Vector3( 1, 1, 1 ));
            Move( body, Vector3( 1,1,1));
        }

        dead = true;
        life = 0;

        flashlight->SwitchOff();

        camera->FadePercent( 5 );
        camera->SetFadeColor( Vector3( 70, 0, 0 ) );
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

    Freeze( itm->object );
    Detach( itm->object );
    itm->inInventory = true;

    SetPosition( itm->object, Vector3( 10000, 10000, 10000 )); // far far away

    inventory.items.push_back( itm );
}

/*
========
Player::UpdateInventory
========
*/
void Player::UpdateInventory() {
    if( IsKeyHit( keyInventory ) && !locked ) {
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
        pitch.SetTarget( pitch.GetTarget() + GetMouseYSpeed() * mouseSpeed );
        yaw.SetTarget( yaw.GetTarget() - GetMouseXSpeed() * mouseSpeed );
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
	SetRotation( camera->cameraNode, Quaternion( Vector3( 1, 0, 0 ), pitch ) );
    SetRotation( body, Quaternion( Vector3( 0, 1, 0 ), yaw ) );

	headAngle.SetTarget( 0.0f );
	if( IsKeyDown( keyLookLeft )) {
		headAngle.SetTarget( headAngle.GetMin() );
	}
	if( IsKeyDown( keyLookRight )) {
		headAngle.SetTarget( headAngle.GetMax() );
	}
	headAngle.ChaseTarget( 17.0f * g_dt );
	SetRotation( head, Quaternion( Vector3( 0, 0, 1 ), headAngle ));
}

/*
========
Player::UpdateJumping
========
*/
void Player::UpdateJumping() {	
	// do ray test, to determine collision with objects above camera
	NodeHandle headBumpObject = RayTest( GetPosition( body ) + Vector3( 0, capsuleHeight * 0.98, 0 ), GetPosition( body ) + Vector3( 0, 1.02 * capsuleHeight, 0 ), nullptr );
	

    if( IsKeyHit( keyJump ) && !locked ) {
       if( CanJump() ) {
            jumpTo = Vector3( 0, 150, 0 );
            landed = false;
       }
    }

    gravity = gravity.Lerp( jumpTo, 40.0f * g_dt );

    if( gravity.y >= jumpTo.y ) {
        landed = true;
    }

    if( landed || headBumpObject.IsValid() ) {
        jumpTo = Vector3( 0, -150, 0 );
        if( CanJump() ) {
            jumpTo = Vector3( 0, 0, 0 );
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

	if( IsKeyHit( keyStealth )) {
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

            if( IsKeyDown( keyMoveForward )) {
                currentWay->SetDirection( Way::Direction::Forward );
                move = true;
            }
            if( IsKeyDown( keyMoveBackward )) {
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
                Move( player->body, Vector3( 0, 0, 0 ));

                moved = false;
            }
        }
    } else {
        Vector3 look = GetLookVector( body );
        Vector3 right = GetRightVector( body );

        speedTo = Vector3( 0, 0, 0 );

        if( !locked ) {
            if( IsKeyDown( keyMoveForward )) {
                speedTo = speedTo + look;
            }
            if( IsKeyDown( keyMoveBackward )) {
                speedTo = speedTo - look;
            }
            if( IsKeyDown( keyStrafeLeft )) {
                speedTo = speedTo + right;
            }
            if( IsKeyDown( keyStrafeRight )) {
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
        if( IsKeyDown( keyRun ) && moved ) {
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
        SetFOV( camera->cameraNode, fov );

        speed = speed.Lerp( speedTo + gravity, 10.0f * g_dt );
        Move( body, speed * Vector3( 100, 1, 100 ) * g_dt );		
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
	Vector3 pos = GetPosition( body );
	NodeHandle affectLight;
	for( int i = 0; i < GetWorldPointLightCount(); i++ ) {
		if( IsLightViewPoint( GetWorldPointLight( i ), pos )) {
			inLight = true;
			affectLight = GetWorldPointLight( i );
			break;
		}
	}
	if( !inLight ) {
		for( int i = 0; i < GetWorldSpotLightCount(); i++ ) {
			if( IsLightViewPoint( GetWorldSpotLight( i ), pos )) {
				inLight = true;
				affectLight = GetWorldSpotLight( i );
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
			SetVolume( snd, 0.15f );
		} else {
			SetVolume( snd, 0.4f );
		}
	}

	int alpha = ( 255 * ( ( stealthFactor > 1.05f ) ? 1.0f : ( stealthFactor + 0.05f ) ) );
	if (alpha > 255 )
		alpha = 255;
	Vector3 color = ( stealthFactor < 1.05f ) ? Vector3( 255, 255, 255 ) : Vector3( 255, 0, 0 );
	DrawGUIRect( GetResolutionWidth() / 2 - 32, 200, 64, 32, stealthSign, color, alpha );
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
    upCursor = GetTexture( "data/gui/up.png" );
    downCursor = GetTexture( "data/gui/down.png" );
    pickupSound = CreateSound2D( "data/sounds/menuhit.ogg" );
    statusBar = GetTexture( "data/gui/statusbar.png" );
}

/*
========
Player::CreateCamera
========
*/
void Player::CreateCamera() {
	headHeight = 2.4;

	head = CreateSceneNode();
	Attach( head, body );
	SetPosition( head, Vector3( 0, -2.0f, 0.0f ));
    camera = new GameCamera( fov );
    Attach( camera->cameraNode, head );
	cameraOffset = Vector3( 0, headHeight, 0 );
    cameraBob = Vector3( 0, headHeight, 0 );

    // Pick
    pickPoint = CreateSceneNode();
    Attach( pickPoint, camera->cameraNode );
    SetPosition( pickPoint, Vector3( 0, 0, 0.1 ));

    itemPoint = CreateSceneNode();
    Attach( itemPoint, camera->cameraNode );
    SetPosition( itemPoint, Vector3( 0, 0, 1.0f ));
}

/*
========
Player::CreateBody
========
*/
void Player::CreateBody() {
	capsuleRadius = 0.28f;
	capsuleHeight = 0.95f;

    body = CreateSceneNode();
    SetName( body, "Player" );
    SetCapsuleBody( body, capsuleHeight, capsuleRadius );
    SetupBody();
}

/*
========
Player::SetRockFootsteps
========
*/
void Player::SetRockFootsteps() {
    for( auto s : footsteps ) {
        FreeSoundSource( s );
    }

    footsteps.clear();

    footsteps.push_back( CreateSound3D( "data/sounds/stonestep1.ogg" ) );
    footsteps.push_back( CreateSound3D( "data/sounds/stonestep2.ogg" ) );
    footsteps.push_back( CreateSound3D( "data/sounds/stonestep3.ogg" ) );
    footsteps.push_back( CreateSound3D( "data/sounds/stonestep4.ogg" ) );
	footsteps.push_back( CreateSound3D( "data/sounds/stonestep5.ogg" ) );
	footsteps.push_back( CreateSound3D( "data/sounds/stonestep6.ogg" ) );
	footsteps.push_back( CreateSound3D( "data/sounds/stonestep7.ogg" ) );
	footsteps.push_back( CreateSound3D( "data/sounds/stonestep8.ogg" ) );
	footsteps.push_back( CreateSound3D( "data/sounds/stonestep9.ogg" ) );
	footsteps.push_back( CreateSound3D( "data/sounds/stonestep10.ogg" ) );
	footsteps.push_back( CreateSound3D( "data/sounds/stonestep11.ogg" ) );
	footsteps.push_back( CreateSound3D( "data/sounds/stonestep12.ogg" ) );

    for( auto s : footsteps ) {
        AttachSound( s, body );
        SetVolume( s, 0.45f );
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
        FreeSoundSource( s );
    }

    footsteps.clear();

    footsteps.push_back( CreateSound3D( "data/sounds/dirt1.ogg" ) );
    footsteps.push_back( CreateSound3D( "data/sounds/dirt2.ogg" ) );
    footsteps.push_back( CreateSound3D( "data/sounds/dirt3.ogg" ) );
    footsteps.push_back( CreateSound3D( "data/sounds/dirt4.ogg" ) );

    for( auto s : footsteps ) {
        AttachSound( s, body );
        SetVolume( s, 0.45f );
    }

    footstepsType = FootstepsType::Dirt;
}

/*
========
Player::LoadSounds
========
*/
void Player::LoadSounds() {
    lighterCloseSound = CreateSound3D( "data/sounds/lighter_close.ogg" );
    lighterOpenSound = CreateSound3D( "data/sounds/lighter_open.ogg" );

    AttachSound( lighterCloseSound, camera->cameraNode );
    AttachSound( lighterOpenSound, camera->cameraNode );

    heartBeatSound = CreateSound2D( "data/sounds/heart.ogg" );
    breathSound = CreateSound2D( "data/sounds/breath.ogg" );

    SetSoundReferenceDistance( heartBeatSound, 100.0f );
    SetSoundReferenceDistance( breathSound, 100.0f );

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
        cameraBobCoeff += 0.115 * runBobCoeff;

        float xOffset = sinf( cameraBobCoeff ) * ( runBobCoeff * runBobCoeff ) * 0.075f;
        float yOffset = abs( xOffset );

        if( yOffset < 0.02 && !SoundPlaying( footsteps[ stepPlayed ] ) ) {
            stepPlayed = rand() % footsteps.size();

            PlaySoundSource( footsteps[ stepPlayed ]);
        }

        cameraBob = Vector3( xOffset, yOffset + headHeight, 0 );
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
    SetPosition( camera->cameraNode, cameraOffset + Vector3( 0.0f, stealthOffset, 0.0f ) );
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

        if( IsMouseHit( MB_Right )) {
            ShowNode( sheetInHands->object );
            sheetInHands = 0;
            PlaySoundSource( Sheet::paperFlip );
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
            DrawGUIRect( GetResolutionWidth() / 2 - 16, GetResolutionHeight() / 2 - 16, 32, 32, upCursor, Vector3( 255, 255, 255 ), 180 );
        }
    } else {
        if( objectInHands.IsValid() ) {
            DrawGUIRect( GetResolutionWidth() / 2 - 16, GetResolutionHeight() / 2 - 16, 32, 32, downCursor, Vector3( 255, 255, 255 ), 180 );
        } else {
            DrawGUIText( "+", GetResolutionWidth() / 2 - 16, GetResolutionHeight() / 2 - 16, 32, 32, gui->font, Vector3( 255, 0, 0 ), 1, 180 );
        }
    }
    // Then describe object
    DrawGUIText( pickedObjectDesc.c_str(), GetResolutionWidth() / 2 - 256, GetResolutionHeight() - 200, 512, 128, gui->font, Vector3( 255, 0, 0 ), 1 );
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

				PlaySoundSource( pickupSound );
			}

			Sheet * sheet = Sheet::GetSheetByObject( nearestPicked );

			if( sheet ) {
				sheetInHands = sheet;
				HideNode( sheetInHands->object );
				PlaySoundSource( Sheet::paperFlip );
			}
		}
	}

    if( objectInHands.IsValid() ) {
        Vector3 ppPos = GetPosition( itemPoint );
        Vector3 objectPos = GetPosition( objectInHands );
        Vector3 dir = ppPos - objectPos;
        if( IsMouseDown( MB_Left ) ) {
            Move( objectInHands,  dir * 6 );

            SetAngularVelocity( objectInHands, Vector3( 0, 0, 0 ));

            if( IsMouseDown( MB_Right ) ) {
                if( UseStamina( GetMass( objectInHands )  )) {
                    Move( objectInHands, ( ppPos - GetPosition( camera->cameraNode )).Normalize() * 6 );
                }

                objectThrown = true;
                objectInHands.Invalidate();
            }
        } else {
            SetAngularVelocity( objectInHands, Vector3( 1, 1, 1 ));

            objectInHands.Invalidate();
        }
    }

    if( !IsMouseDown( MB_Left )) {
        objectThrown = false;
    }
}

/*
========
Player::UpdatePicking
========
*/
void Player::UpdatePicking() {
    Vector3 pickPosition;

    picked = RayPick( GetResolutionWidth() / 2, GetResolutionHeight() / 2, &pickPosition );

    nearestPicked.Invalidate();

    if( picked.IsValid() && !objectInHands.IsValid() && !locked  ) {
        objectInHands.Invalidate();

        Vector3 ppPos = GetPosition( pickPoint );
        Vector3 dir = ppPos - pickPosition;

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
                if( IsObjectHasNormalMass( picked ) && !IsNodeFrozen( picked )) {
                    DrawTip( localization.GetString( "objectPick" ) );
                }
            }

            if( IsMouseDown( MB_Left ) ) {
                if( IsObjectHasNormalMass( picked )) {
                    if( !IsNodeFrozen( picked ) && !objectThrown ) {
                        objectInHands = picked;
                    }
                } else {
                    DrawTip( localization.GetString( "tooHeavy" ) );
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

    if( IsKeyHit( keyFlashLight ) && !locked ) {
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
    DrawGUIText( placeDesc.c_str(), GetResolutionWidth() - 300, GetResolutionHeight() - 200, 200, 200, gui->font, Vector3( 255, 255, 255 ), 1, alpha );

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
        FreeSoundSource( s );
    }

    footsteps.clear();

    footsteps.push_back( CreateSound3D( "data/sounds/footsteps/FootStep_shoe_metal_step1.ogg" ) );
    footsteps.push_back( CreateSound3D( "data/sounds/footsteps/FootStep_shoe_metal_step2.ogg" ) );
    footsteps.push_back( CreateSound3D( "data/sounds/footsteps/FootStep_shoe_metal_step3.ogg" ) );
    footsteps.push_back( CreateSound3D( "data/sounds/footsteps/FootStep_shoe_metal_step4.ogg" ) );

    for( auto s : footsteps ) {
        AttachSound( s, body );
        SetVolume( s, 0.55f );
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
    DrawGUIText( text.c_str(), GetResolutionWidth() / 2 - 256, GetResolutionHeight() - 200, 512, 128, gui->font, Vector3( 255, 0, 0 ), 1 );
}

/*
========
Player::IsUseButtonHit
========
*/
bool Player::IsUseButtonHit() {
    return IsKeyHit( keyUse );
}

/*
========
Player::IsObjectHasNormalMass
========
*/
bool Player::IsObjectHasNormalMass( NodeHandle node ) {
    return GetMass( node ) > 0 && GetMass( node ) < 40;
}

/*
========
Player::DeserializeWith
========
*/
void Player::DeserializeWith( TextFileStream & in ) {
	SetupBody();
    SetLocalPosition( body, in.ReadVector3() );

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

    currentWay = Way::GetByObject( FindByName( in.Readstring().c_str() ));
    if( currentWay ) {
        Freeze( player->body );
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

    sheetInHands = Sheet::GetByObject( FindByName( in.Readstring().c_str() ));

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
    camera->SetFadeColor( Vector3( 255, 255, 255 ) );
    SetFriction( body, 0 );
}

/*
========
Player::SerializeWith
========
*/
void Player::SerializeWith( TextFileStream & out ) {    
    Unfreeze( body );
    out.WriteVector3( GetLocalPosition( body ));
    SetAngularFactor( body, Vector3( 0, 0, 0 ));

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

    out.Writestring( currentWay ? GetName( currentWay->GetEnterZone()) : "undefinedWay" );

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

    out.Writestring( sheetInHands ? GetName( sheetInHands->object ) : "undefinedSheet" );

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

void Player::SetupBody()
{
	SetAngularFactor( body, Vector3( 0, 0, 0 ));
	SetFriction( body, 0 );
	SetAnisotropicFriction( body, Vector3( 1, 1, 1 ));
	SetDamping( body, 0, 0 );
	SetMass( body, 2 );
	SetGravity( body, Vector3( 0, 0, 0 ));
	//SetLinearFactor( body, Vector3( 0, 0, 0 ));
}
