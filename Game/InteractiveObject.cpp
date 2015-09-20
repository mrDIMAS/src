#include "Precompiled.h"

#include "InteractiveObject.h"
#include "Player.h"

vector< InteractiveObject* > InteractiveObject::msObjectList;

void InteractiveObject::DeleteAll() {
    for( auto it = msObjectList.begin(); it != msObjectList.end(); it++ ) {
        delete *it;
    }
}

void InteractiveObject::UpdateAll() {
    for( auto it = msObjectList.begin(); it != msObjectList.end(); it++ ) {
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
        mObject.SetAlbedo( mFlashAlbedo );
    }
}

InteractiveObject::InteractiveObject( ruSceneNode object ) {
    msObjectList.push_back( this );
    this->mObject = object;
    object.Freeze();
    mFlashAlbedo = 0.2f;
    mFlashAlbedoTo = 1.0f;
    mFlashSpeed = 0.075f;
    mFlashDirection = true;
}

InteractiveObject::InteractiveObject()
{
	mFlashAlbedo = 0.0f;
	mFlashAlbedoTo = 0.0f;
	mFlashSpeed = 0.0f;
	mFlashDirection = false;
}

InteractiveObject::~InteractiveObject() {
    msObjectList.erase( find( msObjectList.begin(), msObjectList.end(), this ));
}

void InteractiveObject::Update()
{

}
