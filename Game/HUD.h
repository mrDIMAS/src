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
public:
	GUIBar(const shared_ptr<ruGUIScene> & scene, const shared_ptr<ruTexture> & signTex, float x, float y, float w, float h, float minValue, float maxValue, float value, const ruVector3 & color) :
		mValue(value),
		mMaxValue(maxValue),
		mMinValue(minValue),
		mWidth(w),
		mLastValue(mValue),
		mTargetAlpha(255.0f),
		mAlpha(255.0f),
		mAlphaPause(60) {
		mNode = scene->CreateNode();
		mNode->SetPosition(x, y);

		float wSegment = h / 3.0f;

		mBegin = scene->CreateRect(0, 0, wSegment, h, ruTexture::Request("data/gui/bar_begin.png"));
		mBegin->SetPosition(-wSegment, 0);
		mBegin->Attach(mNode);
		mBegin->SetColor(color);

		mBar = scene->CreateRect(0, 0, wSegment, h, ruTexture::Request("data/gui/bar.png"));
		mBar->Attach(mNode);
		mBar->SetColor(color);

		mEnd = scene->CreateRect(0, 0, wSegment, h, ruTexture::Request("data/gui/bar_end.png"));
		mEnd->Attach(mNode);
		mEnd->SetColor(color);

		mSign = scene->CreateRect(0, 0, h, h, signTex);
		mSign->Attach(mNode);
		mSign->SetPosition(-wSegment - h - 2, 0);

		SetValue(value);
	}

	void SetValue(float value) {
		mValue = value;
		if (mValue > mMaxValue) {
			mValue = mMaxValue;
		}
		if (mValue < mMinValue) {
			mValue = mMinValue;
		}
		if (mLastValue != mValue) {
			mAlphaPause = 60;
		}

		mTargetAlpha = mAlphaPause > 0 ? 255.0f : 0.0f;
		mLastValue = mValue;

		if (mAlphaPause > 0 && mTargetAlpha > mAlpha) {
			mAlpha += 3.0f;
		} else if (mAlphaPause <= 0) {
			mAlpha -= 3.0f;
		}

		if (mAlpha > 255.0f) {
			mAlpha = 255.0f;
		}
		if (mAlpha < 50.0f) {
			mAlpha = 50.0f;
		}

		mBegin->SetAlpha(mAlpha);
		mBar->SetAlpha(mAlpha);
		mEnd->SetAlpha(mAlpha);
		mSign->SetAlpha(mAlpha);

		float scaler = mValue / (mMaxValue - mMinValue);
		mBar->SetSize(mWidth * scaler, mBar->GetSize().y);
		mEnd->SetPosition(mWidth * scaler, 0);

		--mAlphaPause;
	}

	float GetValue() const {
		return mValue;
	}

	shared_ptr<ruGUINode> & GetNode() {
		return mNode;
	}
};

class HUD {
private:
	shared_ptr<ruGUIScene> mGUIScene;

	shared_ptr<ruText> mGUIActionText;

	shared_ptr<ruRect> mGUICursorPickUp;
	shared_ptr<ruRect> mGUICursorPut;
	shared_ptr<ruRect> mGUICrosshair;


	unique_ptr<GUIBar> mHealthBar;
	unique_ptr<GUIBar> mStaminaBar;
	unique_ptr<GUIBar> mStealthBar;

	shared_ptr<ruText> mGUIYouDied;
	shared_ptr<ruFont> mGUIYouDiedFont;
	shared_ptr<ruRect> mGUIDamageBackground;
	shared_ptr<ruRect> mGUIRectItemForUse;

	int mDamageBackgroundAlpha;

	unique_ptr<Goal> mGoal;
	unique_ptr<Tip> mTip;

