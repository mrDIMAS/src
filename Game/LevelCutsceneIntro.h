#pragma once

#include "Level.h"

class LevelIntro : public Level {
private:
	shared_ptr<ISceneNode> mUAZ;
	unique_ptr<GameCamera> mCamera;
	shared_ptr<ISceneNode> mCameraPivot;
	Animation mUAZAnim;
	Animation mCameraAnim1;
	Animation mDeerAnim;
	shared_ptr<ISceneNode> mChangeCameraZone1;
	shared_ptr<ISceneNode> mChangeCameraZone2;
	shared_ptr<ISceneNode> mChangeCameraZone3;
	shared_ptr<ISceneNode> mCameraPivot2;
	shared_ptr<ISceneNode> mCameraPivot3;
	shared_ptr<ISceneNode> mCameraPivot4;
	shared_ptr<ISceneNode> mNewLevelLoadZone;
	shared_ptr<ISound> mEngineLoop;
	Vector3 mLastUAZPosition;
	SmoothFloat mEnginePitch;

	// text
	float mTextAlpha;
	float mTextAlphaTo;
	bool mShowIntro;
	shared_ptr<IGUIScene> mGUIScene;
	shared_ptr<IText> mGUIText;
	shared_ptr<IText> mGUISkipText;
public:
	LevelIntro(unique_ptr<Game> & game, const unique_ptr<PlayerTransfer> & playerTransfer);
	~LevelIntro();
	virtual void DoScenario();
	void OnSerialize(SaveFile & out);
	void OnDeserialize(SaveFile & in);
	virtual void Hide()
	{
		Level::Hide();
		mGUIScene->SetVisible(false);
	}
	virtual void Show()
	{
		Level::Show();
		mGUIScene->SetVisible(true);
	}
};