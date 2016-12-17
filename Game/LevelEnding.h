#pragma once

#include "Level.h"

class LevelEnding : public Level {
private:
	shared_ptr<IGUIScene> mGUIScene;
	shared_ptr<IRect> mBackground;
	shared_ptr<IText> mEndingText;
	shared_ptr<IFont> mLargeFont;
	shared_ptr<ICamera> mCamera;
	int mTimer;
public:
	LevelEnding(unique_ptr<Game> & game, const unique_ptr<PlayerTransfer> & playerTransfer);
	~LevelEnding();
	virtual void DoScenario() override final;
	virtual void OnSerialize(SaveFile & out) override final;
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
