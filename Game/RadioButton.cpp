#include "Precompiled.h"

#include "RadioButton.h"
#include "GUIProperties.h"
#include "Menu.h"

RadioButton::RadioButton(const shared_ptr<IGUIScene> & scene, float x, float y, shared_ptr<ITexture> buttonImage, const string & text) {
	int textHeight = 16;
	float buttonWidth = 110;
	float buttonHeight = 32;
	mGUIButton = scene->CreateButton(x, y, buttonWidth, buttonHeight, buttonImage, text, pGUIProp->mFont, pGUIProp->mForeColor, TextAlignment::Center);
	mCheck = scene->CreateRect(buttonWidth + 10, 6, 20, 20, scene->GetEngine()->GetRenderer()->GetTexture("data/gui/menu/checkbox_checked.tga"), pGUIProp->mForeColor);
	mCheck->Attach(mGUIButton);
	mGUIButton->AddAction(GUIAction::OnClick, [this] { OnChange(); });
	SetEnabled(false);
}

void RadioButton::SetEnabled(bool state) {
	mOn = state;
	SelectCheckTexture();
}

bool RadioButton::IsChecked() {
	return mOn;
}

void RadioButton::SetChangeAction(const Delegate & delegat) {
	mGUIButton->RemoveAllActions();
	mGUIButton->AddAction(GUIAction::OnClick, [this] { OnChange(); });
	mGUIButton->AddAction(GUIAction::OnClick, delegat);
}

void RadioButton::AttachTo(shared_ptr<IGUINode> node) {
	mGUIButton->Attach(node);
}

void RadioButton::SelectCheckTexture() {
	auto renderer = mCheck->GetScene().lock()->GetEngine()->GetRenderer();
	if(mOn) {
		mCheck->SetTexture(renderer->GetTexture("data/gui/menu/checkbox_checked.tga"));
	} else {
		mCheck->SetTexture(renderer->GetTexture("data/gui/menu/checkbox.tga"));
	}
}

void RadioButton::OnChange() {
	mOn = !mOn;
	SelectCheckTexture();
}
