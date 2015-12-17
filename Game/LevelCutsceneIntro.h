#pragma once

#include "Level.h"

class LevelCutsceneIntro : public Level {
private:
	ruSceneNode * mUAZ;
	GameCamera * mCamera;
	ruSceneNode * mCameraPivot;
	ruAnimation mUAZAnim;
	ruAnimation mCameraAnim1;
	ruAnimation mDeerAnim;
	ruSceneNode * mChangeCameraZone1;
	ruSceneNode * mChangeCameraZone2;
	ruSceneNode * mChangeCameraZone3;
	ruSceneNode * mCameraPivot2;
	ruSceneNode * mCameraPivot3;
	ruSceneNode * mCameraPivot4;
	ruSceneNode * mNewLevelLoadZone;
	ruSound mEngineLoop;
	ruVector3 mLastUAZPosition;
	SmoothFloat mEnginePitch;
public:
	LevelCutsceneIntro();
	~LevelCutsceneIntro();
	virtual void DoScenario();
	void OnSerialize( SaveFile & out );
	void OnDeserialize( SaveFile & in );
};