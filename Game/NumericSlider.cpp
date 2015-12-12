#include "Precompiled.h"

#include "NumericSlider.h"
#include "GUIProperties.h"

Slider::Slider( float x, float y, float minimum, float maximum, float step, ruTextureHandle buttonImage, const string & text ) {
    float buttonWidth = 32;
    float buttonHeight = 32;

    int textHeight = 16;
    int captionWidth = 100;

    mValue = maximum;

    mfMinimum = minimum;
    mfMaximum = maximum;

    mfStep = step;

    mGUIText = ruCreateGUIText( text, x, y + textHeight / 2, captionWidth, textHeight, pGUIProp->mFont, pGUIProp->mForeColor, 0 );
    mGUIValueText = ruCreateGUIText( "Value", x + captionWidth + buttonWidth * 1.25f, y  + textHeight / 2, 3.15f * buttonWidth, 16, pGUIProp->mFont, pGUIProp->mForeColor, 1 );

    mGUIIncreaseButton = ruCreateGUIButton( x + captionWidth + 4.0f * buttonWidth, y, buttonWidth, buttonHeight, buttonImage, ">", pGUIProp->mFont, pGUIProp->mForeColor, 1 );
	ruAddGUINodeAction( mGUIIncreaseButton, ruGUIAction::OnClick, ruDelegate::Bind( this, &Slider::OnIncreaseClick ));

    mGUIDecreaseButton = ruCreateGUIButton( x + captionWidth + buttonWidth, y, buttonWidth, buttonHeight, buttonImage, "<", pGUIProp->mFont, pGUIProp->mForeColor, 1 );
	ruAddGUINodeAction( mGUIDecreaseButton, ruGUIAction::OnClick, ruDelegate::Bind( this, &Slider::OnDecreaseClick ));
}

int Slider::GetWidth() {
	return 4 * 32 + 100 + 32; // HARD code :)
}

int Slider::GetHeight() {
	return 32;
}

void Slider::SetValue( float value ) {
    mValue = value;
	UpdateText();
}

float Slider::GetValue() {
    return mValue;
}

Slider::~Slider() {
    ruFreeGUINode( mGUIValueText );
    ruFreeGUINode( mGUIText );
    ruFreeGUINode( mGUIIncreaseButton );
    ruFreeGUINode( mGUIDecreaseButton );
}

void Slider::SetChangeAction( const ruDelegate & action ) {
	ruRemoveAllGUINodeActions( mGUIIncreaseButton );
	ruAddGUINodeAction( mGUIIncreaseButton, ruGUIAction::OnClick, ruDelegate::Bind( this, &Slider::OnIncreaseClick ));
	ruAddGUINodeAction( mGUIIncreaseButton, ruGUIAction::OnClick, action );

	ruRemoveAllGUINodeActions( mGUIDecreaseButton );
	ruAddGUINodeAction( mGUIDecreaseButton, ruGUIAction::OnClick, ruDelegate::Bind( this, &Slider::OnDecreaseClick ));
	ruAddGUINodeAction( mGUIDecreaseButton, ruGUIAction::OnClick, action );
}

void Slider::AttachTo( ruGUINodeHandle node ) {
	ruAttachGUINode( mGUIIncreaseButton, node );
	ruAttachGUINode( mGUIDecreaseButton, node );
	ruAttachGUINode( mGUIText, node );
	ruAttachGUINode( mGUIValueText, node );
}

void Slider::OnDecreaseClick() {
	if( mValue > mfMinimum ) {
		mValue -= mfStep;
	}		
	UpdateText();
}

void Slider::OnIncreaseClick() {
	if( mValue < mfMaximum ) {
		mValue += mfStep;
	}		
	UpdateText();
}

void Slider::UpdateText() {
	ruSetGUINodeText( mGUIValueText, StringBuilder() << fixed << setprecision( 1 ) << mValue );
}
