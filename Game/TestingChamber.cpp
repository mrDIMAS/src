#include "TestingChamber.h"
#include "Player.h"
#include "Item.h"
#include "CrawlWay.h"
#include "Ladder.h"
#include "Door.h"


TestingChamber::TestingChamber( ) {
    scene = ruLoadScene( "data/maps/candidates/testingChamber/testingChamber.scene" );

    ruSetNodePosition( player->body, ruGetNodePosition( ruFindByName("PlayerPosition") ) + ruVector3( 0, 1, 0 ) );

    Item * fuelCanister = new Item( ruFindByName( "FuelCanister" ), Item::FuelCanister );
    Item * detonator = new Item( ruFindByName( "Detonator" ), Item::Detonator );
    Item * wires = new Item( ruFindByName( "Wires" ), Item::Wires );

    

    //CrawlWay * testCw = new CrawlWay( FindInObjectByName( scene, "CrawlBegin"), FindInObjectByName( scene, "CrawlEnd"), FindInObjectByName( scene, "CrawlEnter"),
    //                               FindInObjectByName( scene, "CrawlBeginLeavePoint"), FindInObjectByName( scene, "CrawlEndLeavePoint"));
    Ladder * testLadder = new Ladder( ruFindInObjectByName( scene, "LadderBegin"), ruFindInObjectByName( scene, "LadderEnd"), ruFindInObjectByName( scene, "LadderEnter"),
                                      ruFindInObjectByName( scene, "LadderBeginLeavePoint"), ruFindInObjectByName( scene, "LadderEndLeavePoint"));

    Door * door = new Door( ruFindInObjectByName( scene, "Door" ), 90 );

	ruSetAmbientColor( ruVector3( 200, 200, 200 ));

	// create path, bruteforce!!11
	path.push_back( new GraphVertex( ruGetNodePosition( ruFindInObjectByName(scene, "Path00" ))) );
	path.push_back( new GraphVertex( ruGetNodePosition( ruFindInObjectByName(scene, "Path01" ))) );
	path.push_back( new GraphVertex( ruGetNodePosition( ruFindInObjectByName(scene, "Path02" ))) );
	path.push_back( new GraphVertex( ruGetNodePosition( ruFindInObjectByName(scene, "Path03" ))) );
	path.push_back( new GraphVertex( ruGetNodePosition( ruFindInObjectByName(scene, "Path04" ))) );
	path.push_back( new GraphVertex( ruGetNodePosition( ruFindInObjectByName(scene, "Path05" ))) );
	path.push_back( new GraphVertex( ruGetNodePosition( ruFindInObjectByName(scene, "Path06" ))) );
	path.push_back( new GraphVertex( ruGetNodePosition( ruFindInObjectByName(scene, "Path07" ))) );
	path.push_back( new GraphVertex( ruGetNodePosition( ruFindInObjectByName(scene, "Path08" ))) );
	path.push_back( new GraphVertex( ruGetNodePosition( ruFindInObjectByName(scene, "Path09" ))) );
	path.push_back( new GraphVertex( ruGetNodePosition( ruFindInObjectByName(scene, "Path10" ))) );
	path.push_back( new GraphVertex( ruGetNodePosition( ruFindInObjectByName(scene, "Path11" ))) );
	path.push_back( new GraphVertex( ruGetNodePosition( ruFindInObjectByName(scene, "PathToLadder00" ))) );
	path.push_back( new GraphVertex( ruGetNodePosition( ruFindInObjectByName(scene, "PathToLadder01" ))) );
	path.push_back( new GraphVertex( ruGetNodePosition( ruFindInObjectByName(scene, "PathToLadder02" ))) );
	path.push_back( new GraphVertex( ruGetNodePosition( ruFindInObjectByName(scene, "PathToLadder03" ))) );
	path.push_back( new GraphVertex( ruGetNodePosition( ruFindInObjectByName(scene, "PathToLadder04" ))) );
	path.push_back( new GraphVertex( ruGetNodePosition( ruFindInObjectByName(scene, "PathToLadder05" ))) );

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
	ruSetAmbientColor( ruVector3( 200, 200, 200 ));
    enemy->Think();
}