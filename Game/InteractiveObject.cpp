#include "Precompiled.h"

#include "InteractiveObject.h"
#include "Player.h"

vector< InteractiveObject* > InteractiveObject::objects;

void InteractiveObject::DeleteAll() {
    for( auto it = objects.begin(); it != objects.end(); it++ ) {
        delete *it;
    }
}

void InteractiveObject::UpdateAll() {
    for( auto it = objects.begin(); it != objects.end(); it++ ) {
        InteractiveObject * io = *it;

        io->UpdateFlashing();
        io->Update();
    }
}

void InteractiveObject::UpdateFlashing() {
    if( mFlashDirection ) {
        if( mFlashAlbedo < 1 ) {
            mFlashAlbedo += 0.015f;
        } else {
            mFlashDirection = false;
        }
    } else {
        if( mFlashAlbedo > 0.2 ) {
            mFlashAlbedo -= 0.015f;
        } else {
            mFlashDirection = true;
        }
    }

    if( mObject.IsValid() ) {
        ruSetNodeAlbedo( mObject, mFlashAlbedo );
    }
}

InteractiveObject::InteractiveObject( ruNodeHandle object ) {
    objects.push_back( this );
    this->mObject = object;
    ruFreeze( object );
    mFlashAlbedo = 0.2f;
    mFlashAlbedoTo = 1.0f;
    mFlashSpeed = 0.075f;
    mFlashDirection = true;
}

InteractiveObject::~InteractiveObject() {
    objects.erase( find( objects.begin(), objects.end(), this ));
}
