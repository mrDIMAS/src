#pragma once

#include "Goal.h"
#include "Tip.h"
#include "Parser.h"
#include "Item.h"

class GUIBar {
private:
	shared_ptr<ruGUINode> mNode;
	shared_ptr<ruRect> mBegin;
	shared_ptr<ruRect> mEnd;
	shared_ptr<ruRect> mBar;
	shared_ptr<ruRect> mSign;
	float mValue;
	float mMaxValue;
	float mMinValue;
	float mWidth;
	float mLastValue;
	float mTargetAlpha;
	float mAlpha;
	int mAlphaPause;
	bool mLeftToRight;
	float mWSegment;
	bool mSmooth;
	float mTargetValue;
public:
	GUIBar(const shared_ptr<ruGUIScene> & scene, const shared_ptr<ruTexture> & signTex, float x, float y, float w, float h, float minValue, float maxValue, float value, const ruVector3 & color, bool leftToRight = true);
	void SetValue(float value);
	float GetValue() const;
	void SetSmooth(bool value);
	bool IsSmooth() const;
	void Update();
	shared_ptr<ruGUINode> & GetNode();
};

class HUD {
private:
	unique_ptr<Game> & mGame;

	shared_ptr<ruGUIScene> mGUIScene;

	shared_ptr<ruText> mGUIActionText;

	shared_ptr<ruRect> mGUICursorPickUp;
	shared_ptr<ruRect> mGUICursorPut;
	shared_ptr<ruRect> mGUICrosshair;


	unique_ptr<GUIBar> mHealthBar;
	unique_ptr<GUIBar> mStaminaBar;
	unique_ptr<GUIBar> mStealthBar;
	unique_ptr<GUIBar> mNoiseBar;

	shared_ptr<ruText> mGUIYouDied;
	shared_ptr<ruFont> mGUIYouDiedFont;
	shared_ptr<ruRect> mGUIDamageBackground;
	shared_ptr<ruRect> mGUIRectItemForUse;

	int mDamageBackgroundAlpha;

	unique_ptr<Goal> mGoal;
	unique_ptr<Tip> mTip;

	ruConfig mLocalization;
public:
	HUD(unique_ptr<Game> & game);
	void SetStamina(float value);
	void SetHealth(float value);
	void SetNoise(float value);
	void ShowDamage();
	void SetObjective(const string & text);
	void SetStealth(float value);
	void SetDead(bool dead);
	void SetVisible(bool visible);
	void SetAction(ruInput::Key keyCode, const string & action);
	void SetTip(const string & text);
	shared_ptr<ruGUIScene> GetScene() const;
	void ShowUsedItem(Item * i);
	void SetCursor(bool somethingPicked, bool pickedHasNormalMass, bool somethingInHands, bool inventoryOpen);
	void Update();
};
