#include "CrawlWay.h"
#include "Player.h"


void CrawlWay::DoPlayerCrawling() {
    if( !mEntering ) {
        ruVector3 direction = ruGetNodePosition( mTarget ) - pPlayer->GetCurrentPosition();

        float distance = direction.Length();

        direction.Normalize();

        if( !mLeave ) {
            pPlayer->Move( direction, 0.75f );

            if( distance < 0.25f ) {
                mLeave = true;

                if( ( pPlayer->GetCurrentPosition() - ruGetNodePosition( mEnd )).Length2() < ( pPlayer->GetCurrentPosition() - ruGetNodePosition( mBegin )).Length2()) {
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

void CrawlWay::LookAtTarget() {

}


void CrawlWay::SetDirection( Direction direction ) {

}

CrawlWay::CrawlWay( ruNodeHandle hBegin, ruNodeHandle hEnd, ruNodeHandle hEnterZone, ruNodeHandle hBeginLeavePoint, ruNodeHandle hEndLeavePoint )
    : Way( hBegin, hEnd, hEnterZone, hBeginLeavePoint, hEndLeavePoint ) {

}

CrawlWay::~CrawlWay() {

}

void CrawlWay::DoLeave() {
    ruVector3 direction = ruGetNodePosition( mTarget ) - pPlayer->GetCurrentPosition();
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
