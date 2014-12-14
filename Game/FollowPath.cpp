#include "FollowPath.h"

vector<FollowPath*> FollowPath::msFollowPathList;

bool FollowPath::IsPathEnded()
{
	return mPathEnded;
}

void FollowPath::SelectDirection( Direction direction )
{
	mDirection = direction;
}

void FollowPath::Enter( ruVector3 entryPosition ) 
{
	if( mPointList.size() > 1 ) {
		int iBeginPoint = 0;
		int iEndPoint = mPointList.size() - 1;
		float distanceFromBeginToEntry = (ruGetNodePosition( mPointList[iBeginPoint] ) - entryPosition).Length2();
		float distanceFromEndToEntry = (ruGetNodePosition( mPointList[iEndPoint] ) - entryPosition).Length2();
		if( distanceFromBeginToEntry < distanceFromEndToEntry ) {
			mPointNum = iBeginPoint;
		} else {
			mPointNum = iEndPoint;
		}		
		mPathEnded = false;
	} else {
		RaiseError("FollowPath::SelectBeginPointAndDirection path must contain at least begin and end!");
	}
}

ruNodeHandle FollowPath::GetTargetPoint()
{
	return mPointList[mPointNum];
}

void FollowPath::NextPoint()
{
	if( mPointList.size() ) {
		if( mDirection == Direction::Forward )
		{
			int iEndPoint = mPointList.size() - 1;
			if( mPointNum < iEndPoint ) {
				mPointNum++;
			} else {
				mPathEnded = true;
			}
		} else if( mDirection == Direction::Backward ) {
			if( mPointNum > 0 ) {
				mPointNum--;					
			} else {
				mPathEnded = true;
			}
		}
	} else {
		RaiseError("FollowPath::NextPoint path is empty!");
	}
}

void FollowPath::AddPoint( ruNodeHandle node )
{
	mPointList.push_back( node );
}

FollowPath::~FollowPath()
{
	mPointList.clear();
	msFollowPathList.erase( find( msFollowPathList.begin(), msFollowPathList.end(), this ));
}

FollowPath::FollowPath()
{
	mPointNum = 0;
	mPathEnded = true;
	msFollowPathList.push_back( this );
}

ruNodeHandle FollowPath::GetEndPoint()
{
	if( mPointList.size() > 1 ) {
		return mPointList[ mPointList.size() - 1 ];
	} else {
		RaiseError("FollowPath::GetBeginPoint path must contain begin and end!");
	};
}

ruNodeHandle FollowPath::GetBeginPoint()
{
	if( mPointList.size() ) {
		return mPointList[0];
	} else {
		RaiseError("FollowPath::GetBeginPoint path is empty");
	};
}

ruNodeHandle FollowPath::GetEntryNode()
{
	if( mEntryNode.IsValid() ) {
		return mEntryNode;
	} else {
		RaiseError( "FollowPath::GetEntryNode mEntryNode invalid!");
	};
}

void FollowPath::SetEntryNode( ruNodeHandle entryNode )
{
	mEntryNode = entryNode;
}
