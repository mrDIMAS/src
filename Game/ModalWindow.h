#pragma once

class ModalWindow {
private:
	ruGUINodeHandle mCanvas;
	ruButtonHandle mYesButton;
	ruButtonHandle mNoButton;
	ruTextHandle mText;
	
public:
	explicit ModalWindow( int x, int y, int w, int h, ruTextureHandle texture,  ruTextureHandle buttonTexture, ruVector3 color );
	void SetYesAction( const ruDelegate & yesAction );
	void SetNoAction( const ruDelegate & noAction );
	void AttachTo( ruGUINodeHandle node );
	void Ask( const string & text );
	void Update();
	void Close();
	void CloseNoAction();
};