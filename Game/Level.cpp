#include "Precompiled.h"

#include "Level.h"
#include "LevelArrival.h"
#include "LevelMine.h"
#include "GUIProperties.h"
#include "Player.h"
#include "Menu.h"
#include "LevelResearchFacility.h"
#include "TestingChamber.h"
#include "LevelIntroduction.h"
#include "SaveLoader.h"
#include "SaveWriter.h"
#include "LevelSewers.h"
#include "LevelCutsceneIntro.h"

shared_ptr<ruText> Level::msGUILoadingText;
shared_ptr<ruRect> Level::msGUILoadingBackground;
shared_ptr<ruFont> Level::msGUIFont;

int g_initialLevel;
Level * pCurrentLevel = 0;
int Level::msCurLevelID = 0;

Level::Level() {
    mInitializationComplete = false;
    mTypeNum = 0; //undefined
}

Level::~Level() {
    for( auto pSheet : mSheetList ) {
        delete pSheet;
    }
	for( auto pButton : mButtonList ) {
		delete pButton;
	}
}

void Level::LoadLocalization( string fn ) {
    mLocalization.ParseFile( localizationPath + fn );
}

void Level::Hide() {
    mScene->Hide();
    for( auto & sWeak : mSounds ) {
		shared_ptr<ruSound> & sound = sWeak.lock();
		if( sound ) {
			sound->Pause();
		}
    }
	for( auto pLamp : mLampList ) {
		pLamp->Hide();
	}
}

void Level::Show() {
    mScene->Show();
    for( auto & sWeak : mSounds ) {
		shared_ptr<ruSound> & sound = sWeak.lock();
		if( sound ) {
			if( sound->IsPaused() ) {
				sound->Play();
			}
        }
    }
	for( auto pLamp : mLampList ) {
		pLamp->Show();
	}
}

