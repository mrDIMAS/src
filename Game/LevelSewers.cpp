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

	mGate1 = new Gate( GetUniqueObject( "SmallGate1" ), GetUniqueObject( "Button1Open" ), GetUniqueObject( "Button1Close" ),
		GetUniqueObject( "Button1Open2" ), GetUniqueObject( "Button1Close2" ) );

	mGate2 = new Gate( GetUniqueObject( "SmallGate2" ), GetUniqueObject( "Button2Open" ), GetUniqueObject( "Button2Close" ),
		GetUniqueObject( "Button2Open2" ), GetUniqueObject( "Button2Close2" ) );

	DoneInitialization();
}

LevelSewers::~LevelSewers( ) {

}

void LevelSewers::DoScenario() {
	ruSetAmbientColor( ruVector3( 12.5f / 255.0f, 12.5f / 255.0f, 12.5f / 255.0f ));
	mGate1->Update();
	mGate2->Update();
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