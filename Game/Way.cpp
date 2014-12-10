#include "Way.h"
#include "Player.h"

vector<Way*> Way::sWayList;

Way::Way( ruNodeHandle hBegin, ruNodeHandle hEnd, ruNodeHandle hEnterZone, ruNodeHandle hBeginLeavePoint, ruNodeHandle hEndLeavePoint )
{
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

    sWayList.push_back( this );
}

Way::~Way()
{
    sWayList.erase( find( sWayList.begin(), sWayList.end(), this ));
}

void Way::Enter()
{
    inside = false;
    entering = true;
    if( ( ruGetNodePosition( pPlayer->mBody ) - ruGetNodePosition( begin )).Length2() < ( ruGetNodePosition( pPlayer->mBody ) - ruGetNodePosition( end )).Length2() )
        target = begin;
    else
        target = end;
    ruFreeze( pPlayer->mBody );
    pPlayer->mpCurrentWay = this;
}

void Way::DoEntering()
{
    if( entering ) {
        ruVector3 direction = ruGetNodePosition( target ) - ruGetNodePosition( pPlayer->mBody );

        float distance = direction.Length();

        direction.Normalize();

        ruMoveNode( pPlayer->mBody, direction * 1.1f );

        if( distance < 0.25f ) {
            entering = false;
            inside = true;

            if( target == end )
                target = begin;
            else
                target = end;

            ruMoveNode( pPlayer->mBody, ruVector3( 0, 0, 0 ));
        }
    }
}

bool Way::IsFreeLook()
{
    return freeLook;
}

ruNodeHandle Way::GetTarget()
{
    return target;
}

bool Way::IsEnterPicked()
{
    return pPlayer->mNearestPickedNode == enterZone;
}

bool Way::IsPlayerInside()
{
    return inside;
}

bool Way::IsEntering()
{
    return entering;
}

ruNodeHandle Way::GetEnterZone()
{
    return enterZone;
}

void Way::DeserializeWith( TextFileStream & in )
{
    inside = in.ReadBoolean(  );
    entering = in.ReadBoolean(  );
    freeLook = in.ReadBoolean(  );
    leave = in.ReadBoolean();
    int targetNum = in.ReadInteger( );
    if( targetNum == 0 )
        target = begin;
    if( targetNum == 1 )
        target = end;
    if( targetNum == 2 )
        target = beginLeavePoint;
    if( targetNum == 3 )
        target = endLeavePoint;
}

void Way::SerializeWith( TextFileStream & out )
{
    out.WriteString( ruGetNodeName( enterZone ));
    out.WriteBoolean( inside );
    out.WriteBoolean( entering );
    out.WriteBoolean( freeLook );
    out.WriteBoolean( leave );
    int targetNum = 0;
    if( target == begin )
        targetNum = 0;
    if( target == end )
        targetNum = 1;
    if( target == beginLeavePoint )
        targetNum = 2;
    if( target == endLeavePoint )
        targetNum = 3;
    out.WriteInteger( targetNum );
}

Way * Way::GetByObject( ruNodeHandle obj )
{
    for( auto way : sWayList )
        if( way->enterZone == obj )
            return way;
    return 0;
}
