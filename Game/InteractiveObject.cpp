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
    if( flashDirection ) {
        if( flashAlbedo < 1 ) {
            flashAlbedo += 0.015f;
        } else {
            flashDirection = false;
        }
    } else {
        if( flashAlbedo > 0.2 ) {
            flashAlbedo -= 0.015f;
        } else {
            flashDirection = true;
        }
    }

    if( object.IsValid() ) {
        SetAlbedo( object, flashAlbedo );
    }
}

InteractiveObject::InteractiveObject( NodeHandle object ) {
    objects.push_back( this );

    this->object = object;

    flashAlbedo = 0.2f;
    flashAlbedoTo = 1.0f;
    flashSpeed = 0.075f;
    flashDirection = true;
}

InteractiveObject::~InteractiveObject() {
    objects.erase( find( objects.begin(), objects.end(), this ));
}
