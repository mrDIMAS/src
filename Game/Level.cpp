#include "Precompiled.h"

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
#include "LevelSewers.h"

ruTextHandle Level::msGUILoadingText;
ruRectHandle Level::msGUILoadingBackground;
ruFontHandle Level::msGUIFont;

int g_initialLevel;
Level * pCurrentLevel = 0;
int Level::msCurLevelID = 0;

Level::Level() {
    mInitializationComplete = false;
    mTypeNum = 0; //undefined
    mMusic.Invalidate();
}

Level::~Level() {
    ruFreeSceneNode( mScene );

    for( auto pItem : mItemList ) {
        if( pItem->IsFree() ) {
            delete pItem;
        }
	}

    for( auto pSheet : mSheetList ) {
        delete pSheet;
    }

    for( auto pDoor : mDoorList ) {
        delete pDoor;
    }

    for( auto pLadder : mLadderList ) {
        delete pLadder;
    }

    for( auto pItemPlace : mItemPlaceList ) {
        delete pItemPlace;
    }

    for( auto pValve : mValveList ) {
        delete pValve;
    }

    for( auto pLift : mLiftList ) {
        delete pLift;
    }

	for( auto pLamp : mLampList ) {
		delete pLamp;
	}

    for( auto iSound : mSounds ) {
        ruFreeSound( iSound );
    }

	ruFreeSound( mMusic );
}

void Level::LoadLocalization( string fn ) {
    mLocalization.ParseFile( localizationPath + fn );
}

void Level::Hide() {
    ruHideNode( mScene );
    for( auto & sound : mSounds ) {
        ruPauseSound( sound );
    }
	for( auto pLamp : mLampList ) {
		pLamp->Hide();
	}
}

void Level::Show() {
    ruShowNode( mScene );
    for( auto & sound : mSounds ) {
        if( ruIsSoundPaused( sound )) {
            ruPlaySound( sound );
        }
    }
	for( auto pLamp : mLampList ) {
		pLamp->Show();
	}
}

struct WeaponTransfer {
	Weapon::Type mType;
	int projCount;
	WeaponTransfer( Weapon::Type type, int prc ) {
		mType = type;
		projCount = prc;
	}
};

void Level::Change( int levelId, bool continueFromSave ) {
	static int lastLevel = 0;

    Level::msCurLevelID = levelId;
		
	/////////////////////////////////////////////////////
	// draw 'loading' string
	ruSetGUINodeVisible( msGUILoadingText, true );
	ruSetGUINodeVisible( msGUILoadingBackground, true );
	if( pPlayer ) {
		pPlayer->SetHUDVisible( false );
	}
    
    ruRenderWorld();

	ruSetGUINodeVisible( msGUILoadingText, false );
	ruSetGUINodeVisible( msGUILoadingBackground, false );

	/////////////////////////////////////////////////////
	// before load new level, we must clean up everything

	// items can be transfered through levels, so store it
	vector<Item::Type> items;
	// weapons
	vector<WeaponTransfer> weapons;
		
	if( pPlayer ) {
		pPlayer->GetInventory()->GetItemList( items );
		for( auto pWeapon : pPlayer->mWeaponList ) {
			weapons.push_back( WeaponTransfer( pWeapon->GetType(), pWeapon->GetProjectileCount()));
		}
		delete pPlayer;
		pPlayer = nullptr;
	}

	if( pCurrentLevel ) {
		delete pCurrentLevel;
		pCurrentLevel = nullptr;
	}

	while( Door::msDoorList.size() ) {
		delete Door::msDoorList.front();
	}
	Door::msDoorList.clear();

	while( Way::msWayList.size() ) {
		delete Way::msWayList.front();
	}
	Way::msWayList.clear();

	while( Enemy::msEnemyList.size() ) {
		delete Enemy::msEnemyList.front();
	}
	Enemy::msEnemyList.clear();

	while( Item::msItemList.size() ) {
		delete Item::msItemList.front();
	}
	Item::msItemList.clear();

	while( InteractiveObject::msObjectList.size() ) {
		delete InteractiveObject::msObjectList.front();
	}
	InteractiveObject::msObjectList.clear();

	while( Sheet::msSheetList.size() ) {
		delete Sheet::msSheetList.front();
	}
	Sheet::msSheetList.clear();

	while( ItemPlace::sItemPlaceList .size() ) {
		delete ItemPlace::sItemPlaceList .front();
	}
	ItemPlace::sItemPlaceList.clear();	

	/////////////////////////////////////////////////////
	
	// create player
	if( Level::msCurLevelID != LevelName::L0Introduction ) {
		pPlayer = new Player();
	}

	// and now we can load new level
	switch( Level::msCurLevelID )
	{
	case LevelName::L0Introduction:
		pCurrentLevel = new LevelIntroduction;
		break;
	case LevelName::L1Arrival:
		pCurrentLevel = new LevelArrival;
		break;
	case LevelName::L2Mine:
		pCurrentLevel = new LevelMine;
		break;
	case LevelName::L3ResearchFacility:
		pCurrentLevel = new LevelResearchFacility;
		break;
	case LevelName::L4Sewers:
		pCurrentLevel = new LevelSewers;
		break;
	case LevelName::LXTestingChamber:
		pCurrentLevel = new TestingChamber;
		break;
	default:
		break;
	}

    if( continueFromSave ) {
        SaveLoader( "lastGame.save" ).RestoreWorldState();
    }

	// only if level is changed to another
	if( lastLevel != levelId ) {
		// after loading, give old items to the player
		for( auto itemType : items ) {
			if( itemType != Item::Type::Lighter ) { // lighter automatically added to player
				pPlayer->AddItem( new Item( itemType ));
			}
		}
		// weapons
		for( auto wpnTransfer : weapons ) {
			Weapon * pWpn = pPlayer->AddWeapon( wpnTransfer.mType );
			pWpn->SetProjectileCount( wpnTransfer.projCount );
		}
	}

	lastLevel = levelId;
}

