#include "Precompiled.h"
#include "Zone.h"

Zone::Zone( ruNodeHandle object ) {
	mObject = object;
	mType = Type::OneShot;
	mPlayerInside = false;
}

void Zone::Update() {
	if( mActive ) {
		if( pPlayer->IsInsideZone( mObject ) ) {
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

void Zone::SetActive( bool active )
{
	mActive = active;
}
