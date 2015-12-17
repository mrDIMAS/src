#pragma once

#include "Level.h"
#include "LightAnimator.h"
#include "AmbientSoundSet.h"
#include "Gate.h"

class LevelSewers : public Level {
public:
	Gate * mGate1;
	Gate * mGate2;

	ruSceneNode * mZoneKnocks;
	ruSound mKnocksSound;

	explicit LevelSewers( );
	~LevelSewers( );

	virtual void DoScenario();
	virtual void Show();
	virtual void Hide();

	virtual void OnSerialize( SaveFile & out ) final;

	virtual void OnDeserialize( SaveFile & in ) final;
};