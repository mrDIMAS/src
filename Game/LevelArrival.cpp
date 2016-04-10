#include "Precompiled.h"

#include "LevelArrival.h"
#include "Player.h"
#include "LevelMine.h"


LevelArrival::LevelArrival( ) : mChangeLevel( false ) {
    mTypeNum = 2;

    // Load localization
    LoadLocalization( "arrival.loc" );

    // Load main scene
    LoadSceneFromFile( "data/maps/release/arrival/arrival.scene" );

    //////////////////////////////////////////////////////////////////////////
    // Find and create all sheets
    AddSheet( new Sheet( GetUniqueObject( "Note1" ), mLocalization.GetString( "note1Desc" ), mLocalization.GetString( "note1" )));

    //////////////////////////////////////////////////////////////////////////
    // Find zones
	mLiftStopZone = GetUniqueObject( "LiftStopZone" );

    //////////////////////////////////////////////////////////////////////////
    // Find entities

	mLiftLamp = std::dynamic_pointer_cast<ruLight>( GetUniqueObject( "LiftLamp" ));
	mLiftLamp->Hide();

	mTutorialZone1 = GetUniqueObject( "TutorialZone1" );
	mTutorialZone2 = GetUniqueObject( "TutorialZone2" );
	mTutorialZone3 = GetUniqueObject( "TutorialZone3" );
	mTutorialZone4 = GetUniqueObject( "TutorialZone4" );
	mTutorialZone5 = GetUniqueObject( "TutorialZone5" );
	mTutorialZone6 = GetUniqueObject( "TutorialZone6" );

	mHalt = GetUniqueObject( "Halt" );

    //////////////////////////////////////////////////////////////////////////
    // Player noticements
    pPlayer->SetObjective( mLocalization.GetString( "objective1" ));

    pPlayer->SetPosition( GetUniqueObject("PlayerPosition")->GetPosition() + ruVector3( 0, 1, 0 ));

    //////////////////////////////////////////////////////////////////////////
    // Load sounds
	
	AddSound( mPowerDownSound = ruSound::Load2D( "data/sounds/powerdown.ogg"));
	mPowerDownSound->SetVolume( 0.7 );

	AddSound( mMetalWhineSound = ruSound::Load2D( "data/sounds/metal_whining.ogg" ));
	mMetalWhineSound->SetVolume( 0.95 );

	AddSound( mMetalStressSound = ruSound::Load2D( "data/sounds/metal_stress1.ogg" ));
	AddSound( mLiftFallSound = ruSound::Load2D( "data/sounds/lift_fall.ogg" ));
	mLiftFallSound->SetVolume( 1.25f );

    AddSound( mWindSound = ruSound::Load2D( "data/sounds/wind.ogg"));
    mWindSound->SetVolume( 0.5f );
    mWindSound->Play();

	mLift = make_shared<Lift>( GetUniqueObject( "Lift1" ) );
	mLift->SetControlPanel( GetUniqueObject( "Lift1Screen" ));
	mLift->SetDestinationPoint( GetUniqueObject( "Lift1Dest" ));
	mLift->SetSourcePoint( GetUniqueObject( "Lift1Source" ));
	mLift->SetMotorSound( ruSound::Load3D( "data/sounds/motor_idle.ogg"));
	mLift->SetBackDoors( GetUniqueObject( "Lift1BackDoorLeft"), GetUniqueObject( "Lift1BackDoorRight" ));
	mLift->SetFrontDoors( GetUniqueObject( "Lift1FrontDoorLeft"), GetUniqueObject( "Lift1FrontDoorRight" ));
	AddLift( mLift );

    ruSound::SetAudioReverb( 15 );

    AddAmbientSound( ruSound::Load3D( "data/sounds/ambient/forest/forestambient1.ogg" ));
    AddAmbientSound( ruSound::Load3D( "data/sounds/ambient/forest/forestambient2.ogg" ));
    AddAmbientSound( ruSound::Load3D( "data/sounds/ambient/forest/forestambient3.ogg" ));
    AddAmbientSound( ruSound::Load3D( "data/sounds/ambient/forest/forestambient4.ogg" ));
    AddAmbientSound( ruSound::Load3D( "data/sounds/ambient/forest/forestambient5.ogg" ));
    AddAmbientSound( ruSound::Load3D( "data/sounds/ambient/forest/forestambient6.ogg" ));

    pPlayer->mpCamera->mCamera->SetSkybox(
		ruTexture::Request( "data/textures/skyboxes/night3/nightsky_u.jpg" ),
		ruTexture::Request( "data/textures/skyboxes/night3/nightsky_l.jpg" ),
		ruTexture::Request( "data/textures/skyboxes/night3/nightsky_r.jpg" ),
		ruTexture::Request( "data/textures/skyboxes/night3/nightsky_f.jpg" ), 
		ruTexture::Request( "data/textures/skyboxes/night3/nightsky_b.jpg" )
	);

	mStages[ "LiftCrashed" ] = false;

	// every action series disabled by default
	mLiftCrashSeries.AddAction( 0.0f, ruDelegate::Bind( this, &LevelArrival::ActLiftCrash_PowerDown ) );
	mLiftCrashSeries.AddAction( mPowerDownSound->GetLength(), ruDelegate::Bind( this, &LevelArrival::ActLiftCrash_AfterPowerDown ) );
	mLiftCrashSeries.AddAction( mMetalStressSound->GetLength(), ruDelegate::Bind( this, &LevelArrival::ActLiftCrash_AfterFirstStressSound ) );
	mLiftCrashSeries.AddAction( mLiftFallSound->GetLength(), ruDelegate::Bind( this, &LevelArrival::ActLiftCrash_AfterFalldown ) );

    DoneInitialization();
}