void Level::AddLift( Lift * pLift ) {
    mLiftList.push_back( pLift );
}

void Level::AddValve( Valve * pValve ) {
    mValveList.push_back( pValve );
}

void Level::AddLadder( Ladder * pLadder ) {
    mLadderList.push_back( pLadder );
}

void Level::AddDoor( Door * pDoor ) {
	for( auto iDoor : mDoorList ) {
		if( iDoor->mDoorNode == pDoor->mDoorNode ) {
			return;
		}
	}
    mDoorList.push_back( pDoor );
}

void Level::AddSheet( Sheet * pSheet ) {
    mSheetList.push_back( pSheet );
}

void Level::AddItem( Item * item ) {
    mItemList.push_back( item );
}

void Level::AddItemPlace( ItemPlace * pItemPlace ) {
    mItemPlaceList.push_back( pItemPlace );
}

void Level::Deserialize( TextFileStream & in ) {
    int childCount = in.ReadInteger( );
    for( int i = 0; i < childCount; i++ ) {
        string name = in.ReadString();
        ruNodeHandle node = ruFindInObjectByName( mScene, name );
		ruVector3 pos = in.ReadVector3();
		ruQuaternion quat = in.ReadQuaternion();
		bool visible = in.ReadBoolean();
		bool isLight = in.ReadBoolean();
		float litRange = 0.0f;
		if( isLight ) {
			litRange = in.ReadFloat();
		}
        if( node.IsValid() ) {
            ruSetNodeLocalPosition( node, pos );
            ruSetNodeLocalRotation( node, quat );           
            if( visible ) {
                ruShowNode( node );
            } else {
                ruHideNode( node );
            }
			if( isLight ) {
				ruSetLightRange( node, litRange );
				ruSetLightFloatingEnabled( node, true );
				float d = 0.1f;
				ruSetLightFloatingLimits( node, ruVector3( -d, -d, -d ), ruVector3( d, d, d ) );
			}
        }
    }
    int countStages = in.ReadInteger();
    for( int i = 0; i < countStages; i++ ) {
        string stageName = in.ReadString();
        bool stageState = in.ReadBoolean();
        mStages[ stageName ] = stageState;
    }
	int doorCount = in.ReadInteger();
	for( int i = 0; i < doorCount; i++ ) {
		Door * pDoor = Door::GetByName( in.ReadString() );
		if( pDoor ) {
			pDoor->Deserialize( in );
		}
	}
    OnDeserialize( in );
}

void Level::Serialize( TextFileStream & out ) {
    int childCount = ruGetNodeCountChildren( mScene );
    out.WriteInteger( childCount );
    for( int i = 0; i < childCount; i++ ) {
        ruNodeHandle node = ruGetNodeChild( mScene, i );
        out.WriteString( ruGetNodeName( node ));
        out.WriteVector3( ruGetNodeLocalPosition( node ));
        out.WriteQuaternion( ruGetNodeLocalRotation( node ));
        out.WriteBoolean( ruIsNodeVisible( node ));
		out.WriteBoolean( ruIsLight( node ) );
		if( ruIsLight( node )) {
			out.WriteFloat( ruGetLightRange( node ));
		}
    }
    out.WriteInteger( mStages.size());
    for( auto stage : mStages ) {
        out.WriteString( stage.first );
        out.WriteBoolean( stage.second );
    }

	out.WriteInteger( Door::msDoorList.size() );
	for( auto pDoor : Door::msDoorList ) {
		out.WriteString( ruGetNodeName( pDoor->mDoorNode ) );
		pDoor->Serialize( out );
	}
    OnSerialize( out );
}

