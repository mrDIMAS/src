#include "Player.h"
#include "Menu.h"
#include "Door.h"
#include "utils.h"

Player * player = 0;

Player::Player() {
    localization.ParseFile( localizationPath + "player.loc" );

    // Stamina vars
    maxStamina = 100.0f;
    stamina = maxStamina;

    fov = SmoothFloat( 75.0f, 75.0f, 90.0f );

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

    // State vars
    dead = false;
    landed = false;
    locked = false;

    sheetInHands = nullptr;

    // Camera bob vars
    headHeight = 0.4;
    cameraBob = Vector3( 0, headHeight, 0 );
    runBobCoeff = 1.0f;
    cameraBobCoeff = 0;

    // Effects vars
    frameColor = Vector3( 1.0f, 1.0f, 1.0f );

    // Control vars
    mouseSens = 0.5f;
    keyMoveForward = mi::W;
    keyMoveBackward = mi::S;
    keyStrafeLeft = mi::A;
    keyStrafeRight = mi::D;
    keyJump = mi::Space;
    keyFlashLight = mi::F;
    keyRun = mi::LeftShift;
    keyInventory = mi::Tab;
    keyUse = mi::E;

    // GUI vars
    staminaAlpha = SmoothFloat( 255.0, 0.0f, 255.0f );
    healthAlpha = SmoothFloat( 255.0, 0.0f, 255.0f );

    currentWay = nullptr;

    LoadGUIElements();
    CreateBody();
    CreateCamera();
    CreateFlashLight();
    LoadSounds();
    CompleteObjective();
    SetDirtFootsteps();
}

void Player::SetPlaceDescription( string desc ) {
    placeDesc = desc;
    placeDescTimer = 240;
}

void Player::DrawStatusBar() {
    if( moved ) {
        staminaAlpha.SetTarget( 255 );
        healthAlpha.SetTarget( 255 );
    } else {
        staminaAlpha.SetTarget( 50 );
        healthAlpha.SetTarget( 50 );
    }

    staminaAlpha.ChaseTarget( 0.15f );
    healthAlpha.ChaseTarget( 0.15f );

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

bool Player::CanJump( ) {
    for( int i = 0; i < GetContactCount( body ); i++ )
        if( GetContact( body, i ).normal.y > 0.5 ) {
            return true;
        }
    return false;
}

int Player::GotItemAnyOfType( int type ) {
    for( auto item : inventory.items )
        if( item->type == type ) {
            return true;
        }
    return false;
}

bool Player::UseStamina( float st ) {
    if( stamina - st < 0 ) {
        return false;
    }

    stamina -= st;

    return true;
}

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

void Player::UpdateInventory() {
    if( mi::KeyHit( (mi::Key)keyInventory ) && !locked ) {
        inventory.opened = !inventory.opened;
    }

    inventory.Update();
}

void Player::SetObjective( string text ) {
    string objectiveText = localization.GetString( "currentObjective" );
    objectiveText += text;

    goal.SetText( objectiveText );

    objectiveDone = false;
}

void Player::CompleteObjective() {
    SetObjective( localization.GetString( "objectiveUnknown" ));

    objectiveDone = true;
}

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
        pitch.SetTarget( pitch.GetTarget() + mi::MouseYSpeed() * mouseSpeed );
        yaw.SetTarget( yaw.GetTarget() - mi::MouseXSpeed() * mouseSpeed );
    }
    
    damagePitchOffset.ChaseTarget( 0.65f );

    if( damagePitchOffset >= 19.0f ) {
        damagePitchOffset.SetTarget( 0.0f );
    }

    if( !locked ) {
        if( smoothCamera ) {
            pitch.ChaseTarget( 0.35f );
            yaw.ChaseTarget( 0.35f );
        } else {
            yaw = yaw.GetTarget();
            pitch = pitch.GetTarget();
        }
    }

    SetRotation( camera->cameraNode, Quaternion( Vector3( 1, 0, 0 ), pitch + damagePitchOffset ) );
    SetRotation( body, Quaternion( Vector3( 0, 1, 0 ), yaw ) );
}

