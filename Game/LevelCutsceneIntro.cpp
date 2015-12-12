#include "Precompiled.h"
#include "LevelCutsceneIntro.h"

LevelCutsceneIntro::LevelCutsceneIntro() : mEnginePitch( 1.0f, 0.65f, 1.25f )
{
	mTypeNum = LevelName::LCSIntro;

	LoadSceneFromFile( "data/maps/release/intro/intro.scene" );
	mUAZ = GetUniqueObject( "UAZ" );
	mLastUAZPosition = mUAZ.GetPosition();

	mUAZAnim = ruAnimation( 0, 350, 20 );
	mUAZ.SetAnimation( &mUAZAnim );
	mUAZAnim.enabled = true;

	ruSceneNode deerBone = GetUniqueObject( "Bone012" );
	mDeerAnim = ruAnimation( 0, 350, 35 );
	deerBone.SetAnimation( &mDeerAnim );
	mDeerAnim.enabled = true;

	AddSound( mEngineLoop = ruSound::Load3D( "data/sounds/engineloop.ogg"));
	mEngineLoop.Attach( GetUniqueObject( "Engine" ));
	mEngineLoop.SetLoop( true );
	mEngineLoop.Play();

	mNewLevelLoadZone = GetUniqueObject( "NewLevelLoadZone" );

	mChangeCameraZone1 = GetUniqueObject( "ChangeCamera1" );
	mChangeCameraZone2 = GetUniqueObject( "ChangeCamera2" );
	mChangeCameraZone3 = GetUniqueObject( "ChangeCamera3" );

	mCameraPivot = GetUniqueObject( "Camera" );
	mCameraAnim1 = ruAnimation( 0, 120, 12 );
	mCameraPivot.SetAnimation( &mCameraAnim1 );
	mCameraAnim1.enabled = true;

	mCameraPivot2 = GetUniqueObject( "Camera2" );
	mCameraPivot3 = GetUniqueObject( "Camera3" );
	mCameraPivot4 = GetUniqueObject( "Camera4" );

	mCamera = new GameCamera;
	mCamera->mNode.Attach( mCameraPivot );
	ruSetCameraSkybox( mCamera->mNode,
		ruGetTexture( "data/textures/skyboxes/night3/nightsky_u.jpg" ),
		ruGetTexture( "data/textures/skyboxes/night3/nightsky_l.jpg" ),
		ruGetTexture( "data/textures/skyboxes/night3/nightsky_r.jpg" ),
		ruGetTexture( "data/textures/skyboxes/night3/nightsky_f.jpg" ), 
		ruGetTexture( "data/textures/skyboxes/night3/nightsky_b.jpg" ));

	
	DoneInitialization();
}

LevelCutsceneIntro::~LevelCutsceneIntro()
{

}

void LevelCutsceneIntro::DoScenario() {
	if( Level::msCurLevelID != LevelName::LCSIntro ) {
		return;
	}

	mCamera->MakeCurrent();
	mUAZAnim.Update();
	mDeerAnim.Update();
	mCameraAnim1.Update();
	ruEngine::SetAmbientColor( ruVector3( 0.25, 0.25, 0.25 ));

	if( mUAZ.IsInsideNode( mChangeCameraZone1 )) {
		mCamera->mNode.Attach( mCameraPivot2 );
	}

	if( mUAZ.IsInsideNode( mChangeCameraZone2 )) {
		mCamera->mNode.Attach( mCameraPivot3 );
	}

	if( mUAZ.IsInsideNode( mChangeCameraZone3 )) {
		mCamera->mNode.Attach( mCameraPivot4 );
	}

	mEnginePitch.SetTarget((mUAZ.GetPosition() - mLastUAZPosition ).Length() * 10);
	mEnginePitch.ChaseTarget( 0.1f );

	mLastUAZPosition = mUAZ.GetPosition();
	mEngineLoop.SetPitch(mEnginePitch);

	if( mUAZ.IsInsideNode( mNewLevelLoadZone )) {
		Level::Change( LevelName::L1Arrival );
	}
}

void LevelCutsceneIntro::OnDeserialize( SaveFile & in )
{

}

void LevelCutsceneIntro::OnSerialize( SaveFile & out )
{

}
