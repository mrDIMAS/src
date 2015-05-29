#include "Precompiled.h"
#include "ModalWindow.h"
#include "GUI.h"

ModalWindow::ModalWindow( int x, int y, int w, int h, ruTextureHandle texture, ruTextureHandle buttonTexture, ruVector3 color ) {
	int buttonWidth = 128;
	int buttonHeight = 32;

	mCanvas = ruCreateGUIRect( x, y, w, h, texture, color );
	mYesButton = ruCreateGUIButton( 10, h - buttonHeight - 10, buttonWidth, buttonHeight, buttonTexture, "Yes", pGUI->mFont, ruVector3( 255, 255, 255 ), 1 );
	mNoButton = ruCreateGUIButton( w - buttonWidth - 10, h - buttonHeight - 10, buttonWidth, buttonHeight, buttonTexture, "No", pGUI->mFont, ruVector3( 255, 255, 255 ), 1 );
	mText = ruCreateGUIText( "Text", 20, 20, w - 40, h - 40, pGUI->mFont, ruVector3( 255, 255, 255 ), 0 );

	ruAttachGUINode( mYesButton, mCanvas );
	ruAttachGUINode( mNoButton, mCanvas );
	ruAttachGUINode( mText, mCanvas );

	mAnswer = Answer::None;
	mOpen = true;
}

void ModalWindow::Update() {
	if( mOpen ) {
		if( ruIsButtonHit( mNoButton )) {
			mAnswer = Answer::No;
		}
		if( ruIsButtonHit( mYesButton )) {
			mAnswer = Answer::Yes;
		}
		if( mAnswer != Answer::None ) {
			ruSetGUINodeVisible( mCanvas, false );
			mOpen = false;
		};
	}
}

ModalWindow::Answer ModalWindow::GetAnswer() {
	return mAnswer;
}

void ModalWindow::Ask( const string & text ) {
	ruSetGUINodeText( mText, text );
	mAnswer = Answer::None;
	mOpen = true;
	ruSetGUINodeVisible( mCanvas, true );
}

void ModalWindow::AttachTo( ruGUINodeHandle node ) {
	ruAttachGUINode( mCanvas, node );
}

bool ModalWindow::IsAnswered()
{
	return !mOpen;
}

void ModalWindow::Reset()
{
	mAnswer = Answer::None;
}

void ModalWindow::SetYesAction( const ruDelegate & yesAction ) {
	ruRemoveAllGUINodeActions( mYesButton );
	ruAddGUINodeAction( mYesButton, ruGUIAction::OnClick, yesAction );
}

void ModalWindow::SetNoAction( const ruDelegate & noAction ) {
	ruRemoveAllGUINodeActions( mNoButton );
	ruAddGUINodeAction( mNoButton, ruGUIAction::OnClick, noAction );
}
