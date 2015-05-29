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

	ruAddGUINodeAction( mYesButton, ruGUIAction::OnClick, ruDelegate::Bind( this, &ModalWindow::Close ) );
	ruAddGUINodeAction( mNoButton, ruGUIAction::OnClick, ruDelegate::Bind( this, &ModalWindow::Close ) );

	Close();
}

void ModalWindow::Ask( const string & text ) {
	ruSetGUINodeText( mText, text );
	ruSetGUINodeVisible( mCanvas, true );
}

void ModalWindow::AttachTo( ruGUINodeHandle node ) {
	ruAttachGUINode( mCanvas, node );
}

void ModalWindow::SetYesAction( const ruDelegate & yesAction ) {
	ruRemoveAllGUINodeActions( mYesButton );
	ruAddGUINodeAction( mYesButton, ruGUIAction::OnClick, yesAction );
	ruAddGUINodeAction( mYesButton, ruGUIAction::OnClick, ruDelegate::Bind( this, &ModalWindow::Close ) );
}

void ModalWindow::SetNoAction( const ruDelegate & noAction ) {
	ruRemoveAllGUINodeActions( mNoButton );
	ruAddGUINodeAction( mNoButton, ruGUIAction::OnClick, noAction );
	ruAddGUINodeAction( mNoButton, ruGUIAction::OnClick, ruDelegate::Bind( this, &ModalWindow::Close ) );
}

void ModalWindow::Close() {
	ruSetGUINodeVisible( mCanvas, false );
}
