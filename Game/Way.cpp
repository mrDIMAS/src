#include "Way.h"
#include "Player.h"

vector<Way*> Way::all;

Way::Way( ruNodeHandle hBegin, ruNodeHandle hEnd, ruNodeHandle hEnterZone, ruNodeHandle hBeginLeavePoint, ruNodeHandle hEndLeavePoint ) {
    begin = hBegin;
    end = hEnd;
    enterZone = hEnterZone;
    target = hBegin;
    beginLeavePoint = hBeginLeavePoint;
    endLeavePoint = hEndLeavePoint;

    entering = false;
    inside = false;
    freeLook = false;
    leave = false;

    all.push_back( this );
}

Way::~Way() {
    all.erase( find( all.begin(), all.end(), this ));
}

void Way::Enter() {
    inside = false;
    entering = true;
    if( ( ruGetNodePosition( player->body ) - ruGetNodePosition( begin )).Length2() < ( ruGetNodePosition( player->body ) - ruGetNodePosition( end )).Length2() ) {
        target = begin;
    } else {
        target = end;
    }
    ruFreeze( player->body );
    player->currentWay = this;
}

void Way::DoEntering() {
    if( entering ) {
        ruVector3 direction = ruGetNodePosition( target ) - ruGetNodePosition( player->body );

        float distance = direction.Length();

        direction.Normalize();

        ruMoveNode( player->body, direction * 1.1f );

        if( distance < 0.25f ) {
            entering = false;
            inside = true;

            if( target == end ) {
                target = begin;
            } else {
                target = end;
            }

            ruMoveNode( player->body, ruVector3( 0, 0, 0 ));
        }
    }
}

bool Way::IsFreeLook() {
    return freeLook;
}

ruNodeHandle Way::GetTarget() {
    return target;
}

bool Way::IsEnterPicked() {
    return player->nearestPicked == enterZone;
}

bool Way::IsPlayerInside() {
    return inside;
}

bool Way::IsEntering() {
    return entering;
}

ruNodeHandle Way::GetEnterZone() {
    return enterZone;
}

void Way::DeserializeWith( TextFileStream & in ) {
    inside = in.ReadBoolean(  );
    entering = in.ReadBoolean(  );
    freeLook = in.ReadBoolean(  );
    leave = in.ReadBoolean();
    int targetNum = in.ReadInteger( );
    if( targetNum == 0 ) {
        target = begin;
    }
    if( targetNum == 1 ) {
        target = end;
    }
    if( targetNum == 2 ) {
        target = beginLeavePoint;
    }
    if( targetNum == 3 ) {
        target = endLeavePoint;
    }
}

void Way::SerializeWith( TextFileStream & out ) {
    out.Writestring( ruGetNodeName( enterZone ));
    out.WriteBoolean( inside );
    out.WriteBoolean( entering );
    out.WriteBoolean( freeLook );
    out.WriteBoolean( leave );
    int targetNum = 0;
    if( target == begin ) {
        targetNum = 0;
    }
    if( target == end ) {
        targetNum = 1;
    }
    if( target == beginLeavePoint ) {
        targetNum = 2;
    }
    if( target == endLeavePoint ) {
        targetNum = 3;
    }
    out.WriteInteger( targetNum );
}

Way * Way::GetByObject( ruNodeHandle obj ) {
    for( auto way : all )
        if( way->enterZone == obj ) {
            return way;
        }
    return 0;
}
