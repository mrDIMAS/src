#pragma once

#include "Game.h"

class Actor
{
protected:
    float bodyHeight;
    float bodyWidth;
    ruVector3 currentPosition;
    ruVector3 newPosition;

public:
    ruNodeHandle mBody;
    explicit Actor( float height, float width );
    virtual ~Actor();
    void SetPosition( ruVector3 position );
    void Move( ruVector3 direction, float speed );
    ruVector3 GetCurrentPosition( )
    {
        return ruGetNodePosition( mBody );
    }
    char IsInsideZone( ruNodeHandle zone )
    {
        return ruIsNodeInsideNode( mBody, zone );
    }
};