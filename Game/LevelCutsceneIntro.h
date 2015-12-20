#pragma once

#include "Level.h"

class LevelCutsceneIntro : public Level {
private:
	shared_ptr<ruSceneNode> mUAZ;
	GameCamera * mCamera;
	shared_ptr<ruSceneNode> mCameraPivot;
	ruAnimation mUAZAnim;
	ruAnimation mCameraAnim1;
	ruAnimation mDeerAnim;
	shared_ptr<ruSceneNode> mChangeCameraZone1;
	shared_ptr<ruSceneNode> mChangeCameraZone2;
	shared_ptr<ruSceneNode> mChangeCameraZone3;
	shared_ptr<ruSceneNode> mCameraPivot2;
	shared_ptr<ruSceneNode> mCameraPivot3;
	shared_ptr<ruSceneNode> mCameraPivot4;
	shared_ptr<ruSceneNode> mNewLevelLoadZone;
	shared_ptr<ruSound> mEngineLoop;
	ruVector3 mLastUAZPosition;
	SmoothFloat mEnginePitch;
public:
	LevelCutsceneIntro();
	~LevelCutsceneIntro();
	virtual void DoScenario();
	void OnSerialize( SaveFile & out );
	void OnDeserialize( SaveFile & in );
};