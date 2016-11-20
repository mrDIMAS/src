#pragma once

class ModalWindow {
private:
	shared_ptr<ruGUINode> mCanvas;
	shared_ptr<ruButton> mYesButton;
	shared_ptr<ruButton> mNoButton;
	shared_ptr<ruText> mText;

public:
	explicit ModalWindow(const shared_ptr<ruGUIScene> & scene, int x, int y, int w, int h, shared_ptr<ruTexture> texture, shared_ptr<ruTexture> buttonTexture, ruVector3 color);
	void SetYesAction(const ruDelegate & yesAction);
	void SetNoAction(const ruDelegate & noAction);
	void AttachTo(shared_ptr<ruGUINode> node);
	void Ask(const string & text);
	void Update();
	void Close();
	void CloseNoAction();
};