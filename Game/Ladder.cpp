#include "Precompiled.h"

#include "Ladder.h"
#include "Player.h"

Ladder::Ladder( shared_ptr<ruSceneNode> hBegin, shared_ptr<ruSceneNode> hEnd, shared_ptr<ruSceneNode> hEnterZone, shared_ptr<ruSceneNode> hBeginLeavePoint, shared_ptr<ruSceneNode> hEndLeavePoint )
    : Way( hBegin, hEnd, hEnterZone, hBeginLeavePoint, hEndLeavePoint ) {
    mFreeLook = true;
}

void Ladder::DoPlayerCrawling() {
    if( !mEntering ) {
        ruVector3 direction = mTarget->GetPosition() - pPlayer->GetCurrentPosition();

        float distance = direction.Length();

        direction.Normalize();

        if( !mLeave ) {
            pPlayer->Move( direction, 0.75f );

            if( distance < 0.25f ) {
                mLeave = true;

                if( ( pPlayer->GetCurrentPosition() - mEnd->GetPosition() ).Length2() < ( pPlayer->GetCurrentPosition() - mBegin->GetPosition() ).Length2()) {
                    mTarget = mEndLeavePoint;
                } else {
                    mTarget = mBeginLeavePoint;
                }
            }
        } else {
            DoLeave();
        }
    }
}

void Ladder::DoLeave() {
    ruVector3 direction = mTarget->GetPosition() - pPlayer->GetCurrentPosition();

    float distance = direction.Length();

    direction.Normalize();

    pPlayer->Move( direction, 0.75f );

    if( distance < 0.25f ) {
        mLeave = false;
        mInside = false;
        pPlayer->Unfreeze();
        pPlayer->StopInstant();
    }
}

void Ladder::SetDirection( Direction direction ) {
    if( direction == Way::Direction::Forward ) {
        if( mLeave ) {
            mTarget = mEndLeavePoint;
        } else {
            mTarget = mEnd;
        }
    }

    if( direction == Way::Direction::Backward ) {
        if( mLeave ) {
            mTarget = mBeginLeavePoint;
        } else {
            mTarget = mBegin;
        }
    }
}

void Ladder::Enter() {
    Way::Enter();
}
