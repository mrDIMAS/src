#include "Precompiled.h"

#include "LevelArrival.h"
#include "Player.h"
#include "LevelMine.h"


LevelArrival::LevelArrival( ) : mChangeLevel( false ), mPowerRestored( false ) {
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
    mGenerator = GetUniqueObject( "Generator" );

    mLamp1 = GetUniqueObject( "Projector1" );
	mLamp1.Hide();

    mLamp2 = GetUniqueObject( "Projector2" );
	mLamp2.Hide();

	mLiftLamp = GetUniqueObject( "LiftLamp" );
	mLiftLamp.Hide();


    //////////////////////////////////////////////////////////////////////////
    // Player noticements
    pPlayer->SetObjective( mLocalization.GetString( "objective1" ));

    pPlayer->SetPosition( GetUniqueObject("PlayerPosition").GetPosition() + ruVector3( 0, 1, 0 ));

    //////////////////////////////////////////////////////////////////////////
    // Load sounds
    AddSound( mGeneratorSound = ruSound::Load3D( "data/sounds/generator.ogg" ));
    mGeneratorSound.Attach( GetUniqueObject( "Generator" ) );
    mGeneratorSound.SetRolloffFactor( 5 );
    mGeneratorSound.SetReferenceDistance( 5 );
	mGeneratorSound.SetLoop( true );
	mGeneratorSound.Pause();
	
	AddSound( mPowerDownSound = ruSound::Load2D( "data/sounds/powerdown.ogg"));
	mPowerDownSound.SetVolume( 0.7 );
	AddSound( mMetalWhineSound = ruSound::Load2D( "data/sounds/metal_whining.ogg" ));
	mMetalWhineSound.SetVolume( 0.95 );

	AddSound( mMetalStressSound = ruSound::Load2D( "data/sounds/metal_stress1.ogg" ));
	AddSound( mLiftFallSound = ruSound::Load2D( "data/sounds/lift_fall.ogg" ));
	mLiftFallSound.SetVolume( 1.25f );

    AddSound( mWindSound = ruSound::Load2D( "data/sounds/wind.ogg"));
    mWindSound.SetVolume( 0.5f );
    mWindSound.Play();

	mLift = make_shared<Lift>( GetUniqueObject( "Lift1" ) );
	mLift->SetControlPanel( GetUniqueObject( "Lift1Screen" ));
	mLift->SetDestinationPoint( GetUniqueObject( "Lift1Dest" ));
	mLift->SetSourcePoint( GetUniqueObject( "Lift1Source" ));
	mLift->SetMotorSound( ruSound::Load3D( "data/sounds/motor_idle.ogg"));
	mLift->SetBackDoors( GetUniqueObject( "Lift1BackDoorLeft"), GetUniqueObject( "Lift1BackDoorRight" ));
	mLift->SetFrontDoors( GetUniqueObject( "Lift1FrontDoorLeft"), GetUniqueObject( "Lift1FrontDoorRight" ));
	AddLift( mLift );

    ruSetAudioReverb( 15 );

    AddAmbientSound( ruSound::Load3D( "data/sounds/ambient/forest/forestambient1.ogg" ));
    AddAmbientSound( ruSound::Load3D( "data/sounds/ambient/forest/forestambient2.ogg" ));
    AddAmbientSound( ruSound::Load3D( "data/sounds/ambient/forest/forestambient3.ogg" ));
    AddAmbientSound( ruSound::Load3D( "data/sounds/ambient/forest/forestambient4.ogg" ));
    AddAmbientSound( ruSound::Load3D( "data/sounds/ambient/forest/forestambient5.ogg" ));
    AddAmbientSound( ruSound::Load3D( "data/sounds/ambient/forest/forestambient6.ogg" ));

    ruSetCameraSkybox( pPlayer->mpCamera->mNode,
		ruGetTexture( "data/textures/skyboxes/night3/nightsky_u.jpg" ),
		ruGetTexture( "data/textures/skyboxes/night3/nightsky_l.jpg" ),
		ruGetTexture( "data/textures/skyboxes/night3/nightsky_r.jpg" ),
		ruGetTexture( "data/textures/skyboxes/night3/nightsky_f.jpg" ), 
		ruGetTexture( "data/textures/skyboxes/night3/nightsky_b.jpg" ));

	mStages[ "LiftCrashed" ] = false;

	mGeneratorStartSound = ruSound::Load3D( "data/sounds/generator_start.ogg" );
	mGeneratorStartSound.Attach( mGenerator );

	// every action series disabled by default
	mLiftCrashSeries.AddAction( 0.0f, ruDelegate::Bind( this, &LevelArrival::ActLiftCrash_PowerDown ) );
	mLiftCrashSeries.AddAction( mPowerDownSound.GetLength(), ruDelegate::Bind( this, &LevelArrival::ActLiftCrash_AfterPowerDown ) );
	mLiftCrashSeries.AddAction( mMetalStressSound.GetLength(), ruDelegate::Bind( this, &LevelArrival::ActLiftCrash_AfterFirstStressSound ) );
	mLiftCrashSeries.AddAction( mLiftFallSound.GetLength(), ruDelegate::Bind( this, &LevelArrival::ActLiftCrash_AfterFalldown ) );

	// when player pushes button on the generator, he starts mGeneratorStartSeries that actually plays sound sequence and starts the generator
	AddButton( mGeneratorButton = new Button( GetUniqueObject( "GeneratorButton" ), ruVector3( 0.0f, 0.0f, 1.0f ), ruSound::Load3D( "data/sounds/button_push.ogg" ), ruSound::Load3D( "data/sounds/button_pop.ogg" )));
	mGeneratorButton->OnPush.AddListener( ruDelegate::Bind( this, &LevelArrival::GeneratorEnableAction ) );

	mGeneratorStartSeries.AddAction( 0.0f, ruDelegate::Bind( this, &LevelArrival::ActGenerator_Start ) );
	mGeneratorStartSeries.AddAction( mGeneratorStartSound.GetLength(), ruDelegate::Bind( this, &LevelArrival::ActGenerator_OnLine ) );

	mGeneratorSmokePosition = GetUniqueObject( "GeneratorSmoke" ).GetPosition();


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
	mGeneratorStartSeries.Perform();

	if( mPowerRestored ) {
		mLift->Update();
	}

	if( !mStages[ "LiftCrashed" ] ) {
		ruEngine::SetAmbientColor( ruVector3( 0.11, 0.11, 0.11 ));
		PlayAmbientSounds();
		if( pPlayer->IsInsideZone( mLiftStopZone )) {
			mLift->SetPaused( true );
			ruSetLightRange( mLiftLamp, 0.01f );
			mStages[ "LiftCrashed" ] = true;
			mLiftCrashSeries.SetEnabled( true );			
		}
	} else {
		// fully dark
		ruEngine::SetAmbientColor( ruVector3( 0.0, 0.0, 0.0 ));
	}    
		
    if( mChangeLevel ) {
        Level::Change( LevelName::L2Mine );
    }
}

