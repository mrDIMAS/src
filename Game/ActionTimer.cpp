#include "Precompiled.h"
#include "ActionTimer.h"

extern double gFixedTick;

vector<ActionTimer*> ActionTimer::msActionTimerList;

void ActionTimer::Update() {
	if( mActive ) {
		mTimeCounter += gFixedTick; 
		OnIdle.DoActions();
		if( mTimeCounter >= mTimeInterval ) {
			OnEnd.DoActions();
			mActive = false;
			if( mSelfDelete ) {
				mCanBeSelfDeleted = true;
			}
		}
	}
}

void ActionTimer::Activate() {
	if( !mActive ) {
		mActive = true;
		mTimeCounter = 0.0f;
		OnBegin.DoActions();
	}
}

ActionTimer::ActionTimer( float timeInterval, bool selfDelete ) {
	mTimeInterval = timeInterval;
	mActive = false;
	mStoragePointerErased = false;
	mCanBeSelfDeleted = false;
	mTimeCounter = 0.0f;
	mSelfDelete = selfDelete;
	msActionTimerList.push_back( this );
}

ActionTimer::~ActionTimer() {
	if( !mStoragePointerErased ) {
		msActionTimerList.erase( find( msActionTimerList.begin(), msActionTimerList.end(), this ));
	}
}

void ActionTimer::UpdateAll() {
	for( auto iter = msActionTimerList.begin(); iter != msActionTimerList.end(); ) {
		ActionTimer * pAT = *iter;
		if( pAT->mCanBeSelfDeleted ) {
			iter = msActionTimerList.erase( iter );
			pAT->mStoragePointerErased = true;
			delete pAT;
		} else {
			pAT->Update();
			iter++;
		}
	}
}

float ActionTimer::GetPercentage() {
	return mTimeCounter / mTimeInterval;
}

void ActionTimer::Deserialize( TextFileStream & in ) {
	in.ReadBoolean( mActive );
	in.ReadFloat( mTimeInterval );
	in.ReadFloat( mTimeCounter );
	in.ReadBoolean( mSelfDelete );
	in.ReadBoolean( mStoragePointerErased );
	in.ReadBoolean( mCanBeSelfDeleted );
}

void ActionTimer::Serialize( TextFileStream & out ) {
	out.WriteBoolean( mActive );
	out.WriteFloat( mTimeInterval );
	out.WriteFloat( mTimeCounter );
	out.WriteBoolean( mSelfDelete );
	out.WriteBoolean( mStoragePointerErased );
	out.WriteBoolean( mCanBeSelfDeleted );
}
