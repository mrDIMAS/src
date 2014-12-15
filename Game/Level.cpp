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
int Level::msCurLevelID = 0;

Level::Level()
{
	mInitializationComplete = false;
    mTypeNum = 0; //undefined
    mMusic.Invalidate();
}

Level::~Level()
{
    ruFreeSceneNode( mScene );

    for( auto pItem : mItemList )
        if( pItem->IsFree() )
            delete pItem;

    for( auto pSheet : mSheetList )
        delete pSheet;

    for( auto pDoor : mDoorList )
        delete pDoor;

    for( auto pCrawlWay : mCrawlWayList )
        delete pCrawlWay;

    for( auto pLadder : mLadderList )
        delete pLadder;

    for( auto pItemPlace : mItemPlaceList )
        delete pItemPlace;

    for( auto pValve : mValveList )
        delete pValve;

    for( auto pLift : mLiftList )
        delete pLift;

    for( auto iSound : mSounds )
        ruFreeSound( iSound );
}

void Level::LoadLocalization( string fn )
{
    mLocalization.ParseFile( localizationPath + fn );
}

void Level::Hide()
{
    ruHideNode( mScene );
    for( auto & sound : mSounds )
        ruPauseSound( sound );
}

void Level::Show()
{
    ruShowNode( mScene );
    for( auto & sound : mSounds )
    {
        if( ruIsSoundPaused( sound ))
            ruPlaySound( sound );
    }
}

void Level::Change( int levelId, bool continueFromSave )
{
    Level::msCurLevelID = levelId;

    static int lastLevel = 0;

    if( lastLevel != Level::msCurLevelID )
    {
       // ruDrawGUIText( pMainMenu->mLocalization.GetString( "loading" ), ruGetResolutionWidth() / 2 - 64, ruGetResolutionHeight() / 2 - 64, 128, 128, pGUI->mFont, ruVector3( 255, 255, 0), 1 );

        // draw 'loading' string
        ruRenderWorld( 1.0f / 60.0f );

        lastLevel = Level::msCurLevelID;

        if( !pPlayer && Level::msCurLevelID != LevelName::L0Introduction )
            pPlayer = new Player();

        if( pCurrentLevel )
            delete pCurrentLevel;

        if( Level::msCurLevelID == LevelName::L0Introduction )
            pCurrentLevel = new LevelIntroduction;
        else
            pPlayer->FreeHands();

        if( Level::msCurLevelID == LevelName::L1Arrival )
            pCurrentLevel = new LevelArrival;

        if( Level::msCurLevelID == LevelName::L2Mine )
            pCurrentLevel = new LevelMine;

        if( Level::msCurLevelID == LevelName::L3ResearchFacility )
            pCurrentLevel = new LevelResearchFacility;

        if( Level::msCurLevelID == LevelName::LXTestingChamber )
            pCurrentLevel = new TestingChamber;

        if( continueFromSave ) {
            SaveLoader( "lastGame.save" ).RestoreWorldState();
		}
        for( int i = 0; i < ruGetWorldPointLightCount(); i++ )
        {
            ruSetLightFloatingEnabled( ruGetWorldPointLight( i ), true );
            float d = 0.1f;
            ruSetLightFloatingLimits( ruGetWorldPointLight( i ), ruVector3( -d, -d, -d ), ruVector3( d, d, d ) );
        }
    }
}

void Level::AddLift( Lift * pLift )
{
    mLiftList.push_back( pLift );
}

void Level::AddValve( Valve * pValve )
{
    mValveList.push_back( pValve );
}

void Level::AddLadder( Ladder * pLadder )
{
    mLadderList.push_back( pLadder );
}

void Level::AddCrawlWay( CrawlWay * pCrawlWay )
{
    mCrawlWayList.push_back( pCrawlWay );
}

void Level::AddDoor( Door * pDoor )
{
    mDoorList.push_back( pDoor );
}

void Level::AddSheet( Sheet * pSheet )
{
    mSheetList.push_back( pSheet );
}

void Level::AddItem( Item * item )
{
    mItemList.push_back( item );
}

void Level::AddItemPlace( ItemPlace * pItemPlace )
{
    mItemPlaceList.push_back( pItemPlace );
}

void Level::DeserializeWith( TextFileStream & in )
{
    int childCount = in.ReadInteger( );
    for( int i = 0; i < childCount; i++ )
    {
        string name = in.Readstring();
        ruNodeHandle node = ruFindInObjectByName( mScene, name.c_str() );
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
        mStages[ stageName ] = stageState;
    }
    OnDeserialize( in );
}

void Level::SerializeWith( TextFileStream & out )
{
    int childCount = ruGetNodeCountChildren( mScene );
    out.WriteInteger( childCount );
    for( int i = 0; i < childCount; i++ )
    {
        ruNodeHandle node = ruGetNodeChild( mScene, i );
        out.WriteString( ruGetNodeName( node ));
        out.WriteVector3( ruGetNodeLocalPosition( node ));
        out.WriteQuaternion( ruGetNodeLocalRotation( node ));
        out.WriteBoolean( ruIsNodeVisible( node ));
    }
    out.WriteInteger( mStages.size());
    for( auto stage : mStages )
    {
        out.WriteString( stage.first );
        out.WriteBoolean( stage.second );
    }
    OnSerialize( out );
}

void Level::AddSound( ruSoundHandle sound )
{
	if( !sound.IsValid() ) {
		RaiseError( "Unable to add ambient sound! Invalid source!" );
	}
    mSounds.push_back( sound );
}

void Level::PlayAmbientSounds()
{
    mAmbSoundSet.DoRandomPlaying();
}

void Level::AddAmbientSound( ruSoundHandle sound )
{
	if( !sound.IsValid() ) {
		RaiseError( "Unable to add ambient sound! Invalid source!" );
	}
    mSounds.push_back( sound );
    mAmbSoundSet.AddSound( sound );
}

ruNodeHandle Level::GetUniqueObject( const char * name )
{
	// the point of this behaviour is to reduce number of possible errors during runtime, if some object doesn't exist in the scene( but it must )
	// game notify user on level loading stage, but not in the game. So this feature is very useful for debugging purposes
	// also this feature can help to improve some performance by reducing FindXXX calls, which take a lot of time
	if( mInitializationComplete ) {
		RaiseError( Format( "You must get object in game level intialization! Get object in game logic loop is strictly forbidden! Object name: '%s'", name ));
	}
	if( !mScene.IsValid() ) {
		RaiseError( Format( "Object '%s' can't be found in the empty scene. Load scene first!", name ));
	}
	ruNodeHandle object = ruFindInObjectByName( mScene, name );
	// each unique object must be presented in the scene, otherwise error will be generated
	if( !object.IsValid() ) {
		RaiseError( Format( "Object '%s' can't be found in the scene! Game will be closed.", name ));
	}
	return object;
}

void Level::LoadSceneFromFile( const char * file )
{
	mScene = ruLoadScene( file );
	if( !mScene.IsValid() ) {
		RaiseError( Format( "Unable to load scene from '%s'! Game will be closed.", file ));
	}
}

void Level::CreateBlankScene()
{
	mScene = ruCreateSceneNode();
}

void Level::BuildPath( Path & path, const char * nodeBaseName )
{
	path.BuildPath( mScene, nodeBaseName );
}

void Level::DoneInitialization()
{
	mInitializationComplete = true;
}
