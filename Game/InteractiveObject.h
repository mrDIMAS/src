#pragma once

#include "Game.h"

class InteractiveObject {
private:
	float mFlashAlbedo;
	float mFlashAlbedoTo;
	float mFlashSpeed;
	bool mFlashDirection;
	int mInteractCountLeft;
	string mPickDesc;
public:
    ruSceneNode mObject;
	ruEvent OnInteract;
    static vector< InteractiveObject* > msObjectList;
    explicit InteractiveObject( ruSceneNode object );
	explicit InteractiveObject();
    virtual ~InteractiveObject();
    void UpdateFlashing();
    virtual void Update();
    static void UpdateAll();
    static void DeleteAll();
	void SetPickDescription( const string & pd ) {
		mPickDesc = pd;
	}
	string GetPickDescription() const {
		return mPickDesc;
	}
	static InteractiveObject * FindByObject( ruSceneNode node ) {
		for( auto & pIO : msObjectList ) {
			if( pIO->mObject == node ) {
				return pIO;
			}
		}
		return nullptr;
	}
};