#include "Precompiled.h"
#include "Keypad.h"
#include "LevelSewers.h"

LevelSewers::LevelSewers( ) : mWaterFlow( 0.0f ), mVerticalWaterFlow( 0.0f )  {
	mTypeNum = 5;

	LoadLocalization( "sewers.loc" );

	LoadSceneFromFile( "data/maps/release/sewers/sewers.scene" );

	pPlayer->SetPosition( GetUniqueObject( "PlayerPosition" )->GetPosition() );

	AddLadder( make_shared<Ladder>( GetUniqueObject( "Ladder3Begin" ), GetUniqueObject( "Ladder3End" ), GetUniqueObject( "Ladder3Enter" ),
		GetUniqueObject( "Ladder3BeginLeavePoint"), GetUniqueObject( "Ladder3EndLeavePoint")));

	mGate1 = unique_ptr<Gate>( new Gate( GetUniqueObject( "SmallGate1" ), GetUniqueObject( "Button1Open" ), GetUniqueObject( "Button1Close" ),
		GetUniqueObject( "Button1Open2" ), GetUniqueObject( "Button1Close2" ) ));

	mGate2 = unique_ptr<Gate>( new Gate( GetUniqueObject( "SmallGate2" ), GetUniqueObject( "Button2Open" ), GetUniqueObject( "Button2Close" ),
		GetUniqueObject( "Button2Open2" ), GetUniqueObject( "Button2Close2" ) ));

	mGateToLift = unique_ptr<Gate>( new Gate( GetUniqueObject( "SmallGate3" ), GetUniqueObject( "Button3Open" ), GetUniqueObject( "Button3Close" ),
		GetUniqueObject( "Button3Open2" ), GetUniqueObject( "Button3Close2" ) ));

	AutoCreateLampsByNamePattern( "Lamp?([[:digit:]]+)", "data/sounds/lamp_buzz.ogg" );

	mZoneKnocks = GetUniqueObject( "ZoneKnocks" );

	mKnocksSound = ruSound::Load3D( "data/sounds/knocks.ogg" );
	mKnocksSound->SetPosition( mZoneKnocks->GetPosition() );
	mKnocksSound->SetRolloffFactor( 0.2f );
	mKnocksSound->SetRoomRolloffFactor( 0.2f );

	ruSound::SetAudioReverb( 10 );

	AddSheet( new Sheet( GetUniqueObject( "Note1" ), mLocalization.GetString( "note1Desc" ), mLocalization.GetString( "note1" ) ) );
	AddSheet( new Sheet( GetUniqueObject( "Note2" ), mLocalization.GetString( "note2Desc" ), mLocalization.GetString( "note2" ) ) );


	AddDoor( make_shared<Door>( GetUniqueObject( "Door1" ), 90.0f ));
	AddDoor( make_shared<Door>( GetUniqueObject( "Door2" ), 90.0f ));
	AddDoor( make_shared<Door>( GetUniqueObject( "Door3" ), 90.0f ));
	AddDoor( make_shared<Door>( GetUniqueObject( "Door4" ), 90.0f ));
	AddDoor( make_shared<Door>( GetUniqueObject( "Door005" ), 90.0f ));
	AddDoor( make_shared<Door>( GetUniqueObject( "Door006" ), 90.0f ));
	AddDoor( make_shared<Door>( GetUniqueObject( "Door007" ), 90.0f ));
	AddDoor( make_shared<Door>( GetUniqueObject( "Door008" ), 90.0f ));
	AddDoor( make_shared<Door>( GetUniqueObject( "Door009" ), 90.0f ));
	AddDoor( make_shared<Door>( GetUniqueObject( "Door010" ), 90.0f ));

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
		mDoorToCode.get(), "5486" ));

	mEnemySpawnPosition = GetUniqueObject( "EnemySpawnPosition" );

	AddZone( mZoneEnemySpawn = make_shared<Zone>( GetUniqueObject( "EnemySpawnZone" )));
	mZoneEnemySpawn->OnPlayerEnter.AddListener( ruDelegate::Bind( this, &LevelSewers::OnPlayerEnterSpawnEnemyZone ));

	mWater = GetUniqueObject( "Water" );
	

	mStages[ "KnocksDone" ] = false;
	mStages[ "EnemySpawned" ] = false;

	std::regex rx( "VerticalWater?([[:digit:]]+)" );
	for( int i = 0; i < mScene->GetCountChildren(); i++ ) {
		shared_ptr<ruSceneNode> child = mScene->GetChild( i );
		if( regex_match( child->GetName(), rx )) {
			mVerticalWaterList.push_back( child );
		}
	}

	


	DoneInitialization();
}

