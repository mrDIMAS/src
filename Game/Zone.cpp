#include "Precompiled.h"
#include "Zone.h"
#include "Level.h"



void Zone::Update()
{
	if(mActive) {
		auto & player = Game::Instance()->GetLevel()->GetPlayer();
		if(player->IsInsideZone(mObject)) {
			if(!mPlayerInside) {
				OnPlayerEnter();
			}
			if(mType != Type::OneShot) {
				OnPlayerInside();
			} else {
				mActive = false;
			}
			mPlayerInside = true;
		} else {
			if(mPlayerInside) {
				OnPlayerLeave();
			}
		}
	}
}

void Zone::SetActive(bool active)
{
	mActive = active;
}