void LevelArrival::OnDeserialize( SaveFile & in )
{
	mLiftCrashSeries.Deserialize( in );
	mGeneratorStartSeries.Deserialize( in );
}

void LevelArrival::OnSerialize( SaveFile & out )
{
	mLiftCrashSeries.Serialize( out );
	mGeneratorStartSeries.Serialize( out );
}

// ACTIONS
void LevelArrival::ActLiftCrash_PowerDown() {
	mPowerDownSound.Play();		
	mWindSound.Pause();
	mGeneratorSound.Pause();
}
void LevelArrival::ActLiftCrash_AfterPowerDown() {		
	mMetalStressSound.Play();
	pPlayer->TrembleCamera( 1.5f );
	mLift->SetEngineSoundEnabled( false );
	mLift->SetPaused( false );
	mLift->SetSpeedMultiplier( 0.24f );
}
void LevelArrival::ActLiftCrash_AfterFirstStressSound() {
	pPlayer->GetFlashLight()->SwitchOff();
	mLiftFallSound.Play();
	pPlayer->TrembleCamera( 2.0f );
	pPlayer->LockFlashlight( true );
}
void LevelArrival::ActLiftCrash_AfterFalldown() {
	mChangeLevel = true;
	pPlayer->LockFlashlight( false );
	pPlayer->SetHealth( 20 );
}
// when player pushes generator start button
void LevelArrival::GeneratorEnableAction( ) {
	mGeneratorStartSeries.SetEnabled( true );
}
void LevelArrival::ActGenerator_Start() {
	mGeneratorStartSound.Play();
	ruParticleSystemProperties psProps;
	psProps.type = PS_STREAM;
	psProps.speedDeviationMin = ruVector3( -0.0025, 0.014, 0.0015 );
	psProps.speedDeviationMax = ruVector3( -0.0025, 0.024, 0.0015 );;
	psProps.texture = ruGetTexture( "data/textures/particles/p1.png");
	psProps.colorBegin = ruVector3( 150, 150, 150 );
	psProps.colorEnd = ruVector3( 150, 150, 150 );
	psProps.pointSize = 0.05f;
	psProps.particleThickness = 1.5f;
	psProps.boundingRadius = 0.8f;
	psProps.useLighting = true;
	psProps.scaleFactor = 0.005f;
	psProps.alphaOffset = 0.8f;
	mGeneratorSmoke = ruCreateParticleSystem( 30, psProps );
	mGeneratorSmoke.SetPosition( mGeneratorSmokePosition );
} 
void LevelArrival::ActGenerator_OnLine() {
	mLamp1.Show();
	mLamp2.Show();
	mLiftLamp.Show();		
	mPowerRestored = true;
	mGeneratorSound.Play();
}