void Player::UpdateMoving() {
    static int stepPlayed = 0;

    for( auto cw : Way::all ) {
        if( cw->IsEnterPicked() ) {
            if( !cw->IsPlayerInside() ) {
                DrawTip( Format( localization.GetString( "crawlIn" ), GetKeyName( keyUse )));
            }

            if( IsUseButtonHit() ) {
                cw->Enter();
            }
        }
    }

    for( auto door : Door::all ) {
        door->DoInteraction();

        if( door->IsPickedByPlayer() ) {
            DrawTip( Format( localization.GetString( "openClose" ), GetKeyName( keyUse )));

            if( IsUseButtonHit() ) {
                door->SwitchState();
            }
        }
    }

    if( currentWay ) {
        currentWay->DoEntering();

        if( !currentWay->IsFreeLook() ) {
            currentWay->LookAtTarget();
        }

        if( currentWay->IsPlayerInside() ) {
            bool move = false;

            if( mi::KeyDown( (mi::Key)keyMoveForward )) {
                currentWay->SetDirection( Way::Direction::Forward );
                move = true;
            }
            if( mi::KeyDown( (mi::Key)keyMoveBackward )) {
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
            if( mi::KeyDown( (mi::Key)keyMoveForward )) {
                speedTo = speedTo + look;
            }
            if( mi::KeyDown( (mi::Key)keyMoveBackward )) {
                speedTo = speedTo - look;
            }
            if( mi::KeyDown( (mi::Key)keyStrafeLeft )) {
                speedTo = speedTo + right;
            }
            if( mi::KeyDown( (mi::Key)keyStrafeRight )) {
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

        if( mi::KeyDown( (mi::Key)keyRun ) && moved ) {
            if( stamina > 0 ) {
                speedTo = speedTo * runSpeedMult;
                stamina -= 0.15f;
                fov.SetTarget( fov.GetMax() );
                runBobCoeff = 1.425f;
            }
        } else {
            if( stamina < maxStamina ) {
                stamina += 0.35f;
            }
        }

        fov.ChaseTarget( 0.07f );

        SetFOV( camera->cameraNode, fov );
        speed = speed.Lerp( speedTo, 0.25f ) + gravity;
        Move( body, speed * Vector3( 2.5, 1, 2.5 ) );
    }

    UpdateCameraBob();
}

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
    UpdatePicking();
    UpdateItemsHandling();
    UpdateEnvironmentDamaging();
    UpdateInventory();
    DrawSheetInHands();
    DescribePickedObject();
}

void Player::LoadGUIElements() {
    upCursor = GetTexture( "data/gui/up.png" );
    downCursor = GetTexture( "data/gui/down.png" );
    pickupSound = CreateSound2D( "data/sounds/menuhit.ogg" );
    statusBar = GetTexture( "data/gui/statusbar.png" );
}


void Player::CreateCamera() {
    camera = new GameCamera( fov );
    Attach( camera->cameraNode, body );
    SetSkybox( camera->cameraNode, "data/textures/skyboxes/night4/nnksky01");

    // Pick
    pickPoint = CreateSceneNode();
    Attach( pickPoint, camera->cameraNode );
    SetPosition( pickPoint, Vector3( 0, 0, 0.1 ));

    itemPoint = CreateSceneNode();
    Attach( itemPoint, camera->cameraNode );
    SetPosition( itemPoint, Vector3( 0, 0, 1.0f ));
}

void Player::CreateBody() {
    body = CreateSceneNode();
    SetName( body, "Player" );
    SetCapsuleBody( body, 0.95, 0.28 );
    SetAngularFactor( body, Vector3( 0, 0, 0 ));
    SetPosition( body, Vector3( 0, 100, 0 ));
    SetFriction( body, 0 );
    SetAnisotropicFriction( body, Vector3( 1, 1, 1 ));
    SetDamping( body, 0, 0 );
    SetMass( body, 2 );
    SetGravity( body, Vector3( 0, 0, 0 ));
}

void Player::SetRockFootsteps() {
    for( auto s : footsteps ) {
        FreeSoundSource( s );
    }

    footsteps.clear();

    footsteps.push_back( CreateSound3D( "data/sounds/step1.ogg" ) );
    footsteps.push_back( CreateSound3D( "data/sounds/step2.ogg" ) );
    footsteps.push_back( CreateSound3D( "data/sounds/step3.ogg" ) );
    footsteps.push_back( CreateSound3D( "data/sounds/step4.ogg" ) );

    for( auto s : footsteps ) {
        AttachSound( s, body );
        SetVolume( s, 0.35f );
    }

    footstepsType = FootstepsType::Rock;
}

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

void Player::UpdateFright() {
    breathVolume.ChaseTarget( 0.075f );
    heartBeatVolume.ChaseTarget( 0.075f );
    heartBeatPitch.ChaseTarget( 0.0025f );
    breathPitch.ChaseTarget( 0.0025f );

    SetVolume( breathSound, breathVolume );
    SetVolume( heartBeatSound, heartBeatVolume );

    PlaySoundSource( heartBeatSound, true );
    PlaySoundSource( breathSound, true );

    SetPitch( breathSound, breathPitch );
    SetPitch( heartBeatSound, heartBeatPitch );
}

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

    cameraOffset = cameraOffset.Lerp( cameraBob, 0.25f );
    SetPosition( camera->cameraNode, cameraOffset );
}

void Player::UpdateJumping() {
    if( mi::KeyHit( (mi::Key)keyJump ) && !locked ) {
        if( CanJump() ) {
            jumpTo = Vector3( 0, 1.8, 0 );
            landed = false;
        }
    }

    gravity = gravity.Lerp( jumpTo, 0.75f );

    if( gravity.y >= jumpTo.y ) {
        landed = true;
    }

    if( landed ) {
        jumpTo = Vector3( 0, -2.0, 0 );

        if( CanJump() ) {
            jumpTo = Vector3( 0, 0, 0 );
        }
    };
}

void Player::DrawSheetInHands() {
    if( sheetInHands ) {
        sheetInHands->Draw();

        pickedObjectDesc = sheetInHands->desc;

        pickedObjectDesc += localization.GetString( "sheetOpen" );

        if( mi::MouseHit( mi::Right )) {
            ShowNode( sheetInHands->object );
            sheetInHands = 0;
            PlaySoundSource( Sheet::paperFlip );
        }
    }
}

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

void Player::UpdateEnvironmentDamaging() {
    for( int i = 0; i < GetContactCount( body ); i++ ) {
        Contact contact = GetContact( body, i );

        if( contact.impulse > 30 ) {
            Damage( contact.impulse / 5 );
        }
    }
}

void Player::UpdateItemsHandling() {
    if( objectInHands.IsValid() ) {
        Vector3 ppPos = GetPosition( itemPoint );
        Vector3 objectPos = GetPosition( objectInHands );
        Vector3 dir = ppPos - objectPos;
        if( mi::MouseDown( mi::Left ) ) {
            Move( objectInHands,  dir * 6 );

            SetAngularVelocity( objectInHands, Vector3( 0, 0, 0 ));

            if( mi::MouseDown( mi::Right ) ) {
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

        if( objectInHands.IsValid() ) {
            if( IsUseButtonHit() ) {
                Item * itm = Item::GetByObject( objectInHands );

                if( itm ) {
                    AddItem( itm );

                    PlaySoundSource( pickupSound );
                }

                Sheet * sheet = Sheet::GetSheetByObject( objectInHands );

                if( sheet ) {
                    sheetInHands = sheet;
                    HideNode( sheetInHands->object );
                    PlaySoundSource( Sheet::paperFlip );
                }

                objectInHands.Invalidate();
            }
        }
    }

    if( !mi::MouseDown( mi::Left )) {
        objectThrown = false;
    }
}

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
                pickedObjectDesc += Format( localization.GetString( "itemPick" ), GetKeyName( keyUse));
            } else if( sheet ) {
                pickedObjectDesc = sheet->desc;
                pickedObjectDesc += Format( localization.GetString( "sheetPick" ), GetKeyName( keyUse ));
            } else {
                if( IsObjectHasNormalMass( picked ) && !IsNodeFrozen( picked )) {
                    DrawTip( localization.GetString( "objectPick" ) );
                }
            }

            if( mi::MouseDown( mi::Left ) ) {
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

void Player::CreateFlashLight() {
    flashlight = new Flashlight();

    flashlight->Attach( camera->cameraNode );

    flashLightItem = new Item( flashlight->model, Item::Flashlight );
    inventory.items.push_back( flashLightItem );
}


void Player::UpdateFlashLight() {
    flashlight->Update();

    flashLightItem->content = flashlight->charge;

    if( mi::KeyHit( (mi::Key)keyFlashLight ) && !locked ) {
        flashlight->Switch();
    }
}

void Player::DrawGUIElements() {
    int alpha = placeDescTimer < 50 ? 255.0f * (float)placeDescTimer / 50.0f : 255;
    DrawGUIText( placeDesc.c_str(), GetResolutionWidth() - 300, GetResolutionHeight() - 200, 200, 200, gui->font, Vector3( 255, 255, 255 ), 1, alpha );

    if( placeDescTimer ) {
        placeDescTimer--;
    }

    goal.AnimateAndRender();
}

void Player::FreeHands() {
    objectInHands.Invalidate();
}

void Player::ChargeFlashLight( Item * fuel ) {
    flashlight->Fuel();
}

Player::~Player() {
    delete flashlight;
    delete camera;
    delete flashLightItem;
}

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

void Player::DrawTip( string text ) {
    DrawGUIText( text.c_str(), GetResolutionWidth() / 2 - 256, GetResolutionHeight() - 200, 512, 128, gui->font, Vector3( 255, 0, 0 ), 1 );
}

bool Player::IsUseButtonHit() {
    return mi::KeyHit( (mi::Key)keyUse );
}

bool Player::IsObjectHasNormalMass( NodeHandle node ) {
    return GetMass( node ) > 0 && GetMass( node ) < 40;
}

void Player::DeserializeWith( TextFileStream & in ) {
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

    flashlight->DeserializeWith( in );

    tip.Deserialize( in );
}

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

    flashlight->SerializeWith( out );

    tip.Serialize( out );
}
