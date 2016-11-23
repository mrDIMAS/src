#pragma once

#include "Goal.h"
#include "Tip.h"
#include "Parser.h"
#include "Item.h"

class GUIBar {
private:
	shared_ptr<IGUINode> mNode;
	shared_ptr<IRect> mBegin;
	shared_ptr<IRect> mEnd;
	shared_ptr<IRect> mBar;
	shared_ptr<IRect> mSign;
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
	GUIBar(const shared_ptr<IGUIScene> & scene, const shared_ptr<ITexture> & signTex, float x, float y, float w, float h, float minValue, float maxValue, float value, const Vector3 & color, bool leftToRight = true);
	void SetValue(float value);
	float GetValue() const;
	void SetSmooth(bool value);
	bool IsSmooth() const;
	void Update();
	shared_ptr<IGUINode> & GetNode();
};

class HUD {
private:
	unique_ptr<Game> & mGame;

	shared_ptr<IGUIScene> mGUIScene;

	shared_ptr<IText> mGUIActionText;

	shared_ptr<IRect> mGUICursorPickUp;
	shared_ptr<IRect> mGUICursorPut;
	shared_ptr<IRect> mGUICrosshair;


	unique_ptr<GUIBar> mHealthBar;
	unique_ptr<GUIBar> mStaminaBar;
	unique_ptr<GUIBar> mStealthBar;
	unique_ptr<GUIBar> mNoiseBar;

	shared_ptr<IText> mGUIYouDied;
	shared_ptr<IFont> mGUIYouDiedFont;
	shared_ptr<IRect> mGUIDamageBackground;
	shared_ptr<IRect> mGUIRectItemForUse;

	int mDamageBackgroundAlpha;

	unique_ptr<Goal> mGoal;
	unique_ptr<Tip> mTip;

	Config mLocalization;
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
	void SetAction(IInput::Key keyCode, const string & action);
	void SetTip(const string & text);
	shared_ptr<IGUIScene> GetScene() const;
	void ShowUsedItem(Item * i);
	void SetCursor(bool somethingPicked, bool pickedHasNormalMass, bool somethingInHands, bool inventoryOpen);
	void Update();
};
