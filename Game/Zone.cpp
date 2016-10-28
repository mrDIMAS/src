#include "Precompiled.h"
#include "Zone.h"
#include "Level.h"

Zone::Zone( shared_ptr<ruSceneNode> object ) : mObject( object ), mType( Type::OneShot ), mPlayerInside( false ), mActive( true ) {
}

void Zone::Update() {
	if( mActive ) {
		auto & player = Level::Current()->GetPlayer();
		if(player->IsInsideZone( mObject ) ) {
			if( !mPlayerInside ) {
				OnPlayerEnter();				
			}
			if( mType != Type::OneShot ) {
				OnPlayerInside();
			} else {
				mActive = false;
			}
			mPlayerInside = true;
		} else {
			if( mPlayerInside ) {
				OnPlayerLeave();
			}
		}
	}
}

void Zone::SetActive( bool active ) {
	mActive = active;
}
