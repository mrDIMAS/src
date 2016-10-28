#pragma once

class Button {
private:
	shared_ptr<ruSceneNode> mNode;
	shared_ptr<ruSound> mPushSound;
	string mText;
	ruAnimation mPushAnimation;
	void Proxy_DoPushActions() {

	}
public:
	ruEvent OnPush; // in the middle of push animation
	Button(const shared_ptr<ruSceneNode> & node, const string & text, const ruDelegate & onPush);
	void Update();
};