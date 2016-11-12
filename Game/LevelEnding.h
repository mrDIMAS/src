#pragma once

#include "Level.h"

class LevelEnding : public Level {
private:
	shared_ptr<ruGUIScene> mGUIScene;
	shared_ptr<ruRect> mBackground;
	shared_ptr<ruText> mEndingText;
	shared_ptr<ruFont> mLargeFont;
	shared_ptr<ruCamera> mCamera;
public:
	LevelEnding(const unique_ptr<PlayerTransfer> & playerTransfer);
	~LevelEnding();
	virtual void DoScenario() override final;
	virtual void OnSerialize(SaveFile & out) override final;
	virtual void Hide() {
		Level::Hide();
		mGUIScene->SetVisible(false);
	}
	virtual void Show() {
		Level::Show();
		mGUIScene->SetVisible(true);
	}
};
