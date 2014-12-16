#include "Way.h"
#include "Player.h"

vector<Way*> Way::msWayList;

Way::Way( ruNodeHandle hBegin, ruNodeHandle hEnd, ruNodeHandle hEnterZone, ruNodeHandle hBeginLeavePoint, ruNodeHandle hEndLeavePoint ) {
    mBegin = hBegin;
    mEnd = hEnd;
    mEnterZone = hEnterZone;
    mTarget = hBegin;
    mBeginLeavePoint = hBeginLeavePoint;
    mEndLeavePoint = hEndLeavePoint;

    mEntering = false;
    mInside = false;
    mFreeLook = false;
    mLeave = false;

    msWayList.push_back( this );
}

Way::~Way() {
    msWayList.erase( find( msWayList.begin(), msWayList.end(), this ));
}

void Way::Enter() {
    mInside = false;
    mEntering = true;
    if( ( pPlayer->GetCurrentPosition() - ruGetNodePosition( mBegin )).Length2() < ( pPlayer->GetCurrentPosition() - ruGetNodePosition( mEnd )).Length2() ) {
        mTarget = mBegin;
    } else {
        mTarget = mEnd;
    }
    pPlayer->Freeze();
    pPlayer->mpCurrentWay = this;
}

void Way::DoEntering() {
    if( mEntering ) {
        ruVector3 direction = ruGetNodePosition( mTarget ) - pPlayer->GetCurrentPosition();
        float distance = direction.Length();
        direction.Normalize();
        pPlayer->Move( direction, 1.1f );
        if( distance < 0.25f ) {
            mEntering = false;
            mInside = true;
            if( mTarget == mEnd ) {
                mTarget = mBegin;
            } else {
                mTarget = mEnd;
            }
            pPlayer->StopInstant();
        }
    }
}

bool Way::IsFreeLook() {
    return mFreeLook;
}

ruNodeHandle Way::GetTarget() {
    return mTarget;
}

bool Way::IsEnterPicked() {
    return pPlayer->mNearestPickedNode == mEnterZone;
}

bool Way::IsPlayerInside() {
    return mInside;
}

bool Way::IsEntering() {
    return mEntering;
}

ruNodeHandle Way::GetEnterZone() {
    return mEnterZone;
}

void Way::DeserializeWith( TextFileStream & in ) {
    mInside = in.ReadBoolean(  );
    mEntering = in.ReadBoolean(  );
    mFreeLook = in.ReadBoolean(  );
    mLeave = in.ReadBoolean();
    int targetNum = in.ReadInteger( );
    if( targetNum == 0 ) {
        mTarget = mBegin;
    }
    if( targetNum == 1 ) {
        mTarget = mEnd;
    }
    if( targetNum == 2 ) {
        mTarget = mBeginLeavePoint;
    }
    if( targetNum == 3 ) {
        mTarget = mEndLeavePoint;
    }
}

void Way::SerializeWith( TextFileStream & out ) {
    out.WriteString( ruGetNodeName( mEnterZone ));
    out.WriteBoolean( mInside );
    out.WriteBoolean( mEntering );
    out.WriteBoolean( mFreeLook );
    out.WriteBoolean( mLeave );
    int targetNum = 0;
    if( mTarget == mBegin ) {
        targetNum = 0;
    }
    if( mTarget == mEnd ) {
        targetNum = 1;
    }
    if( mTarget == mBeginLeavePoint ) {
        targetNum = 2;
    }
    if( mTarget == mEndLeavePoint ) {
        targetNum = 3;
    }
    out.WriteInteger( targetNum );
}

Way * Way::GetByObject( ruNodeHandle obj ) {
    for( auto way : msWayList )
        if( way->mEnterZone == obj ) {
            return way;
        }
    return 0;
}
