#pragma once

#include "Level.h"

class LevelForest : public Level {
private:
	shared_ptr<ruSound> mWindSound;
	float mWaterFlow;
	shared_ptr<ruSceneNode> mWater;
	shared_ptr<ruSceneNode> mZoneEnd;
	shared_ptr<ruFog> mFog;
public:
	LevelForest(const unique_ptr<PlayerTransfer> & playerTransfer);
	~LevelForest();
	virtual void DoScenario() override final;
	virtual void OnSerialize(SaveFile & out) override final;
};
