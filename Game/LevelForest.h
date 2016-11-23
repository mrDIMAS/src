#pragma once

#include "Level.h"

class LevelForest : public Level {
private:
	shared_ptr<ISound> mWindSound;
	float mWaterFlow;
	shared_ptr<ISceneNode> mWater;
	shared_ptr<ISceneNode> mZoneEnd;
	shared_ptr<IFog> mFog;
public:
	LevelForest(unique_ptr<Game> & game, const unique_ptr<PlayerTransfer> & playerTransfer);
	~LevelForest();
	virtual void DoScenario() override final;
	virtual void OnSerialize(SaveFile & out) override final;
};
