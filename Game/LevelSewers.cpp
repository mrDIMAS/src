#include "Precompiled.h"

#include "LevelSewers.h"

LevelSewers::LevelSewers( ) {
	mTypeNum = 5;
	LoadSceneFromFile( "data/maps/release/sewers/sewers.scene" );

	pPlayer->SetPosition( GetUniqueObject( "PlayerPosition" ).GetPosition() );

	AddLadder( make_shared<Ladder>( GetUniqueObject( "Ladder1Begin" ), GetUniqueObject( "Ladder1End" ), GetUniqueObject( "Ladder1Enter" ),
		GetUniqueObject( "Ladder1BeginLeavePoint"), GetUniqueObject( "Ladder1EndLeavePoint")));
	AddLadder( make_shared<Ladder>( GetUniqueObject( "Ladder2Begin" ), GetUniqueObject( "Ladder2End" ), GetUniqueObject( "Ladder2Enter" ),
		GetUniqueObject( "Ladder2BeginLeavePoint"), GetUniqueObject( "Ladder2EndLeavePoint")));
	AddLadder( make_shared<Ladder>( GetUniqueObject( "Ladder3Begin" ), GetUniqueObject( "Ladder3End" ), GetUniqueObject( "Ladder3Enter" ),
		GetUniqueObject( "Ladder3BeginLeavePoint"), GetUniqueObject( "Ladder3EndLeavePoint")));
	AddLadder( make_shared<Ladder>( GetUniqueObject( "Ladder4Begin" ), GetUniqueObject( "Ladder4End" ), GetUniqueObject( "Ladder4Enter" ),
		GetUniqueObject( "Ladder4BeginLeavePoint"), GetUniqueObject( "Ladder4EndLeavePoint")));

	mGate1 = new Gate( GetUniqueObject( "SmallGate1" ), GetUniqueObject( "Button1Open" ), GetUniqueObject( "Button1Close" ),
		GetUniqueObject( "Button1Open2" ), GetUniqueObject( "Button1Close2" ) );

	mGate2 = new Gate( GetUniqueObject( "SmallGate2" ), GetUniqueObject( "Button2Open" ), GetUniqueObject( "Button2Close" ),
		GetUniqueObject( "Button2Open2" ), GetUniqueObject( "Button2Close2" ) );

	AutoCreateLampsByNamePattern( "Lamp?([[:digit:]]+)", "data/sounds/lamp_buzz.ogg" );

	mZoneKnocks = GetUniqueObject( "ZoneKnocks" );

	mKnocksSound = ruSound::Load3D( "data/sounds/knocks.ogg" );
	mKnocksSound.SetPosition( mZoneKnocks.GetPosition() );
	mKnocksSound.SetRolloffFactor( 0.2f );
	mKnocksSound.SetRoomRolloffFactor( 0.2f );

	ruSetAudioReverb( 10 );
 
	pPlayer->SetRockFootsteps();
	DoneInitialization();

	mStages[ "KnocksDone" ] = false;
}

LevelSewers::~LevelSewers( ) {

}

void LevelSewers::DoScenario() {
	ruEngine::SetAmbientColor( ruVector3( 12.5f / 255.0f, 12.5f / 255.0f, 12.5f / 255.0f ));
	mGate1->Update();
	mGate2->Update();

	if( !mStages[ "KnocksDone" ] ) {
		if( pPlayer->IsInsideZone( mZoneKnocks )) {
			mKnocksSound.Play();
			mStages[ "KnocksDone" ] = true;
		}
	}
}

void LevelSewers::Show() {
	Level::Show();
}

void LevelSewers::Hide() {
	Level::Hide();
}

void LevelSewers::OnDeserialize( SaveFile & in )
{

}

void LevelSewers::OnSerialize( SaveFile & out )
{

}
