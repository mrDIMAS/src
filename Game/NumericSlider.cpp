#include "Precompiled.h"

#include "NumericSlider.h"
#include "GUIProperties.h"

Slider::Slider( float x, float y, float minimum, float maximum, float step, shared_ptr<ruTexture> buttonImage, const string & text ) {
    float buttonWidth = 32;
    float buttonHeight = 32;

    int textHeight = 16;
    int captionWidth = 100;

    mValue = maximum;

    mfMinimum = minimum;
    mfMaximum = maximum;

    mfStep = step;

    mGUIText = ruText::Create( text, x, y + textHeight / 2, captionWidth, textHeight, pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Left );
    mGUIValueText = ruText::Create( "Value", x + captionWidth + buttonWidth * 1.25f, y  + textHeight / 2, 3.15f * buttonWidth, 16, pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center );

    mGUIIncreaseButton = ruButton::Create( x + captionWidth + 4.0f * buttonWidth, y, buttonWidth, buttonHeight, buttonImage, ">", pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center );
	mGUIIncreaseButton->AddAction( ruGUIAction::OnClick, ruDelegate::Bind( this, &Slider::OnIncreaseClick ));

    mGUIDecreaseButton = ruButton::Create( x + captionWidth + buttonWidth, y, buttonWidth, buttonHeight, buttonImage, "<", pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center );
	mGUIDecreaseButton->AddAction( ruGUIAction::OnClick, ruDelegate::Bind( this, &Slider::OnDecreaseClick ));
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
    mGUIValueText->Free( );
    mGUIText->Free( );
    mGUIIncreaseButton->Free( );
    mGUIDecreaseButton->Free( );
}

void Slider::SetChangeAction( const ruDelegate & action ) {
	mGUIIncreaseButton->RemoveAllActions();
	mGUIIncreaseButton->AddAction( ruGUIAction::OnClick, ruDelegate::Bind( this, &Slider::OnIncreaseClick ));
	mGUIIncreaseButton->AddAction( ruGUIAction::OnClick, action );

	mGUIDecreaseButton->RemoveAllActions();
	mGUIDecreaseButton->AddAction( ruGUIAction::OnClick, ruDelegate::Bind( this, &Slider::OnDecreaseClick ));
	mGUIDecreaseButton->AddAction( ruGUIAction::OnClick, action );
}

void Slider::AttachTo( ruGUINode * node ) {
	mGUIIncreaseButton->Attach( node );
	mGUIDecreaseButton->Attach( node );
	mGUIText->Attach( node );
	mGUIValueText->Attach( node );
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
	mGUIValueText->SetText( StringBuilder() << fixed << setprecision( 1 ) << mValue );
}
