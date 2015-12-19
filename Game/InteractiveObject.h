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
    shared_ptr<ruSceneNode> mObject;
	ruEvent OnInteract;
    static vector< InteractiveObject* > msObjectList;
    explicit InteractiveObject( shared_ptr<ruSceneNode> object );
	explicit InteractiveObject();
    virtual ~InteractiveObject();
    void UpdateFlashing();
    virtual void Update();
    static void UpdateAll();
    static void DeleteAll();
	void SetPickDescription( const string & pd );
	string GetPickDescription() const;
	static InteractiveObject * FindByObject( shared_ptr<ruSceneNode> node );
};