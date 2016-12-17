#include "Precompiled.h"
#include "HUD.h"

GUIBar::GUIBar(const shared_ptr<IGUIScene>& scene, const shared_ptr<ITexture>& signTex, float x, float y, float w, float h, float minValue, float maxValue, float value, const Vector3 & color, bool leftToRight) :
	mValue(value),
	mMaxValue(maxValue),
	mMinValue(minValue),
	mWidth(w),
	mLastValue(mValue),
	mTargetAlpha(255.0f),
	mAlpha(255.0f),
	mAlphaPause(60),
	mLeftToRight(leftToRight),
	mSmooth(false),
	mTargetValue(value) {
	mNode = scene->CreateNode();
	mNode->SetPosition(x, y);

	mWSegment = h / 3.0f;

	auto renderer = scene->GetEngine()->GetRenderer();

	mBegin = scene->CreateRect(0, 0, mWSegment, h, renderer->GetTexture("data/gui/bar_begin.png"));
	mBegin->SetPosition(-mWSegment, 0);
	mBegin->Attach(mNode);
	mBegin->SetColor(color);

	mBar = scene->CreateRect(0, 0, mWSegment, h, renderer->GetTexture("data/gui/bar.png"));
	mBar->Attach(mNode);
	mBar->SetColor(color);

	mEnd = scene->CreateRect(0, 0, mWSegment, h, renderer->GetTexture("data/gui/bar_end.png"));
	mEnd->Attach(mNode);
	mEnd->SetColor(color);

	mSign = scene->CreateRect(0, 0, h, h, signTex);
	mSign->Attach(mNode);
	if(leftToRight) {
		mSign->SetPosition(-mWSegment - h - 2, 0);
	} else {
		mEnd->SetPosition(mWidth - mWSegment, 0);
		mSign->SetPosition(mWidth + mWSegment / 2, 0);
	}

	SetValue(value);
}

void GUIBar::SetValue(float value) {
	if(mSmooth) {
		mTargetValue = value;
		if(mTargetValue > mMaxValue) {
			mTargetValue = mMaxValue;
		}
		if(mTargetValue < mMinValue) {
			mTargetValue = mMinValue;
		}
	} else {
		mValue = value;
		if(mValue > mMaxValue) {
			mValue = mMaxValue;
		}
		if(mValue < mMinValue) {
			mValue = mMinValue;
		}
	}

	if(mLastValue != mValue) {
		mAlphaPause = 60;
	}

	mTargetAlpha = mAlphaPause > 0 ? 255.0f : 0.0f;
	mLastValue = mValue;

	if(mAlphaPause > 0 && mTargetAlpha > mAlpha) {
		mAlpha += 3.0f;
	} else if(mAlphaPause <= 0) {
		mAlpha -= 3.0f;
	}

	if(mAlpha > 255.0f) {
		mAlpha = 255.0f;
	}
	if(mAlpha < 50.0f) {
		mAlpha = 50.0f;
	}

	mBegin->SetAlpha(mAlpha);
	mBar->SetAlpha(mAlpha);
	mEnd->SetAlpha(mAlpha);
	mSign->SetAlpha(mAlpha);

	float scaler = mValue / (mMaxValue - mMinValue);
	mBar->SetSize(mWidth * scaler, mBar->GetSize().y);
	if(mLeftToRight) {
		mEnd->SetPosition(mWidth * scaler, 0);
	} else {
		mBegin->SetPosition(mWidth - mWidth * scaler - mWSegment * 2, 0);
		mBar->SetPosition(mWidth - mWidth * scaler - mWSegment, 0);
	}

	--mAlphaPause;
}

float GUIBar::GetValue() const {
	return mValue;
}

void GUIBar::SetSmooth(bool value) {
	mSmooth = value;
}

bool GUIBar::IsSmooth() const {
	return mSmooth;
}

void GUIBar::Update() {
	if(mSmooth) {
		mValue += (mTargetValue - mValue) * 0.05f;
	}
}

shared_ptr<IGUINode>& GUIBar::GetNode() {
	return mNode;
}

