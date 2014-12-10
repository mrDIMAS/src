#include "Level.h"
#include "LevelArrival.h"
#include "LevelMine.h"
#include "GUI.h"
#include "Player.h"
#include "Menu.h"
#include "LevelResearchFacility.h"
#include "TestingChamber.h"
#include "LevelIntroduction.h"
#include "SaveLoader.h"
#include "SaveWriter.h"

int g_initialLevel;
Level * pCurrentLevel = 0;
int Level::curLevelID = 0;

Level::Level()
{
	mInitializationComplete = false;
    mTypeNum = 0; //undefined
    music.Invalidate();
}

Level::~Level()
{
    ruFreeSceneNode( scene );

    for( auto item : items )
        if( !item->mInInventory )
            delete item;

    for( auto sheet : sheets )
        delete sheet;

    for( auto door : doors )
        delete door;

    for( auto crawlWay : crawlWays )
        delete crawlWay;

    for( auto ladder : ladders )
        delete ladder;

    for( auto itemPlace : itemPlaces )
        delete itemPlace;

    for( auto valve : valves )
        delete valve;

    for( auto lift : lifts )
        delete lift;

    for( auto sound : sounds )
        ruFreeSound( sound );
}

void Level::LoadLocalization( string fn )
{
    mLocalization.ParseFile( localizationPath + fn );
}

void Level::Hide()
{
    ruHideNode( scene );
    for( auto & sound : sounds )
        ruPauseSound( sound );
}

void Level::Show()
{
    ruShowNode( scene );
    for( auto & sound : sounds )
    {
        if( ruIsSoundPaused( sound ))
            ruPlaySound( sound );
    }
}

void Level::Change( int levelId, bool continueFromSave )
{
    Level::curLevelID = levelId;

    static int lastLevel = 0;

    if( lastLevel != Level::curLevelID )
    {
        ruDrawGUIText( pMainMenu->mLocalization.GetString( "loading" ), ruGetResolutionWidth() / 2 - 64, ruGetResolutionHeight() / 2 - 64, 128, 128, pGUI->mFont, ruVector3( 255, 255, 0), 1 );

        // draw 'loading' string
        ruRenderWorld( 1.0f / 60.0f );

        lastLevel = Level::curLevelID;

        if( !pPlayer && Level::curLevelID != LevelName::L0Introduction )
            pPlayer = new Player();

        if( pCurrentLevel )
            delete pCurrentLevel;

        if( Level::curLevelID == LevelName::L0Introduction )
            pCurrentLevel = new LevelIntroduction;
        else
            pPlayer->FreeHands();

        if( Level::curLevelID == LevelName::L1Arrival )
            pCurrentLevel = new LevelArrival;

        if( Level::curLevelID == LevelName::L2Mine )
            pCurrentLevel = new LevelMine;

        if( Level::curLevelID == LevelName::L3ResearchFacility )
            pCurrentLevel = new LevelResearchFacility;

        if( Level::curLevelID == LevelName::LXTestingChamber )
            pCurrentLevel = new TestingChamber;

        if( continueFromSave )
            SaveLoader( "lastGame.save" ).RestoreWorldState();
        else
        {
            //if( player ) {
            //    SaveWriter( "quickSave.save" ).SaveWorldState();
            //}
        }

        // loading can take a lot of time, so g_dt becomes a really huge value which can cause annoying bugs
        // so restart dtTimer

        for( int i = 0; i < ruGetWorldPointLightCount(); i++ )
        {
            ruSetLightFloatingEnabled( ruGetWorldPointLight( i ), true );
            float d = 0.1f;
            ruSetLightFloatingLimits( ruGetWorldPointLight( i ), ruVector3( -d, -d, -d ), ruVector3( d, d, d ) );
        }
    }
}

void Level::AddLift( Lift * lift )
{
    lifts.push_back( lift );
}

void Level::AddValve( Valve * valve )
{
    valves.push_back( valve );
}

void Level::AddLadder( Ladder * ladder )
{
    ladders.push_back( ladder );
}

void Level::AddCrawlWay( CrawlWay * cw )
{
    crawlWays.push_back( cw );
}

void Level::AddDoor( Door * door )
{
    doors.push_back( door );
}

void Level::AddSheet( Sheet * sheet )
{
    sheets.push_back( sheet );
}

void Level::AddItem( Item * item )
{
    items.push_back( item );
}

void Level::AddItemPlace( ItemPlace * ipc )
{
    itemPlaces.push_back( ipc );
}

void Level::DeserializeWith( TextFileStream & in )
{
    int childCount = in.ReadInteger( );
    for( int i = 0; i < childCount; i++ )
    {
        string name = in.Readstring();
        ruNodeHandle node = ruFindInObjectByName( scene, name.c_str() );
        if( node.IsValid() )
        {
            ruSetNodeLocalPosition( node, in.ReadVector3() );
            ruSetNodeLocalRotation( node, in.ReadQuaternion() );
            bool visible = in.ReadBoolean();
            if( visible )
                ruShowNode( node );
            else
                ruHideNode( node );
        }
    }
    int countStages = in.ReadInteger();
    for( int i = 0; i < countStages; i++ )
    {
        string stageName = in.Readstring();
        bool stageState = in.ReadBoolean();
        stages[ stageName ] = stageState;
    }
    OnDeserialize( in );
}

void Level::SerializeWith( TextFileStream & out )
{
    int childCount = ruGetNodeCountChildren( scene );
    out.WriteInteger( childCount );
    for( int i = 0; i < childCount; i++ )
    {
        ruNodeHandle node = ruGetNodeChild( scene, i );
        out.WriteString( ruGetNodeName( node ));
        out.WriteVector3( ruGetNodeLocalPosition( node ));
        out.WriteQuaternion( ruGetNodeLocalRotation( node ));
        out.WriteBoolean( ruIsNodeVisible( node ));
    }
    out.WriteInteger( stages.size());
    for( auto stage : stages )
    {
        out.WriteString( stage.first );
        out.WriteBoolean( stage.second );
    }
    OnSerialize( out );
}

void Level::AddSound( ruSoundHandle sound )
{
    sounds.push_back( sound );
}

void Level::PlayAmbientSounds()
{
    ambSoundSet.DoRandomPlaying();
}

void Level::AddAmbientSound( ruSoundHandle sound )
{
    sounds.push_back( sound );
    ambSoundSet.AddSound( sound );
}

ruNodeHandle Level::GetUniqueObject( const char * name )
{
	if( mInitializationComplete ) {
		RaiseError( Format( "You must get object in game level intialization! Get object in game logic is strictly forbidden! Object name: '%s'", name ));
	}
	if( !scene.IsValid() ) {
		RaiseError( Format( "Object '%s' can't be found in the empty scene. Load scene first!", name ));
	}
	ruNodeHandle object = ruFindInObjectByName( scene, name );
	// each unique object must be presented in the scene, otherwise error will be generated
	if( !object.IsValid() ) {
		RaiseError( Format( "Object '%s' can't be found in the scene! Game will be closed.", name ));
	}
	return object;
}

void Level::LoadSceneFromFile( const char * file )
{
	scene = ruLoadScene( file );
	if( !scene.IsValid() ) {
		RaiseError( Format( "Unable to load scene from '%s'! Game will be closed.", file ));
	}
}

void Level::CreateBlankScene()
{
	scene = ruCreateSceneNode();
}

void Level::BuildPath( Path & path, const char * nodeBaseName )
{
	path.BuildPath( scene, nodeBaseName );
}
