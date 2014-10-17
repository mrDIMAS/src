#include "TestingChamber.h"
#include "Player.h"
#include "Item.h"
#include "CrawlWay.h"
#include "Ladder.h"
#include "Door.h"


TestingChamber::TestingChamber( ) {
    scene = LoadScene( "data/maps/testingChamber/testingChamber.scene" );

    SetPosition( player->body, GetPosition( FindByName("PlayerPosition") ) + Vector3( 0, 1, 0 ) );
    Animate( FindByName( "Torus001"), 0.1, 1 );

    Item * fuelCanister = new Item( FindByName( "FuelCanister" ), Item::FuelCanister );
    Item * detonator = new Item( FindByName( "Detonator" ), Item::Detonator );
    Item * wires = new Item( FindByName( "Wires" ), Item::Wires );

    enemy = new Enemy( "data/models/ripper/ripper.scene" );

    //CrawlWay * testCw = new CrawlWay( FindInObjectByName( scene, "CrawlBegin"), FindInObjectByName( scene, "CrawlEnd"), FindInObjectByName( scene, "CrawlEnter"),
    //                               FindInObjectByName( scene, "CrawlBeginLeavePoint"), FindInObjectByName( scene, "CrawlEndLeavePoint"));
    Ladder * testLadder = new Ladder( FindInObjectByName( scene, "LadderBegin"), FindInObjectByName( scene, "LadderEnd"), FindInObjectByName( scene, "LadderEnter"),
                                      FindInObjectByName( scene, "LadderBeginLeavePoint"), FindInObjectByName( scene, "LadderEndLeavePoint"));

    Door * door = new Door( FindInObjectByName( scene, "Door" ), 90 );
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
    enemy->Update();
}