	Parser mLocalization;
public:
	HUD() :
		mDamageBackgroundAlpha(0)
	{
		mLocalization.ParseFile(gLocalizationPath + "player.loc");

		mGUIScene = ruGUIScene::Create();
		mTip = unique_ptr<Tip>(new Tip(mGUIScene));
		mGoal = unique_ptr<Goal>(new Goal(mGUIScene));

		mGUIActionText = mGUIScene->CreateText("Action text", ruVirtualScreenWidth / 2 - 256, ruVirtualScreenHeight - 200, 512, 128, pGUIProp->mFont, pGUIProp->mNoticeColor, ruTextAlignment::Center);

		mHealthBar = unique_ptr<GUIBar>(new GUIBar(mGUIScene, ruTexture::Request("data/gui/health.png"), 32, ruVirtualScreenHeight - 80, 150, 17, 0, 100, 100, pGUIProp->mForeColor));
		mStaminaBar = unique_ptr<GUIBar>(new GUIBar(mGUIScene, ruTexture::Request("data/gui/stamina.png"), 32, ruVirtualScreenHeight - 60, 150, 17, 0, 100, 100, pGUIProp->mForeColor));
		mStealthBar = unique_ptr<GUIBar>(new GUIBar(mGUIScene, ruTexture::Request("data/gui/stealth.png"), 32, ruVirtualScreenHeight - 40, 150, 17, 0, 100, 100, pGUIProp->mForeColor));

		mGUIYouDiedFont = ruFont::LoadFromFile(40, "data/fonts/font5.ttf");
		mGUIYouDied = mGUIScene->CreateText(mLocalization.GetString("youDied"), (ruVirtualScreenWidth - 300) / 2, ruVirtualScreenHeight / 2, 300, 50, mGUIYouDiedFont, ruVector3(255, 0, 0), ruTextAlignment::Center, 255);
		mGUIYouDied->SetVisible(false);

		mGUIRectItemForUse = mGUIScene->CreateRect(ruVirtualScreenWidth / 2, ruVirtualScreenHeight / 2, 64, 64, nullptr, pGUIProp->mForeColor, 255);

		mGUICursorPickUp = mGUIScene->CreateRect((ruVirtualScreenWidth - 32) / 2, (ruVirtualScreenHeight - 32) / 2, 32, 32, ruTexture::Request("data/gui/up.tga"));
		mGUICursorPut = mGUIScene->CreateRect((ruVirtualScreenWidth - 32) / 2, (ruVirtualScreenHeight - 32) / 2, 32, 32, ruTexture::Request("data/gui/down.tga"));
		mGUICrosshair = mGUIScene->CreateRect((ruVirtualScreenWidth - 32) / 2, (ruVirtualScreenHeight - 32) / 2, 32, 32, ruTexture::Request("data/gui/crosshair.tga"));
				
		mGUIDamageBackground = mGUIScene->CreateRect(0, 0, ruVirtualScreenWidth, ruVirtualScreenHeight, ruTexture::Request("data/textures/effects/damageBackground.tga"), ruVector3(200, 0, 0), mDamageBackgroundAlpha);

		SetObjective(mLocalization.GetString("objectiveUnknown"));
	}

	void SetStamina(float value) {
		mStaminaBar->SetValue(value);
	}

	void SetHealth(float value) {
		mHealthBar->SetValue(value);
	}

	void ShowDamage() {
		mDamageBackgroundAlpha = 60;
	}

	void SetObjective(const string & text) {
		string objectiveText = mLocalization.GetString("currentObjective");
		objectiveText += text;

		mGoal->SetText(objectiveText);
	}

	void SetStealth(float value) {
		mStealthBar->SetValue(value);
	}

	void SetDead(bool dead) {
		mGUIYouDied->SetVisible(dead);
	}

	void SetVisible(bool visible) {
		mGUIScene->SetVisible(visible);
	}
	
	void SetAction(ruInput::Key keyCode, const string & action) {
		string text;
		
		if (keyCode == ruInput::Key::None) {
			text = action;
		} else {
			text = StringBuilder() << "[" << ruInput::GetKeyName(keyCode) << "] - " << action;
		}

		mGUIActionText->SetText(text);
		mGUIActionText->SetVisible(true);
	}

	void SetTip(const string & text) {
		mTip->SetNewText(text);
	}

	shared_ptr<ruGUIScene> GetScene() const {
		return mGUIScene;
	}

	void ShowUsedItem(Item * i) {
		if (i) {
			mGUIRectItemForUse->SetTexture(i->GetPictogram());
			mGUIRectItemForUse->SetVisible(true);
		} else {
			mGUIRectItemForUse->SetVisible(false);
		}
	}

	void SetCursor(bool somethingPicked, bool pickedHasNormalMass, bool somethingInHands, bool inventoryOpen) {
		if (inventoryOpen) {
			mGUICursorPut->SetVisible(false);
			mGUICrosshair->SetVisible(false);
			mGUICursorPickUp->SetVisible(false);
		} else {
			if (somethingPicked) {
				if (pickedHasNormalMass) {
					mGUICursorPickUp->SetVisible(true);
					mGUICrosshair->SetVisible(false);
				} else {
					mGUICursorPut->SetVisible(false);
					mGUICrosshair->SetVisible(true);
				}
			} else {
				mGUICursorPickUp->SetVisible(false);
				if (somethingInHands) {
					mGUICursorPut->SetVisible(true);
					mGUICrosshair->SetVisible(false);
				} else {
					mGUICursorPut->SetVisible(false);
					mGUICrosshair->SetVisible(true);
				}
			}
		}
	}

	void Update() {
		mGUIActionText->SetVisible(false);

		mDamageBackgroundAlpha--;
		if (mDamageBackgroundAlpha < 0) {
			mDamageBackgroundAlpha = 0;
		}
		mGUIDamageBackground->SetAlpha(mDamageBackgroundAlpha);

		mGoal->AnimateAndRender();
		mTip->AnimateAndDraw();
	}
};
