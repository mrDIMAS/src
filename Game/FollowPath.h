#pragma once

#include "Game.h"

class FollowPath {
public:
    enum class Direction {
        Unknown  = 0,
        Forward  = 1,
        Backward = 2
    };
private:
    Direction mDirection;
    ruNodeHandle mEntryNode;
    int mPointNum;
    vector<ruNodeHandle> mPointList;
    bool mPathEnded;
public:
    static vector<FollowPath*> msFollowPathList;
    explicit FollowPath();
    virtual ~FollowPath();
    void AddPoint( ruNodeHandle node );
    void NextPoint();
    void SetEntryNode( ruNodeHandle entryNode );
    ruNodeHandle GetEntryNode();
    ruNodeHandle GetTargetPoint();
    void SelectDirection( Direction direction );
    void Enter( ruVector3 entryPosition );
    ruNodeHandle GetBeginPoint();
    ruNodeHandle GetEndPoint();
    bool IsPathEnded();
};