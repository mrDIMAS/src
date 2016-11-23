#pragma once

class ModalWindow {
private:
	shared_ptr<IGUINode> mCanvas;
	shared_ptr<IButton> mYesButton;
	shared_ptr<IButton> mNoButton;
	shared_ptr<IText> mText;

public:
	explicit ModalWindow(const shared_ptr<IGUIScene> & scene, int x, int y, int w, int h, shared_ptr<ITexture> texture, shared_ptr<ITexture> buttonTexture, Vector3 color);
	void SetYesAction(const Delegate & yesAction);
	void SetNoAction(const Delegate & noAction);
	void AttachTo(shared_ptr<IGUINode> node);
	void Ask(const string & text);
	void Update();
	void Close();
	void CloseNoAction();
};