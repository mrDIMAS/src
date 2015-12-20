#pragma once

class Button {
private:
	shared_ptr<ruSceneNode> mNode;
	ruVector3 mInitialPosition;
	ruVector3 mSize;
	ruVector3 mAxis;
	shared_ptr<ruSound> mPushSound;
	shared_ptr<ruSound> mPopSound;
	bool mPush;
public:
	ruEvent OnPush;
	explicit Button( shared_ptr<ruSceneNode> node, const ruVector3 & axis, shared_ptr<ruSound> pushSound, shared_ptr<ruSound> popSound );
	void Update();
};