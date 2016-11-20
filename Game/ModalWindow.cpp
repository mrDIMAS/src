#include "Precompiled.h"
#include "ModalWindow.h"
#include "GUIProperties.h"

ModalWindow::ModalWindow(const shared_ptr<ruGUIScene> & scene, int x, int y, int w, int h, shared_ptr<ruTexture> texture, shared_ptr<ruTexture> buttonTexture, ruVector3 color)
{
	int buttonWidth = 128;
	int buttonHeight = 32;

	mCanvas = scene->CreateRect(x, y, w, h, texture, color);
	mYesButton = scene->CreateButton(20, h - buttonHeight - 10, buttonWidth, buttonHeight, buttonTexture, "Yes", pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
	mNoButton = scene->CreateButton(w - buttonWidth - 20, h - buttonHeight - 10, buttonWidth, buttonHeight, buttonTexture, "No", pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
	mText = scene->CreateText("Text", 20, 20, w - 40, h - 40, pGUIProp->mFont, ruVector3(255, 255, 255), ruTextAlignment::Left);

	mYesButton->Attach(mCanvas);
	mNoButton->Attach(mCanvas);
	mText->Attach(mCanvas);

	mYesButton->AddAction(ruGUIAction::OnClick, [this] { Close(); });
	mNoButton->AddAction(ruGUIAction::OnClick, [this] { Close(); });

	Close();
}

void ModalWindow::Ask(const string & text)
{
	mText->SetText(text);
	mCanvas->SetVisible(true);
}

void ModalWindow::AttachTo(shared_ptr<ruGUINode> node)
{
	mCanvas->Attach(node);
}

void ModalWindow::SetYesAction(const ruDelegate & yesAction)
{
	mYesButton->RemoveAllActions();
	mYesButton->AddAction(ruGUIAction::OnClick, yesAction);
	mYesButton->AddAction(ruGUIAction::OnClick, [this] { Close(); });
}

void ModalWindow::SetNoAction(const ruDelegate & noAction)
{
	mNoButton->RemoveAllActions();
	mNoButton->AddAction(ruGUIAction::OnClick, noAction);
	mNoButton->AddAction(ruGUIAction::OnClick, [this] { Close(); });
}

void ModalWindow::Close()
{
	mCanvas->SetVisible(false);
}

void ModalWindow::CloseNoAction()
{
	Close();
}
