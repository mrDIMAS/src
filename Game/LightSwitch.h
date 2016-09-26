#pragma once

#include "SaveFile.h"

class LightSwitch {
private:
	shared_ptr<ruSceneNode> mModel;
	bool mEnabled;
	shared_ptr<ruSound> mSwitchSound;
	vector<shared_ptr<ruLight>> mLights;
	ruAnimation mSwitchOnAnim;
	ruAnimation mSwitchOffAnim;
public:
	LightSwitch(const shared_ptr<ruSceneNode> & model, const vector<shared_ptr<ruLight>> & lights, bool enabled = true);
	void AddLight(const shared_ptr<ruLight> & light);
	void Update();
	void Serialize(SaveFile & out);
};
