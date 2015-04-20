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

void Level::Change( int levelId, bool continueFromSave ) {
    Level::msCurLevelID = levelId;

    static int lastLevel = 0;

    if( lastLevel != Level::msCurLevelID ) {
		ruSetGUINodeVisible( msGUILoadingText, true );
		ruSetGUINodeVisible( msGUILoadingBackground, true );
		if( pPlayer ) {
			pPlayer->SetHUDVisible( false );
		}
        // draw 'loading' string
        ruRenderWorld();
		ruSetGUINodeVisible( msGUILoadingText, false );
		ruSetGUINodeVisible( msGUILoadingBackground, false );
		if( pPlayer ) {
			pPlayer->SetHUDVisible( true );
		}
        lastLevel = Level::msCurLevelID;

        if( !pPlayer && Level::msCurLevelID != LevelName::L0Introduction ) {
            pPlayer = new Player();
        }

        if( pCurrentLevel ) {
            delete pCurrentLevel;
        }

        if( Level::msCurLevelID == LevelName::L0Introduction ) {
            pCurrentLevel = new LevelIntroduction;
        } else {
            pPlayer->FreeHands();
        }

        if( Level::msCurLevelID == LevelName::L1Arrival ) {
            pCurrentLevel = new LevelArrival;
        }

        if( Level::msCurLevelID == LevelName::L2Mine ) {
            pCurrentLevel = new LevelMine;
        }

        if( Level::msCurLevelID == LevelName::L3ResearchFacility ) {
            pCurrentLevel = new LevelResearchFacility;
        }

		if( Level::msCurLevelID == LevelName::L4Sewers ) {
			pCurrentLevel = new LevelSewers;
		}

        if( Level::msCurLevelID == LevelName::LXTestingChamber ) {
            pCurrentLevel = new TestingChamber;
        }

		if( pPlayer ) {
			pPlayer->RepairInventory();
		}

        if( continueFromSave ) {
            SaveLoader( "lastGame.save" ).RestoreWorldState();
        }
        for( int i = 0; i < ruGetWorldPointLightCount(); i++ ) {
			ruNodeHandle light = ruGetWorldPointLight( i );
			if( !(light == pPlayer->mpFlashlight->GetLight() )) {
				ruSetLightFloatingEnabled( light, true );
				float d = 0.1f;
				ruSetLightFloatingLimits( light, ruVector3( -d, -d, -d ), ruVector3( d, d, d ) );
			}
        }
    }
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

void Level::DeserializeWith( TextFileStream & in ) {
    int childCount = in.ReadInteger( );
    for( int i = 0; i < childCount; i++ ) {
        string name = in.ReadString();
        ruNodeHandle node = ruFindInObjectByName( mScene, name );
		ruVector3 pos = in.ReadVector3();
		ruQuaternion quat = in.ReadQuaternion();
		bool visible = in.ReadBoolean();
        if( node.IsValid() ) {
            ruSetNodeLocalPosition( node, pos );
            ruSetNodeLocalRotation( node, quat );           
            if( visible ) {
                ruShowNode( node );
            } else {
                ruHideNode( node );
            }
        }
    }
    int countStages = in.ReadInteger();
    for( int i = 0; i < countStages; i++ ) {
        string stageName = in.ReadString();
        bool stageState = in.ReadBoolean();
        mStages[ stageName ] = stageState;
    }
	int pointLightCount = in.ReadInteger();
	for( int i = 0; i < pointLightCount; i++ ) {
		ruNodeHandle lit = ruGetWorldPointLight( i );
		if( lit.IsValid() ) {
			ruSetLightRange( lit, in.ReadFloat() );
		} else {
			// ignore
			in.ReadFloat();
		}
	}
	int spotLightCount = in.ReadInteger();
	for( int i = 0; i < spotLightCount; i++ ) {
		ruNodeHandle spot = ruGetWorldSpotLight( i );
		if( spot.IsValid() ) {
			ruSetLightRange( spot, in.ReadFloat() );
		} else {
			// ignore
			in.ReadFloat();
		}
	}
    OnDeserialize( in );
}

void Level::SerializeWith( TextFileStream & out ) {
    int childCount = ruGetNodeCountChildren( mScene );
    out.WriteInteger( childCount );
    for( int i = 0; i < childCount; i++ ) {
        ruNodeHandle node = ruGetNodeChild( mScene, i );
        out.WriteString( ruGetNodeName( node ));
        out.WriteVector3( ruGetNodeLocalPosition( node ));
        out.WriteQuaternion( ruGetNodeLocalRotation( node ));
        out.WriteBoolean( ruIsNodeVisible( node ));
    }
    out.WriteInteger( mStages.size());
    for( auto stage : mStages ) {
        out.WriteString( stage.first );
        out.WriteBoolean( stage.second );
    }
	out.WriteInteger( ruGetWorldPointLightCount() );
	for( int i = 0; i < ruGetWorldPointLightCount(); i++ ) {
		out.WriteFloat( ruGetLightRange( ruGetWorldPointLight( i )));
	}
	out.WriteInteger( ruGetWorldSpotLightCount() );
	for( int i = 0; i < ruGetWorldSpotLightCount(); i++ ) {
		out.WriteFloat( ruGetLightRange( ruGetWorldSpotLight( i )));
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
	msGUIFont = ruCreateGUIFont( 32, "data/fonts/font1.otf", 0, 0 );
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
