#include "Door.h"

#include "Player.h"

vector< Door* > Door::all;

bool Door::IsPickedByPlayer() {
    return door == player->nearestPicked;
}

Door::~Door() {
    all.erase( find( all.begin(), all.end(), this ));
}

Door::Door( ruNodeHandle hDoor, float fMaxAngle ) {
    door = hDoor;

    maxAngle = fMaxAngle;

    offsetAngle = ruGetNodeEulerAngles( hDoor ).y;

    currentAngle = 0;

    state = State::Closed;

    openSound = ruLoadSound3D( "data/sounds/door/dooropen.ogg" );
    ruAttachSound( openSound, door );

    closeSound = ruLoadSound3D( "data/sounds/door/doorclose.ogg" );
    ruAttachSound( closeSound, door );

    all.push_back( this );
}

void Door::DoInteraction() {
    if( state == State::Closing ) {
        currentAngle -= 60.0f * g_dt;

        if( currentAngle < 0 ) {
            currentAngle = 0.0f;

            state = State::Closed;
        }
    }

    if( state == State::Opening ) {
        currentAngle += 60.0f * g_dt;

        if( currentAngle > maxAngle ) {
            state = State::Opened;

            currentAngle = maxAngle;
        }
    }

    ruSetNodeRotation( door, ruQuaternion( ruVector3( 0, 1, 0 ), currentAngle + offsetAngle ));
}

Door::State Door::GetState() {
    return state;
}

void Door::SwitchState() {
    if( state == State::Closed ) {
        state = State::Opening;
        ruPlaySound( openSound );
    }
    if( state == State::Opened ) {
        state = State::Closing;
        ruPlaySound( closeSound );
    }
}

void Door::Close()
{
	if( state == State::Opened ) {
		state = State::Closing;
		ruPlaySound( closeSound );
	}
}

void Door::Open()
{
	if( state == State::Closed ) {
		state = State::Opening;
		ruPlaySound( openSound );
	}
}
