#pragma once

#include "SaveFile.h"

class LightSwitch {
private:
	shared_ptr<ISceneNode> mModel;
	bool mEnabled;
	shared_ptr<ISound> mSwitchSound;
	vector<shared_ptr<ILight>> mLights;
	Animation mSwitchOnAnim;
	Animation mSwitchOffAnim;
public:
	LightSwitch(const shared_ptr<ISceneNode> & model, const vector<shared_ptr<ILight>> & lights, bool enabled = true);
	void AddLight(const shared_ptr<ILight> & light);
	void Update();
	void Serialize(SaveFile & out);
};