void LevelSewers::CreateEnemy()
{
	// create paths
	Path wayNorth; BuildPath( wayNorth, "WayNorth" );
	Path wayWest; BuildPath( wayWest, "WayWest" );
	Path waySouthWest; BuildPath( waySouthWest, "WaySouthWest" );
	Path wayBasement; BuildPath( wayBasement, "WayBasement" );

	// add edges
	wayNorth.mVertexList[0]->AddEdge( wayWest.mVertexList[0] );
	wayNorth.mVertexList[0]->AddEdge( waySouthWest.mVertexList[0] );
	wayBasement.mVertexList[0]->AddEdge( waySouthWest.mVertexList.back() );

	// concatenate paths
	vector<GraphVertex*> allPaths;
	allPaths.insert( allPaths.end(), wayNorth.mVertexList.begin(), wayNorth.mVertexList.end() );
	allPaths.insert( allPaths.end(), wayWest.mVertexList.begin(), wayWest.mVertexList.end() );
	allPaths.insert( allPaths.end(), waySouthWest.mVertexList.begin(), waySouthWest.mVertexList.end() );
	allPaths.insert( allPaths.end(), wayBasement.mVertexList.begin(), wayBasement.mVertexList.end() );

	// create patrol paths
	vector< GraphVertex* > patrolPoints;

	patrolPoints.push_back( wayNorth.mVertexList.front() );
	patrolPoints.push_back( wayNorth.mVertexList.back() );

	patrolPoints.push_back( wayWest.mVertexList.front() );
	patrolPoints.push_back( wayWest.mVertexList.back() );

	patrolPoints.push_back( waySouthWest.mVertexList.front() );
	patrolPoints.push_back( waySouthWest.mVertexList.back() );

	patrolPoints.push_back( wayBasement.mVertexList.front() );
	patrolPoints.push_back( wayBasement.mVertexList.back() );

	mEnemy = unique_ptr<Enemy>( new Enemy( allPaths, patrolPoints ));
	mEnemy->SetPosition( mEnemySpawnPosition->GetPosition() );
}


LevelSewers::~LevelSewers( ) {

}

void LevelSewers::DoScenario() {
	mWater->SetTexCoordFlow( ruVector2( 0.0, -mWaterFlow ));
	mWaterFlow += 0.00025f;

	mVerticalWaterFlow += 0.001;
	for( auto & pVW : mVerticalWaterList ) {
		pVW->SetTexCoordFlow( ruVector2( 0.0f, mVerticalWaterFlow ));
	}

	if( mEnemy ) {
		mEnemy->Think();
	}

	ruEngine::SetAmbientColor( ruVector3( 9.5f / 255.0f, 9.5f / 255.0f, 9.5f / 255.0f ));
	mGate1->Update();
	mGate2->Update();
	mGateToLift->Update();

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

void LevelSewers::OnDeserialize( SaveFile & in ) {
	if( in.ReadBoolean() ) {
		CreateEnemy();
		mEnemy->SetPosition( in.ReadVector3() );
	}
}

void LevelSewers::OnSerialize( SaveFile & out ) {
	out.WriteBoolean( mEnemy != nullptr );
	if( mEnemy ) {
		out.WriteVector3( mEnemy->GetBody()->GetPosition());
	}
}

