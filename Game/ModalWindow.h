#pragma once

class ModalWindow {
private:
	ruGUINode * mCanvas;
	ruButton * mYesButton;
	ruButton * mNoButton;
	ruText * mText;
	
public:
	explicit ModalWindow( int x, int y, int w, int h, shared_ptr<ruTexture> texture,  shared_ptr<ruTexture> buttonTexture, ruVector3 color );
	void SetYesAction( const ruDelegate & yesAction );
	void SetNoAction( const ruDelegate & noAction );
	void AttachTo( ruGUINode * node );
	void Ask( const string & text );
	void Update();
	void Close();
	void CloseNoAction();
};