void Level::AddSound( ruSoundHandle sound ) {
    if( !sound.IsValid() ) {
        RaiseError( "Unable to add ambient sound! Invalid source!" );
    }
    mSounds.push_back( sound );
}

void Level::PlayAmbientSounds() {
    mAmbSoundSet.DoRandomPlaying();
}

void Level::AddAmbientSound( ruSoundHandle sound ) {
    if( !sound.IsValid() ) {
        RaiseError( "Unable to add ambient sound! Invalid source!" );
    }
    mSounds.push_back( sound );
    mAmbSoundSet.AddSound( sound );
}

ruNodeHandle Level::GetUniqueObject( const string & name ) {
    // the point of this behaviour is to reduce number of possible errors during runtime, if some object doesn't exist in the scene( but it must )
    // game notify user on level loading stage, but not in the game. So this feature is very useful for debugging purposes
    // also this feature can help to improve some performance by reducing FindXXX calls, which take a lot of time
    if( mInitializationComplete ) {
        RaiseError( StringBuilder( "You must get object in game level intialization! Get object in game logic loop is strictly forbidden! Object name: " ) << name );
    }
    if( !mScene.IsValid() ) {
        RaiseError( StringBuilder( "Object " ) << name << " can't be found in the empty scene. Load scene first!" );
    }
    ruNodeHandle object = ruFindInObjectByName( mScene, name );
    // each unique object must be presented in the scene, otherwise error will be generated
    if( !object.IsValid() ) {
        RaiseError( StringBuilder( "Object " ) << name << " can't be found in the scene! Game will be closed." );
    }
    return object;
}

void Level::LoadSceneFromFile( const string & file ) {
    mScene = ruLoadScene( file );
    if( !mScene.IsValid() ) {
        RaiseError( StringBuilder( "Unable to load scene from " ) << file << "! Game will be closed." );
    }
}

void Level::CreateBlankScene() {
    mScene = ruCreateSceneNode();
}

void Level::BuildPath( Path & path, const string & nodeBaseName ) {
    path.BuildPath( mScene, nodeBaseName );
}

void Level::DoneInitialization() {
    mInitializationComplete = true;
}

void Level::CreateLoadingScreen()
{
	msGUIFont = ruCreateGUIFont( 32, "data/fonts/font1.otf" );
	int w = 200;
	int h = 32;
	int x = ( ruGetResolutionWidth() - w ) / 2;
	int y = ( ruGetResolutionHeight() - h ) / 2;
	msGUILoadingText = ruCreateGUIText( "Загрузка...", x, y, w, h, msGUIFont, ruVector3( 0, 0, 0 ), 1 );
	ruSetGUINodeVisible( msGUILoadingText, false );
	msGUILoadingBackground = ruCreateGUIRect( 0, 0, ruGetResolutionWidth(), ruGetResolutionHeight(), ruGetTexture( "data/textures/generic/loadingScreen.jpg" ));
	ruSetGUINodeVisible( msGUILoadingBackground, false );
}

void Level::AddLamp( Lamp * lamp )
{
	mLampList.push_back( lamp );
}

void Level::UpdateGenericObjectsIdle()
{
	for( auto pLamp : mLampList ) {
		pLamp->Update();
	}
	for( auto pZone : mZoneList ) {
		pZone->Update();
	}
}

void Level::AutoCreateLampsByNamePattern( const string & namePattern, string buzzSound )
{
	std::regex rx( namePattern );
	for( int i = 0; i < ruGetNodeCountChildren( mScene ); i++ ) {
		ruNodeHandle child = ruGetNodeChild( mScene, i );
		if( regex_match( ruGetNodeName( child ), rx )) {
			AddLamp( new Lamp( child, ruLoadSound3D( buzzSound )));
		}
	}
}

void Level::AutoCreateBulletsByNamePattern( const string & namePattern ) {
	std::regex rx( namePattern );
	for( int i = 0; i < ruGetNodeCountChildren( mScene ); i++ ) {
		ruNodeHandle child = ruGetNodeChild( mScene, i );
		if( regex_match( ruGetNodeName( child ), rx )) {
			AddItem( new Item( child, Item::Type::Bullet ));
		}
	}
}


void Level::AutoCreateDoorsByNamePattern( const string & namePattern ) {
	std::regex rx( namePattern );
	for( int i = 0; i < ruGetNodeCountChildren( mScene ); i++ ) {
		ruNodeHandle child = ruGetNodeChild( mScene, i );
		bool ignore = false;
		for( auto pDoor : mDoorList ) {
			if( pDoor->mDoorNode == child ) {
				ignore = true;
				break;
			}
		}
		if( !ignore ) {
			if( regex_match( ruGetNodeName( child ), rx )) {
				AddDoor( new Door( child, 90 ));
			}
		}
	}
}

void Level::AddZone( Zone * zone )
{
	mZoneList.push_back( zone );
}
