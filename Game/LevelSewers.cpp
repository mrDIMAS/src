#include "LevelSewers.h"

LevelSewers::LevelSewers( ) {
	mTypeNum = 5;
	LoadSceneFromFile( "data/maps/release/sewers/sewers.scene" );

	pPlayer->SetPosition( ruGetNodePosition( GetUniqueObject( "PlayerPosition" ) ));

	AddLadder( new Ladder( GetUniqueObject( "Ladder1Begin" ), GetUniqueObject( "Ladder1End" ), GetUniqueObject( "Ladder1Enter" ),
		GetUniqueObject( "Ladder1BeginLeavePoint"), GetUniqueObject( "Ladder1EndLeavePoint")));
	AddLadder( new Ladder( GetUniqueObject( "Ladder2Begin" ), GetUniqueObject( "Ladder2End" ), GetUniqueObject( "Ladder2Enter" ),
		GetUniqueObject( "Ladder2BeginLeavePoint"), GetUniqueObject( "Ladder2EndLeavePoint")));
	AddLadder( new Ladder( GetUniqueObject( "Ladder3Begin" ), GetUniqueObject( "Ladder3End" ), GetUniqueObject( "Ladder3Enter" ),
		GetUniqueObject( "Ladder3BeginLeavePoint"), GetUniqueObject( "Ladder3EndLeavePoint")));
	AddLadder( new Ladder( GetUniqueObject( "Ladder4Begin" ), GetUniqueObject( "Ladder4End" ), GetUniqueObject( "Ladder4Enter" ),
		GetUniqueObject( "Ladder4BeginLeavePoint"), GetUniqueObject( "Ladder4EndLeavePoint")));

	mGate1 = new Gate( GetUniqueObject( "SmallGate1" ), GetUniqueObject( "Button1Open" ), GetUniqueObject( "Button1Close" ),
		GetUniqueObject( "Button1Open2" ), GetUniqueObject( "Button1Close2" ) );

	mGate2 = new Gate( GetUniqueObject( "SmallGate2" ), GetUniqueObject( "Button2Open" ), GetUniqueObject( "Button2Close" ),
		GetUniqueObject( "Button2Open2" ), GetUniqueObject( "Button2Close2" ) );

	AutoCreateLampsByNamePattern( "Lamp?([[:digit:]]+)", "data/sounds/lamp_buzz.ogg" );

	mZoneKnocks = GetUniqueObject( "ZoneKnocks" );

	mKnocksSound = ruLoadSound3D( "data/sounds/knocks.ogg" );
	ruSetSoundPosition( mKnocksSound, ruGetNodePosition( mZoneKnocks ));
	ruSetRolloffFactor( mKnocksSound, 0.2f );
	ruSetRoomRolloffFactor( mKnocksSound, 0.2f );

	ruSetAudioReverb( 10 );
 
	pPlayer->SetRockFootsteps();
	DoneInitialization();

	mStages[ "KnocksDone" ] = false;
}

LevelSewers::~LevelSewers( ) {

}

void LevelSewers::DoScenario() {
	ruSetAmbientColor( ruVector3( 12.5f / 255.0f, 12.5f / 255.0f, 12.5f / 255.0f ));
	mGate1->Update();
	mGate2->Update();

	if( !mStages[ "KnocksDone" ] ) {
		if( pPlayer->IsInsideZone( mZoneKnocks )) {
			ruPlaySound( mKnocksSound );
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

void LevelSewers::OnDeserialize( TextFileStream & in )
{

}

void LevelSewers::OnSerialize( TextFileStream & out )
{

}
