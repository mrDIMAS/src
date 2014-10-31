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
Level * currentLevel = 0;
int Level::curLevelID = 0;

Level::Level() {
    music.Invalidate();
}

Level::~Level() {
    FreeSceneNode( scene );

    for( auto item : items )
        if( !item->inInventory ) {
            delete item;
        }

    for( auto sheet : sheets ) {
        delete sheet;
    }

    for( auto door : doors ) {
        delete door;
    }

    for( auto crawlWay : crawlWays ) {
        delete crawlWay;
    }

    for( auto ladder : ladders ) {
        delete ladder;
    }

    for( auto itemPlace : itemPlaces ) {
        delete itemPlace;
    }

    for( auto valve : valves ) {
        delete valve;
    }

    for( auto lift : lifts ) {
        delete lift;
    }

    for( auto sound : sounds ) {
        FreeSoundSource( sound );
    }
}

void Level::LoadLocalization( string fn ) {
    localization.ParseFile( localizationPath + fn );
}

void Level::Hide() {
    HideNode( scene );

    for( auto & sound : sounds ) {
        PauseSoundSource( sound );
    }
}

void Level::Show() {
    ShowNode( scene );

    for( auto & sound : sounds )
        if( IsSoundPaused( sound )) {
            PlaySoundSource( sound );
        }
}

void Level::Change( int levelId, bool continueFromSave ) {
    Level::curLevelID = levelId;

    static int lastLevel = 0;

    if( lastLevel != Level::curLevelID ) {
        DrawGUIText( menu->loc.GetString( "loading" ), GetResolutionWidth() / 2 - 64, GetResolutionHeight() / 2 - 64, 128, 128, gui->font, Vector3( 255, 255, 0), 1 );

        // draw 'loading' string
        RenderWorld();

        lastLevel = Level::curLevelID;

        if( !player && Level::curLevelID != LevelName::L0Introduction ) {
            player = new Player();
        }

        if( currentLevel ) {
            delete currentLevel;
        }

        if( Level::curLevelID == LevelName::L0Introduction ) {
            currentLevel = new LevelIntroduction;
        } else {
            player->FreeHands();
        }

        if( Level::curLevelID == LevelName::L1Arrival ) {
            currentLevel = new LevelArrival;
        }

        if( Level::curLevelID == LevelName::L2Mine ) {
            currentLevel = new LevelMine;
        }

        if( Level::curLevelID == LevelName::L3ResearchFacility ) {
            currentLevel = new LevelResearchFacility;
        }

        if( Level::curLevelID == LevelName::LXTestingChamber ) {
            currentLevel = new TestingChamber;
        }

        if( continueFromSave ) {
            SaveLoader( "lastGame.save" ).RestoreWorldState();
        } else {
            if( player ) {
                SaveWriter( "quickSave.save" ).SaveWorldState();
            }
        }

        for( int i = 0; i < GetWorldPointLightCount(); i++ ) {
            SetLightFloatingEnabled( GetWorldPointLight( i ), true );
            float d = 0.1f;
            SetLightFloatingLimits( GetWorldPointLight( i ), Vector3( -d, -d, -d ), Vector3( d, d, d ) );
        }
    }
}

void Level::AddLift( Lift * lift ) {
    lifts.push_back( lift );
}

void Level::AddValve( Valve * valve ) {
    valves.push_back( valve );
}

void Level::AddLadder( Ladder * ladder ) {
    ladders.push_back( ladder );
}

void Level::AddCrawlWay( CrawlWay * cw ) {
    crawlWays.push_back( cw );
}

void Level::AddDoor( Door * door ) {
    doors.push_back( door );
}

void Level::AddSheet( Sheet * sheet ) {
    sheets.push_back( sheet );
}

void Level::AddItem( Item * item ) {
    items.push_back( item );
}

void Level::AddItemPlace( ItemPlace * ipc ) {
    itemPlaces.push_back( ipc );
}

void Level::DeserializeWith( TextFileStream & in ) {
    int childCount = in.ReadInteger( );
    for( int i = 0; i < childCount; i++ ) {
        string name = in.Readstring();
        NodeHandle node = FindInObjectByName( scene, name.c_str() );
        SetLocalPosition( node, in.ReadVector3() );
        SetLocalRotation( node, in.ReadQuaternion() );
        bool visible = in.ReadBoolean();
        if( visible ) {
            ShowNode( node );
        } else {
            HideNode( node );
        }
    }
    int countStages = in.ReadInteger();
    for( int i = 0; i < countStages; i++ ) {
        string stageName = in.Readstring();
        bool stageState = in.ReadBoolean();
        stages[ stageName ] = stageState;
    }
    OnDeserialize( in );
}

void Level::SerializeWith( TextFileStream & out ) {
    int childCount = GetCountChildren( scene );
    out.WriteInteger( childCount );
    for( int i = 0; i < childCount; i++ ) {
        NodeHandle node = GetChild( scene, i );
        out.Writestring( GetName( node ));
        out.WriteVector3( GetLocalPosition( node ));
        out.WriteQuaternion( GetLocalRotation( node ));
        out.WriteBoolean( IsNodeVisible( node ));
    }
    out.WriteInteger( stages.size());
    for( auto stage : stages ) {
        out.Writestring( stage.first );
        out.WriteBoolean( stage.second );
    }
    OnSerialize( out );
}

void Level::AddSound( SoundHandle sound ) {
    sounds.push_back( sound );
}
