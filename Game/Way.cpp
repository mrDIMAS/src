#include "Precompiled.h"
#include "Level.h"
#include "Way.h"
#include "Player.h"

vector<Way*> Way::msWayList;

Way::Way( shared_ptr<ruSceneNode> hBegin, shared_ptr<ruSceneNode> hEnd, shared_ptr<ruSceneNode> hEnterZone, shared_ptr<ruSceneNode> hBeginLeavePoint, shared_ptr<ruSceneNode> hEndLeavePoint ) {
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
	auto & player = Level::Current()->GetPlayer();
    if( (player->GetCurrentPosition() - mBegin->GetPosition()).Length2() < (player->GetCurrentPosition() - mEnd->GetPosition()).Length2() ) {
        mTarget = mBegin;
    } else {
        mTarget = mEnd;
    }
	player->Freeze();
}

void Way::DoEntering() {
    if( mEntering ) {
		auto & player = Level::Current()->GetPlayer();
        ruVector3 direction = mTarget->GetPosition() - player->GetCurrentPosition();
        float distance = direction.Length();
        direction.Normalize();
		player->Move( direction, 1.1f );
        if( distance < 0.25f ) {
            mEntering = false;
            mInside = true;
            if( mTarget == mEnd ) {
                mTarget = mBegin;
            } else {
                mTarget = mEnd;
            }
			player->StopInstant();
        }
    }
}

bool Way::IsFreeLook() {
    return mFreeLook;
}

shared_ptr<ruSceneNode> Way::GetTarget() {
    return mTarget;
}

bool Way::IsEnterPicked() {
	auto & player = Level::Current()->GetPlayer();
    return player->mNearestPickedNode == mEnterZone;
}

bool Way::IsPlayerInside() {
    return mInside;
}

bool Way::IsEntering() {
    return mEntering;
}

shared_ptr<ruSceneNode> Way::GetEnterZone() {
    return mEnterZone;
}

void Way::Deserialize( SaveFile & in ) {
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

void Way::Serialize( SaveFile & out ) {
    out.WriteString( mEnterZone->GetName() );
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

Way * Way::GetByObject( shared_ptr<ruSceneNode> obj ) {
    for( auto way : msWayList ) {
        if( way->mEnterZone == obj ) {
            return way;
        }
	}
    return nullptr;
}
