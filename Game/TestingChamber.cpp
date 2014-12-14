#include "TestingChamber.h"
#include "Player.h"
#include "Item.h"
#include "CrawlWay.h"
#include "Ladder.h"
#include "Door.h"
#include "FollowPath.h"

TestingChamber::TestingChamber( )
{
    LoadSceneFromFile( "data/maps/candidates/testingChamber/testingChamber.scene" );

    pPlayer->SetPosition( ruGetNodePosition( ruFindByName("PlayerPosition") ) + ruVector3( 0, 1, 0 ) );

    Item * fuelCanister = new Item( ruFindByName( "FuelCanister" ), Item::Type::FuelCanister );
    Item * detonator = new Item( ruFindByName( "Detonator" ), Item::Type::Detonator );
    Item * wires = new Item( ruFindByName( "Wires" ), Item::Type::Wires );

    Ladder * testLadder = new Ladder( GetUniqueObject( "LadderBegin"), GetUniqueObject( "LadderEnd"), GetUniqueObject( "LadderEnter"),
                                      GetUniqueObject( "LadderBeginLeavePoint"), GetUniqueObject( "LadderEndLeavePoint"));

	FollowPath * pFollowPath = new FollowPath;
	pFollowPath->SetEntryNode(GetUniqueObject( "LadderEnter"));
	pFollowPath->AddPoint(GetUniqueObject( "LadderBeginLeavePoint"));
	pFollowPath->AddPoint(GetUniqueObject( "LadderBegin"));
	pFollowPath->AddPoint(GetUniqueObject( "LadderEnd"));
	pFollowPath->AddPoint(GetUniqueObject( "LadderEndLeavePoint"));

    Door * door = new Door( GetUniqueObject( "Door" ), 90 );

    ruSetAmbientColor( ruVector3( 200, 200, 200 ));

    // create path, bruteforce!!11
    path.push_back( new GraphVertex( ruGetNodePosition( GetUniqueObject( "Path00" ))) );
    path.push_back( new GraphVertex( ruGetNodePosition( GetUniqueObject( "Path01" ))) );
    path.push_back( new GraphVertex( ruGetNodePosition( GetUniqueObject( "Path02" ))) );
    path.push_back( new GraphVertex( ruGetNodePosition( GetUniqueObject( "Path03" ))) );
    path.push_back( new GraphVertex( ruGetNodePosition( GetUniqueObject( "Path04" ))) );
    path.push_back( new GraphVertex( ruGetNodePosition( GetUniqueObject( "Path05" ))) );
    path.push_back( new GraphVertex( ruGetNodePosition( GetUniqueObject( "Path06" ))) );
    path.push_back( new GraphVertex( ruGetNodePosition( GetUniqueObject( "Path07" ))) );
    path.push_back( new GraphVertex( ruGetNodePosition( GetUniqueObject( "Path08" ))) );
    path.push_back( new GraphVertex( ruGetNodePosition( GetUniqueObject( "Path09" ))) );
    path.push_back( new GraphVertex( ruGetNodePosition( GetUniqueObject( "Path10" ))) );
    path.push_back( new GraphVertex( ruGetNodePosition( GetUniqueObject( "Path11" ))) );
    path.push_back( new GraphVertex( ruGetNodePosition( GetUniqueObject( "PathToLadder00" ))) );
    path.push_back( new GraphVertex( ruGetNodePosition( GetUniqueObject( "PathToLadder01" ))) );
    path.push_back( new GraphVertex( ruGetNodePosition( GetUniqueObject( "PathToLadder02" ))) );
    path.push_back( new GraphVertex( ruGetNodePosition( GetUniqueObject( "PathToLadder03" ))) );
    path.push_back( new GraphVertex( ruGetNodePosition( GetUniqueObject( "PathToLadder04" ))) );
    path.push_back( new GraphVertex( ruGetNodePosition( GetUniqueObject( "PathToLadder05" ))) );

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

TestingChamber::~TestingChamber()
{

}

void TestingChamber::Show()
{
    Level::Show();
}

void TestingChamber::Hide()
{
    Level::Hide();
}

void TestingChamber::DoScenario()
{
    ruSetAmbientColor( ruVector3( 200, 200, 200 ));
    enemy->Think();
}