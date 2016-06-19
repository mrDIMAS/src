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
				OnPlayerEnter.DoActions();				
			}
			if( mType != Type::OneShot ) {
				OnPlayerInside.DoActions();
			} else {
				mActive = false;
			}
			mPlayerInside = true;
		} else {
			if( mPlayerInside ) {
				OnPlayerLeave.DoActions();
			}
		}
	}
}

void Zone::SetActive( bool active ) {
	mActive = active;
}
