#include "InteractiveObject.h"
#include "Player.h"

vector< InteractiveObject* > InteractiveObject::objects;

void InteractiveObject::DeleteAll() {
    for( auto it = objects.begin(); it != objects.end(); it++ )
        delete *it;
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
            flashAlbedo += 0.05f;
        }
        else {
            flashDirection = false;
        }
    }
    else {
        if( flashAlbedo > 0 ) {
            flashAlbedo -= 0.05f;
        }
        else {
            flashDirection = true;
        }
    }

    SetAlbedo( node, 0.51f );
}

InteractiveObject::InteractiveObject( NodeHandle object ) {
    objects.push_back( this );

    node = object;

    flashAlbedo = 0.2f;
    flashAlbedoTo = 1.0f;
    flashSpeed = 0.075f;
    flashDirection = true;
}

InteractiveObject::~InteractiveObject() {
    objects.erase( find( objects.begin(), objects.end(), this ));
}
