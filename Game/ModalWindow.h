#pragma once

class ModalWindow {
public:
	enum class Answer {
		None,
		Yes,
		No,
	};

private:
	ruGUINodeHandle mCanvas;
	ruButtonHandle mYesButton;
	ruButtonHandle mNoButton;
	ruTextHandle mText;
	Answer mAnswer;
	bool mOpen;

public:
	bool IsAnswered();
	explicit ModalWindow( int x, int y, int w, int h, ruTextureHandle texture,  ruTextureHandle buttonTexture, ruVector3 color );
	void SetYesAction( const ruDelegate & yesAction );
	void SetNoAction( const ruDelegate & noAction );
	void AttachTo( ruGUINodeHandle node );
	void Ask( const string & text );
	Answer GetAnswer();
	void Reset();
	void Update();

};