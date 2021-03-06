#pragma once

class Button {
private:
	shared_ptr<ISceneNode> mNode;
	shared_ptr<ISound> mPushSound;
	string mText;
	Animation mPushAnimation;
	bool mEnabled;
public:
	Event OnPush; // in the middle of push animation
	Button(const shared_ptr<ISceneNode> & node, const string & text, const Delegate & onPush);
	void Update();
	void SetEnabled(bool state) {
		mEnabled = state;
	}
	bool IsEnabled() const {
		return mEnabled;
	}
};