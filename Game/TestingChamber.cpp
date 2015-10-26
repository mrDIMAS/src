#include "Precompiled.h"

#include "TestingChamber.h"
#include "Player.h"
#include "Item.h"
#include "Ladder.h"
#include "Door.h"

TestingChamber::TestingChamber( ) {
    LoadSceneFromFile( "data/maps/candidates/testingChamber/testingChamber.scene" );

    pPlayer->SetPosition( ruFindByName("PlayerPosition").GetPosition() + ruVector3( 0, 1, 0 ) );

    Item * fuelCanister = new Item( ruFindByName( "FuelCanister" ), Item::Type::FuelCanister );
    Item * detonator = new Item( ruFindByName( "Detonator" ), Item::Type::Detonator );
    Item * wires = new Item( ruFindByName( "Wires" ), Item::Type::Wires );

    Ladder * testLadder = new Ladder( GetUniqueObject( "LadderBegin"), GetUniqueObject( "LadderEnd"), GetUniqueObject( "LadderEnter"),
                                      GetUniqueObject( "LadderBeginLeavePoint"), GetUniqueObject( "LadderEndLeavePoint"));

    Door * door = new Door( GetUniqueObject( "Door" ), 90 );

    ruEngine::SetAmbientColor( ruVector3( 200, 200, 200 ));

    // create path, bruteforce!!11
    path.push_back( new GraphVertex( GetUniqueObject( "Path00" ).GetPosition()) );
    path.push_back( new GraphVertex( GetUniqueObject( "Path01" ).GetPosition()) );
    path.push_back( new GraphVertex( GetUniqueObject( "Path02" ).GetPosition()) );
    path.push_back( new GraphVertex( GetUniqueObject( "Path03" ).GetPosition()) );
    path.push_back( new GraphVertex( GetUniqueObject( "Path04" ).GetPosition()) );
    path.push_back( new GraphVertex( GetUniqueObject( "Path05" ).GetPosition()) );
    path.push_back( new GraphVertex( GetUniqueObject( "Path06" ).GetPosition()) );
    path.push_back( new GraphVertex( GetUniqueObject( "Path07" ).GetPosition()) );
    path.push_back( new GraphVertex( GetUniqueObject( "Path08" ).GetPosition()) );
    path.push_back( new GraphVertex( GetUniqueObject( "Path09" ).GetPosition()) );
    path.push_back( new GraphVertex( GetUniqueObject( "Path10" ).GetPosition()) );
    path.push_back( new GraphVertex( GetUniqueObject( "Path11" ).GetPosition()) );
    path.push_back( new GraphVertex( GetUniqueObject( "PathToLadder00" ).GetPosition()) );
    path.push_back( new GraphVertex( GetUniqueObject( "PathToLadder01" ).GetPosition()) );
    path.push_back( new GraphVertex( GetUniqueObject( "PathToLadder02" ).GetPosition()) );
    path.push_back( new GraphVertex( GetUniqueObject( "PathToLadder03" ).GetPosition()) );
    path.push_back( new GraphVertex( GetUniqueObject( "PathToLadder04" ).GetPosition()) );
    path.push_back( new GraphVertex( GetUniqueObject( "PathToLadder05" ).GetPosition()) );

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

    enemy = new Enemy( path, patrolPoints );
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
    ruEngine::SetAmbientColor( ruVector3( 200, 200, 200 ));
    enemy->Think();
}