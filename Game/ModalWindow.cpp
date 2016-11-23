#include "Precompiled.h"
#include "ModalWindow.h"
#include "GUIProperties.h"

ModalWindow::ModalWindow(const shared_ptr<IGUIScene> & scene, int x, int y, int w, int h, shared_ptr<ITexture> texture, shared_ptr<ITexture> buttonTexture, Vector3 color)
{
	int buttonWidth = 128;
	int buttonHeight = 32;

	mCanvas = scene->CreateRect(x, y, w, h, texture, color);
	mYesButton = scene->CreateButton(20, h - buttonHeight - 10, buttonWidth, buttonHeight, buttonTexture, "Yes", pGUIProp->mFont, pGUIProp->mForeColor, TextAlignment::Center);
	mNoButton = scene->CreateButton(w - buttonWidth - 20, h - buttonHeight - 10, buttonWidth, buttonHeight, buttonTexture, "No", pGUIProp->mFont, pGUIProp->mForeColor, TextAlignment::Center);
	mText = scene->CreateText("Text", 20, 20, w - 40, h - 40, pGUIProp->mFont, Vector3(255, 255, 255), TextAlignment::Left);

	mYesButton->Attach(mCanvas);
	mNoButton->Attach(mCanvas);
	mText->Attach(mCanvas);

	mYesButton->AddAction(GUIAction::OnClick, [this] { Close(); });
	mNoButton->AddAction(GUIAction::OnClick, [this] { Close(); });

	Close();
}

void ModalWindow::Ask(const string & text)
{
	mText->SetText(text);
	mCanvas->SetVisible(true);
}

void ModalWindow::AttachTo(shared_ptr<IGUINode> node)
{
	mCanvas->Attach(node);
}

void ModalWindow::SetYesAction(const Delegate & yesAction)
{
	mYesButton->RemoveAllActions();
	mYesButton->AddAction(GUIAction::OnClick, yesAction);
	mYesButton->AddAction(GUIAction::OnClick, [this] { Close(); });
}

void ModalWindow::SetNoAction(const Delegate & noAction)
{
	mNoButton->RemoveAllActions();
	mNoButton->AddAction(GUIAction::OnClick, noAction);
	mNoButton->AddAction(GUIAction::OnClick, [this] { Close(); });
}

void ModalWindow::Close()
{
	mCanvas->SetVisible(false);
}

void ModalWindow::CloseNoAction()
{
	Close();
}