void Level::Change( int levelId, bool continueFromSave ) {
	static int lastLevel = 0;

    Level::msCurLevelID = levelId;
		
	/////////////////////////////////////////////////////
	// draw 'loading' string
	msGUILoadingText->SetVisible( true );
	msGUILoadingBackground->SetVisible( true );
	if( pPlayer ) {
		pPlayer->SetHUDVisible( false );
	}
    
    ruEngine::RenderWorld();

	msGUILoadingText->SetVisible( false );
	msGUILoadingBackground->SetVisible( false );

	/////////////////////////////////////////////////////
	// before load new level, we must clean up everything

	// items can be transfered through levels, so store it
	map<Item,int> items;

	float playerHealth = 100.0f;

	vector<UsableObject*> playerUsableObjects;
	
	if( pPlayer ) {
		if( lastLevel != levelId ) {
			pPlayer->DumpUsableObjects( playerUsableObjects );
		}
		pPlayer->GetInventory()->GetItems( items );
		playerHealth = pPlayer->GetHealth();
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
	if( Level::msCurLevelID != LevelName::L0Introduction && Level::msCurLevelID != LevelName::LCSIntro ) {
		pPlayer = new Player();
		pPlayer->SetHealth( playerHealth );
	}

	// and now we can load new level
	switch( Level::msCurLevelID )
	{
	case LevelName::LCSIntro:
		pCurrentLevel = new LevelCutsceneIntro;
		break;
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
		for( auto uo : playerUsableObjects ) {
			pPlayer->AddUsableObject( uo );
		}
		if( items.size() ) {
			pPlayer->GetInventory()->SetItems( items );		
		}
	}
	
	lastLevel = levelId;

	Game_UpdateClock(); 
}

void Level::AddLift( const shared_ptr<Lift> & lift ) {
    mLiftList.push_back( lift );
}

void Level::AddValve( const shared_ptr<Valve> & valve ) {
    mValveList.push_back( valve );
}

void Level::AddLadder( const shared_ptr<Ladder> & ladder ) {
    mLadderList.push_back( ladder );
}

void Level::AddDoor( const shared_ptr<Door> & door ) {
	for( auto iDoor : mDoorList ) {
		if( iDoor->mDoorNode == door->mDoorNode ) {
			return;
		}
	}
    mDoorList.push_back( door );
}

void Level::AddSheet( Sheet * pSheet ) {
    mSheetList.push_back( pSheet );
}

void Level::AddItemPlace( const shared_ptr<ItemPlace> & pItemPlace ) {
    mItemPlaceList.push_back( pItemPlace );
}

void Level::Deserialize( SaveFile & in ) {
    int childCount = in.ReadInteger( );
    for( int i = 0; i < childCount; i++ ) {
        string name = in.ReadString();
        shared_ptr<ruSceneNode> node = mScene->FindChild( name );
		ruVector3 pos = in.ReadVector3();
		ruQuaternion quat = in.ReadQuaternion();
		bool visible = in.ReadBoolean();
		bool isLight = in.ReadBoolean();
		float litRange = 0.0f;
		if( isLight ) {
			litRange = in.ReadFloat();
		}
        if( node ) {
            node->SetLocalPosition( pos );
            node->SetLocalRotation( quat );           
            if( visible ) {
                node->Show();
            } else {
                node->Hide();
            }
			if( isLight ) {
				std::dynamic_pointer_cast<ruLight>( node )->SetRange( litRange );
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

	for( auto pLift : mLiftList ) {
		pLift->Deserialize( in );
	}

    OnDeserialize( in );
}

void Level::Serialize( SaveFile & out ) {
    int childCount = mScene->GetCountChildren();
    out.WriteInteger( childCount );
    for( int i = 0; i < childCount; i++ ) {
        shared_ptr<ruSceneNode> node = mScene->GetChild( i );
        out.WriteString( node->GetName() );
        out.WriteVector3( node->GetLocalPosition() );
        out.WriteQuaternion( node->GetLocalRotation());
        out.WriteBoolean( node->IsVisible() );
		shared_ptr<ruLight>light = std::dynamic_pointer_cast<ruLight>( node );
		out.WriteBoolean( light != nullptr );
		if( light ) {
			out.WriteFloat( light->GetRange() );
		}
    }
    out.WriteInteger( mStages.size());
    for( auto stage : mStages ) {
        out.WriteString( stage.first );
        out.WriteBoolean( stage.second );
    }

	out.WriteInteger( Door::msDoorList.size() );
	for( auto pDoor : Door::msDoorList ) {
		out.WriteString( pDoor->mDoorNode->GetName() );
		pDoor->Serialize( out );
	}

	for( auto pLift : mLiftList ) {
		pLift->Serialize( out );
	}

    OnSerialize( out );
}

void Level::AddSound( shared_ptr<ruSound> sound ) {
    if( !sound ) {
        throw std::runtime_error( "Unable to add ambient sound! Invalid source!" );
    }
    mSounds.push_back( sound );
}

void Level::PlayAmbientSounds() {
    mAmbSoundSet.DoRandomPlaying();
}

void Level::AddAmbientSound( shared_ptr<ruSound> sound ) {
    if( !sound ) {
        throw std::runtime_error( "Unable to add ambient sound! Invalid source!" );
    }
    mSounds.push_back( sound );
    mAmbSoundSet.AddSound( sound );
}

shared_ptr<ruSceneNode> Level::GetUniqueObject( const string & name ) {
    // the point of this behaviour is to reduce number of possible errors during runtime, if some object doesn't exist in the scene( but it must )
    // game notify user on level loading stage, but not in the game. So this feature is very useful for debugging purposes
    // also this feature can help to improve some performance by reducing FindXXX calls, which take a lot of time
    if( mInitializationComplete ) {
        throw std::runtime_error( StringBuilder( "You must get object in game level initialization! Get object in game logic loop is strictly forbidden! Object name: " ) << name );
    }
    if( !mScene ) {
        throw std::runtime_error( StringBuilder( "Object " ) << name << " can't be found in the empty scene. Load scene first!" );
    }
    shared_ptr<ruSceneNode> object = mScene->FindChild( name );
    // each unique object must be presented in the scene, otherwise error will be generated
    if( !object ) {
        throw std::runtime_error( StringBuilder( "Object " ) << name << " can't be found in the scene! Game will be closed." );
    }
    return object;
}

void Level::LoadSceneFromFile( const string & file ) {
    mScene = ruSceneNode::LoadFromFile( file );
    if( !mScene ) {
        throw std::runtime_error( StringBuilder( "Unable to load scene from " ) << file << "! Game will be closed." );
    }
}

void Level::CreateBlankScene() {
    mScene = ruSceneNode::Create();
}

void Level::BuildPath( Path & path, const string & nodeBaseName ) {
    path.BuildPath( mScene, nodeBaseName );
}

void Level::DoneInitialization() {
    mInitializationComplete = true;
}

void Level::CreateLoadingScreen()
{
	msGUIFont = ruFont::LoadFromFile( 32, "data/fonts/font1.otf" );
	int w = 200;
	int h = 32;
	int x = ( ruVirtualScreenWidth - w ) / 2;
	int y = ( ruVirtualScreenHeight - h ) / 2;
	msGUILoadingText = ruText::Create( "Загрузка...", x, y, w, h, msGUIFont, ruVector3( 0, 0, 0 ), ruTextAlignment::Center );
	msGUILoadingText->SetVisible( false );
	msGUILoadingBackground = ruRect::Create( 0, 0, ruVirtualScreenWidth, ruVirtualScreenHeight, ruTexture::Request( "data/gui/loadingscreen.tga" ), pGUIProp->mBackColor );
	msGUILoadingBackground->SetVisible( false );
}

void Level::AddLamp( const shared_ptr<Lamp> & lamp ) {
	mLampList.push_back( lamp );
}

void Level::UpdateGenericObjectsIdle() {
	if( mMusic ) {
		mMusic->SetVolume( pMainMenu->GetMusicVolume() );
	}

	for( auto pLamp : mLampList ) {
		pLamp->Update();
	}
	for( auto pZone : mZoneList ) {
		pZone->Update();
	}
	for( auto pButton : mButtonList ) {
		pButton->Update();
	}
}

void Level::AutoCreateLampsByNamePattern( const string & namePattern, string buzzSound ) {
	std::regex rx( namePattern );
	for( int i = 0; i < mScene->GetCountChildren(); i++ ) {
		shared_ptr<ruSceneNode> child = mScene->GetChild( i );
		if( regex_match( child->GetName(), rx )) {
			AddLamp( make_shared<Lamp>( child, ruSound::Load3D( buzzSound )));
		}
	}
}

void Level::AutoCreateBulletsByNamePattern( const string & namePattern ) {
	std::regex rx( namePattern );
	for( int i = 0; i < mScene->GetCountChildren(); i++ ) {
		shared_ptr<ruSceneNode> child = mScene->GetChild( i );
		if( regex_match( child->GetName(), rx )) {
			AddInteractiveObject( Item::GetNameByType( Item::Type::Bullet ), make_shared<InteractiveObject>( child ), ruDelegate::Bind( this, &Level::Proxy_GiveBullet ));
		}
	}
}


void Level::AutoCreateDoorsByNamePattern( const string & namePattern ) {
	std::regex rx( namePattern );
	for( int i = 0; i < mScene->GetCountChildren(); i++ ) {
		shared_ptr<ruSceneNode> child = mScene->GetChild( i );
		bool ignore = false;
		for( auto pDoor : mDoorList ) {
			if( pDoor->mDoorNode == child ) {
				ignore = true;
				break;
			}
		}
		if( !ignore ) {
			if( regex_match( child->GetName(), rx )) {
				AddDoor( make_shared<Door>( child, 90 ));
			}
		}
	}
}

void Level::AddZone( const shared_ptr<Zone> & zone ) {
	mZoneList.push_back( zone );
}

void Level::AddButton( Button * button ) {
	mButtonList.push_back( button );
}

void Level::AddInteractiveObject( const string & desc, const shared_ptr<InteractiveObject> & io, const ruDelegate & interactAction ) {
	io->OnInteract.AddListener( interactAction );
	io->SetPickDescription( desc );
	mInteractiveObjectList.push_back( io );
}

void Level::DeserializeAnimation( SaveFile & in, ruAnimation & anim )
{
	anim.SetCurrentFrame( in.ReadInteger());
	anim.SetEnabled( in.ReadBoolean( ));
}

void Level::SerializeAnimation( SaveFile & out, ruAnimation & anim )
{
	out.WriteInteger( anim.GetCurrentFrame() );
	out.WriteBoolean( anim.IsEnabled() );
}

void Level::Proxy_GiveBullet()
{
	pPlayer->AddItem( Item::Type::Bullet );
}