LevelArrival::~LevelArrival() {

}

void LevelArrival::Show() {
    Level::Show();
}

void LevelArrival::Hide() {
    Level::Hide();
}

void LevelArrival::DoScenario() {
    if( Level::msCurLevelID != LevelName::L1Arrival ) {
        return;
    }

	mLiftCrashSeries.Perform();


	mLift->Update();

	if( pPlayer->IsInsideZone( mTutorialZone1 )) {
		pPlayer->SetActionText( mLocalization.GetString( "tutorialControls" ));
	}
	if( pPlayer->IsInsideZone( mTutorialZone2 )) {
		pPlayer->SetActionText( mLocalization.GetString( "tutorialControls2" ));
	}
	if( pPlayer->IsInsideZone( mTutorialZone3 )) {
		pPlayer->SetActionText( mLocalization.GetString( "tutorialControls3" ));
	}
	if( pPlayer->IsInsideZone( mTutorialZone4 )) {
		pPlayer->SetActionText( mLocalization.GetString( "tutorialControls4" ));
	}
	if( pPlayer->IsInsideZone( mTutorialZone5 )) {
		pPlayer->SetActionText( mLocalization.GetString( "tutorialControls5" ));
	}
	if( pPlayer->IsInsideZone( mTutorialZone6 )) {
		pPlayer->SetActionText( mLocalization.GetString( "tutorialControls6" ));
	}

	if( !mStages[ "LiftCrashed" ] ) {
		ruEngine::SetAmbientColor( ruVector3( 0.115, 0.115, 0.115 ));
		PlayAmbientSounds();
		if( pPlayer->IsInsideZone( mLiftStopZone )) {
			mLift->SetPaused( true );
			mLiftLamp->SetRange( 0.01f );
			mStages[ "LiftCrashed" ] = true;
			mLiftCrashSeries.SetEnabled( true );			
		}
	} else {
		// fully dark
		pPlayer->TurnOffFakeLight();
		ruEngine::SetAmbientColor( ruVector3( 0.0, 0.0, 0.0 ));
	}    
		
	if( pPlayer->DistanceTo( mHalt ) < 4 ) {
		mHalt->Unfreeze();
	}

    if( mChangeLevel ) {
        Level::Change( LevelName::L2Mine );
    }
}

void LevelArrival::OnDeserialize( SaveFile & in ){
	mLiftCrashSeries.Deserialize( in );
}

void LevelArrival::OnSerialize( SaveFile & out ){
	mLiftCrashSeries.Serialize( out );
}

// ACTIONS
void LevelArrival::ActLiftCrash_PowerDown() {
	mPowerDownSound->Play();		
	mWindSound->Pause();
}

void LevelArrival::ActLiftCrash_AfterPowerDown() {		
	mMetalStressSound->Play();
	pPlayer->TrembleCamera( 1.5f );
	mLift->SetEngineSoundEnabled( false );
	mLift->SetPaused( false );
	mLift->SetSpeedMultiplier( 0.24f );
}

void LevelArrival::ActLiftCrash_AfterFirstStressSound() {
	if( pPlayer->GetFlashLight() ) {
		pPlayer->GetFlashLight()->SwitchOff();
	}
	mLiftFallSound->Play();
	pPlayer->TrembleCamera( 2.0f );
	pPlayer->LockFlashlight( true );
}

void LevelArrival::ActLiftCrash_AfterFalldown() {
	mChangeLevel = true;
	pPlayer->LockFlashlight( false );
	pPlayer->SetHealth( 20 );
}
