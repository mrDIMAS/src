#include "Precompiled.h"
#include "Keypad.h"
#include "LevelSewers.h"

LevelSewers::LevelSewers( ) {
	mTypeNum = 5;
	LoadSceneFromFile( "data/maps/release/sewers/sewers.scene" );

	pPlayer->SetPosition( GetUniqueObject( "PlayerPosition" )->GetPosition() );

	AddLadder( make_shared<Ladder>( GetUniqueObject( "Ladder3Begin" ), GetUniqueObject( "Ladder3End" ), GetUniqueObject( "Ladder3Enter" ),
		GetUniqueObject( "Ladder3BeginLeavePoint"), GetUniqueObject( "Ladder3EndLeavePoint")));

	mGate1 = unique_ptr<Gate>( new Gate( GetUniqueObject( "SmallGate1" ), GetUniqueObject( "Button1Open" ), GetUniqueObject( "Button1Close" ),
		GetUniqueObject( "Button1Open2" ), GetUniqueObject( "Button1Close2" ) ));

	mGate2 = unique_ptr<Gate>( new Gate( GetUniqueObject( "SmallGate2" ), GetUniqueObject( "Button2Open" ), GetUniqueObject( "Button2Close" ),
		GetUniqueObject( "Button2Open2" ), GetUniqueObject( "Button2Close2" ) ));

	AutoCreateLampsByNamePattern( "Lamp?([[:digit:]]+)", "data/sounds/lamp_buzz.ogg" );

	mZoneKnocks = GetUniqueObject( "ZoneKnocks" );

	mKnocksSound = ruSound::Load3D( "data/sounds/knocks.ogg" );
	mKnocksSound->SetPosition( mZoneKnocks->GetPosition() );
	mKnocksSound->SetRolloffFactor( 0.2f );
	mKnocksSound->SetRoomRolloffFactor( 0.2f );

	ruSound::SetAudioReverb( 10 );

	AddDoor( make_shared<Door>( GetUniqueObject( "Door1" ), 90.0f ));
	AddDoor( make_shared<Door>( GetUniqueObject( "Door2" ), 90.0f ));
	AddDoor( make_shared<Door>( GetUniqueObject( "Door3" ), 90.0f ));
	AddDoor( make_shared<Door>( GetUniqueObject( "Door4" ), 90.0f ));
	AddDoor( make_shared<Door>( GetUniqueObject( "Door005" ), 90.0f ));
	AddDoor( make_shared<Door>( GetUniqueObject( "Door006" ), 90.0f ));
	AddDoor( make_shared<Door>( GetUniqueObject( "Door007" ), 90.0f ));
	AddDoor( make_shared<Door>( GetUniqueObject( "Door008" ), 90.0f ));
	AddDoor( make_shared<Door>( GetUniqueObject( "Door009" ), 90.0f ));

	AddDoor( mDoorToControl = make_shared<Door>( GetUniqueObject( "DoorToControl" ), 90.0f ));

	mKeypad1 = unique_ptr<Keypad>( new Keypad( GetUniqueObject( "Keypad1"), GetUniqueObject( "Keypad1Key0" ), GetUniqueObject( "Keypad1Key1"),
		GetUniqueObject( "Keypad1Key2"), GetUniqueObject( "Keypad1Key3"), GetUniqueObject( "Keypad1Key4"),
		GetUniqueObject( "Keypad1Key5"), GetUniqueObject( "Keypad1Key6" ), GetUniqueObject( "Keypad1Key7"),
		GetUniqueObject( "Keypad1Key8"), GetUniqueObject( "Keypad1Key9"), GetUniqueObject( "Keypad1KeyCancel"), 
		mDoorToControl.get(), "9632" ));

	AddDoor( mDoorToCode = make_shared<Door>( GetUniqueObject( "DoorToCode" ), 90.0f ));

	mKeypad2 = unique_ptr<Keypad>( new Keypad( GetUniqueObject( "Keypad2"), GetUniqueObject( "Keypad2Key0" ), GetUniqueObject( "Keypad2Key1"),
		GetUniqueObject( "Keypad2Key2"), GetUniqueObject( "Keypad2Key3"), GetUniqueObject( "Keypad2Key4"),
		GetUniqueObject( "Keypad2Key5"), GetUniqueObject( "Keypad2Key6" ), GetUniqueObject( "Keypad2Key7"),
		GetUniqueObject( "Keypad2Key8"), GetUniqueObject( "Keypad2Key9"), GetUniqueObject( "Keypad2KeyCancel"), 
		mDoorToCode.get(), "9632" ));

	DoneInitialization();

	mStages[ "KnocksDone" ] = false;
}

LevelSewers::~LevelSewers( ) {

}

void LevelSewers::DoScenario() {
	ruEngine::SetAmbientColor( ruVector3( 9.5f / 255.0f, 9.5f / 255.0f, 9.5f / 255.0f ));
	mGate1->Update();
	mGate2->Update();

	mKeypad1->Update();
	mKeypad2->Update();

	if( !mStages[ "KnocksDone" ] ) {
		if( pPlayer->IsInsideZone( mZoneKnocks )) {
			mKnocksSound->Play();
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
