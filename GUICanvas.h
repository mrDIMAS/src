#pragma  once

#include "GUINode.h"

class GUICanvas : public GUINode {
public:
	float mOptimalResolutionWidth;
	float mOptimalResolutionHeight;
	float mScale;

	explicit GUICanvas( float optimalWidth, float optimalHeight ) {
		mOptimalResolutionHeight = optimalHeight;
		mOptimalResolutionWidth = optimalWidth;
	}

	void Update() {

	}
};