HUD::HUD(unique_ptr<Game> & game) :
	mGame(game),
	mDamageBackgroundAlpha(0) {
	mLocalization.Load(Game::Instance()->GetLocalizationPath() + "player.loc");

	mGUIScene = mGame->GetEngine()->CreateGUIScene();
	mTip = make_unique<Tip>(mGUIScene);
	mGoal = make_unique<Goal>(mGUIScene);

	mGUIActionText = mGUIScene->CreateText("Action text", ruVirtualScreenWidth / 2 - 256, ruVirtualScreenHeight - 200, 512, 128, pGUIProp->mFont, pGUIProp->mNoticeColor, TextAlignment::Center);

	auto renderer = mGame->GetEngine()->GetRenderer();

	mHealthBar = make_unique<GUIBar>(mGUIScene, renderer->GetTexture("data/gui/health.png"), 32, ruVirtualScreenHeight - 80, 150, 17, 0, 100, 100, pGUIProp->mForeColor);
	mStaminaBar = make_unique<GUIBar>(mGUIScene, renderer->GetTexture("data/gui/stamina.png"), 32, ruVirtualScreenHeight - 60, 150, 17, 0, 100, 100, pGUIProp->mForeColor);
	mStealthBar = make_unique<GUIBar>(mGUIScene, renderer->GetTexture("data/gui/stealth.png"), ruVirtualScreenWidth - 170, ruVirtualScreenHeight - 80, 150, 17, 0, 100, 100, pGUIProp->mForeColor, false);
	mNoiseBar = make_unique<GUIBar>(mGUIScene, renderer->GetTexture("data/gui/noise.png"), ruVirtualScreenWidth - 170, ruVirtualScreenHeight - 60, 150, 17, 0, 100, 100, pGUIProp->mForeColor, false);

	mHealthBar->SetSmooth(true);
	mStaminaBar->SetSmooth(true);
	mStealthBar->SetSmooth(true);
	mNoiseBar->SetSmooth(true);


	mGUIYouDiedFont = mGame->GetEngine()->CreateBitmapFont(40, "data/fonts/font5.ttf");
	mGUIYouDied = mGUIScene->CreateText(mLocalization.GetString("youDied"), (ruVirtualScreenWidth - 300) / 2, ruVirtualScreenHeight / 2, 300, 50, mGUIYouDiedFont, Vector3(255, 0, 0), TextAlignment::Center, 255);
	mGUIYouDied->SetVisible(false);

	mGUIRectItemForUse = mGUIScene->CreateRect(ruVirtualScreenWidth / 2, ruVirtualScreenHeight / 2, 64, 64, nullptr, pGUIProp->mForeColor, 255);

	mGUICursorPickUp = mGUIScene->CreateRect((ruVirtualScreenWidth - 32) / 2, (ruVirtualScreenHeight - 32) / 2, 32, 32, renderer->GetTexture("data/gui/up.tga"));
	mGUICursorPut = mGUIScene->CreateRect((ruVirtualScreenWidth - 32) / 2, (ruVirtualScreenHeight - 32) / 2, 32, 32, renderer->GetTexture("data/gui/down.tga"));
	mGUICrosshair = mGUIScene->CreateRect((ruVirtualScreenWidth - 32) / 2, (ruVirtualScreenHeight - 32) / 2, 32, 32, renderer->GetTexture("data/gui/crosshair.tga"));

	mGUIDamageBackground = mGUIScene->CreateRect(0, 0, ruVirtualScreenWidth, ruVirtualScreenHeight, renderer->GetTexture("data/textures/effects/damageBackground.tga"), Vector3(200, 0, 0), mDamageBackgroundAlpha);

	SetObjective(mLocalization.GetString("objectiveUnknown"));
}

void HUD::SetStamina(float value) {
	mStaminaBar->SetValue(value);
}

void HUD::SetHealth(float value) {
	mHealthBar->SetValue(value);
}

void HUD::SetNoise(float value) {
	mNoiseBar->SetValue(value);
}

void HUD::ShowDamage() {
	mDamageBackgroundAlpha = 60;
}

void HUD::SetObjective(const string & text) {
	string objectiveText = mLocalization.GetString("currentObjective");
	objectiveText += text;

	mGoal->SetText(objectiveText);
}

void HUD::SetStealth(float value) {
	mStealthBar->SetValue(value);
}

void HUD::SetDead(bool dead) {
	mGUIYouDied->SetVisible(dead);
}

void HUD::SetVisible(bool visible) {
	mGUIScene->SetVisible(visible);
}

void HUD::SetAction(IInput::Key keyCode, const string & action) {
	string text;

	if(keyCode == IInput::Key::None) {
		text = action;
	} else {
		text = StringBuilder() << "[" << mGame->GetEngine()->GetInput()->GetKeyName(keyCode) << "] - " << action;
	}

	mGUIActionText->SetText(text);
	mGUIActionText->SetVisible(true);
}

void HUD::SetTip(const string & text) {
	mTip->SetNewText(text);
}

shared_ptr<IGUIScene> HUD::GetScene() const {
	return mGUIScene;
}

void HUD::ShowUsedItem(Item * i) {
	if(i) {
		mGUIRectItemForUse->SetTexture(i->GetPictogram());
		mGUIRectItemForUse->SetVisible(true);
	} else {
		mGUIRectItemForUse->SetVisible(false);
	}
}

void HUD::SetCursor(bool somethingPicked, bool pickedHasNormalMass, bool somethingInHands, bool inventoryOpen) {
	if(inventoryOpen) {
		mGUICursorPut->SetVisible(false);
		mGUICrosshair->SetVisible(false);
		mGUICursorPickUp->SetVisible(false);
	} else {
		if(somethingPicked) {
			if(pickedHasNormalMass) {
				mGUICursorPickUp->SetVisible(true);
				mGUICrosshair->SetVisible(false);
			} else {
				mGUICursorPut->SetVisible(false);
				mGUICrosshair->SetVisible(true);
			}
		} else {
			mGUICursorPickUp->SetVisible(false);
			if(somethingInHands) {
				mGUICursorPut->SetVisible(true);
				mGUICrosshair->SetVisible(false);
			} else {
				mGUICursorPut->SetVisible(false);
				mGUICrosshair->SetVisible(true);
			}
		}
	}
}

void HUD::Update() {
	mHealthBar->Update();
	mStaminaBar->Update();
	mStealthBar->Update();
	mNoiseBar->Update();

	mGUIActionText->SetVisible(false);

	mDamageBackgroundAlpha--;
	if(mDamageBackgroundAlpha < 0) {
		mDamageBackgroundAlpha = 0;
	}
	mGUIDamageBackground->SetAlpha(mDamageBackgroundAlpha);

	mGoal->AnimateAndRender();
	mTip->AnimateAndDraw();
}
