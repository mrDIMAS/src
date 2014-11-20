#include "TestingChamber.h"
#include "Player.h"
#include "Item.h"
#include "CrawlWay.h"
#include "Ladder.h"
#include "Door.h"


TestingChamber::TestingChamber( ) {
    scene = LoadScene( "data/maps/candidates/testingChamber/testingChamber.scene" );

    SetPosition( player->body, GetPosition( FindByName("PlayerPosition") ) + Vector3( 0, 1, 0 ) );

    Item * fuelCanister = new Item( FindByName( "FuelCanister" ), Item::FuelCanister );
    Item * detonator = new Item( FindByName( "Detonator" ), Item::Detonator );
    Item * wires = new Item( FindByName( "Wires" ), Item::Wires );

    

    //CrawlWay * testCw = new CrawlWay( FindInObjectByName( scene, "CrawlBegin"), FindInObjectByName( scene, "CrawlEnd"), FindInObjectByName( scene, "CrawlEnter"),
    //                               FindInObjectByName( scene, "CrawlBeginLeavePoint"), FindInObjectByName( scene, "CrawlEndLeavePoint"));
    Ladder * testLadder = new Ladder( FindInObjectByName( scene, "LadderBegin"), FindInObjectByName( scene, "LadderEnd"), FindInObjectByName( scene, "LadderEnter"),
                                      FindInObjectByName( scene, "LadderBeginLeavePoint"), FindInObjectByName( scene, "LadderEndLeavePoint"));

    Door * door = new Door( FindInObjectByName( scene, "Door" ), 90 );

	SetAmbientColor( Vector3( 200, 200, 200 ));

	// create path, bruteforce!!11
	path.push_back( new GraphVertex( GetPosition( FindInObjectByName(scene, "Path00" ))) );
	path.push_back( new GraphVertex( GetPosition( FindInObjectByName(scene, "Path01" ))) );
	path.push_back( new GraphVertex( GetPosition( FindInObjectByName(scene, "Path02" ))) );
	path.push_back( new GraphVertex( GetPosition( FindInObjectByName(scene, "Path03" ))) );
	path.push_back( new GraphVertex( GetPosition( FindInObjectByName(scene, "Path04" ))) );
	path.push_back( new GraphVertex( GetPosition( FindInObjectByName(scene, "Path05" ))) );
	path.push_back( new GraphVertex( GetPosition( FindInObjectByName(scene, "Path06" ))) );
	path.push_back( new GraphVertex( GetPosition( FindInObjectByName(scene, "Path07" ))) );
	path.push_back( new GraphVertex( GetPosition( FindInObjectByName(scene, "Path08" ))) );
	path.push_back( new GraphVertex( GetPosition( FindInObjectByName(scene, "Path09" ))) );
	path.push_back( new GraphVertex( GetPosition( FindInObjectByName(scene, "Path10" ))) );
	path.push_back( new GraphVertex( GetPosition( FindInObjectByName(scene, "Path11" ))) );
	path.push_back( new GraphVertex( GetPosition( FindInObjectByName(scene, "PathToLadder00" ))) );
	path.push_back( new GraphVertex( GetPosition( FindInObjectByName(scene, "PathToLadder01" ))) );
	path.push_back( new GraphVertex( GetPosition( FindInObjectByName(scene, "PathToLadder02" ))) );
	path.push_back( new GraphVertex( GetPosition( FindInObjectByName(scene, "PathToLadder03" ))) );
	path.push_back( new GraphVertex( GetPosition( FindInObjectByName(scene, "PathToLadder04" ))) );
	path.push_back( new GraphVertex( GetPosition( FindInObjectByName(scene, "PathToLadder05" ))) );

	path[0]->AddEdge( path[1] );
	path[1]->AddEdge( path[2] );
	path[2]->AddEdge( path[3] );
	path[3]->AddEdge( path[4] );
	path[3]->AddEdge( path[5] );
	path[4]->AddEdge( path[12] );
	path[5]->AddEdge( path[6] );
	path[6]->AddEdge( path[7] );
	path[7]->AddEdge( path[8] );
	path[8]->AddEdge( path[9] );
	path[9]->AddEdge( path[10] );
	path[10]->AddEdge( path[11] );		
	path[12]->AddEdge( path[13] );
	path[13]->AddEdge( path[14] );
	path[14]->AddEdge( path[15] );
	path[15]->AddEdge( path[16] );
	path[16]->AddEdge( path[17] );

	vector< GraphVertex* > patrolPoints;
	patrolPoints.push_back( path[0] );
	patrolPoints.push_back( path[5] );
	patrolPoints.push_back( path[8] );

	enemy = new Enemy( "data/models/ripper/ripper.scene", path, patrolPoints );
}

TestingChamber::~TestingChamber() {

}

void TestingChamber::Show() {
    Level::Show();
}

void TestingChamber::Hide() {
    Level::Hide();
}

void TestingChamber::DoScenario() {
	SetAmbientColor( Vector3( 200, 200, 200 ));
    enemy->Think();
}