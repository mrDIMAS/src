#pragma once

class Button {
private:
	ruSceneNode * mNode;
	ruVector3 mInitialPosition;
	ruVector3 mSize;
	ruVector3 mAxis;
	ruSound mPushSound;
	ruSound mPopSound;
	bool mPush;
public:
	ruEvent OnPush;
	explicit Button( ruSceneNode * node, const ruVector3 & axis, ruSound pushSound, ruSound popSound );
	void Update();
};