#include "Precompiled.h"

#include "NumericSlider.h"
#include "GUIProperties.h"

Slider::Slider(const shared_ptr<ruGUIScene> & scene, float x, float y, float minimum, float maximum, float step, shared_ptr<ruTexture> buttonImage, const string & text ) {
    float buttonWidth = 32;
    float buttonHeight = 32;

    int textHeight = 16;
    int captionWidth = 100;

    mValue = maximum;

    mfMinimum = minimum;
    mfMaximum = maximum;

    mfStep = step;

    mGUIText = scene->CreateText( text, x, y + textHeight / 2, captionWidth, textHeight, pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Left );
    mGUIValueText = scene->CreateText( " ", x + captionWidth + buttonWidth * 1.25f, y  + textHeight / 2, 3.15f * buttonWidth, 16, pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center );

    mGUIIncreaseButton = scene->CreateButton( x + captionWidth + 4.0f * buttonWidth, y, buttonWidth, buttonHeight, buttonImage, ">", pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center );
	mGUIIncreaseButton->AddAction(ruGUIAction::OnClick, [this] { OnIncreaseClick(); });

    mGUIDecreaseButton = scene->CreateButton( x + captionWidth + buttonWidth, y, buttonWidth, buttonHeight, buttonImage, "<", pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center );
	mGUIDecreaseButton->AddAction(ruGUIAction::OnClick, [this] { OnDecreaseClick(); });
}

int Slider::GetWidth() {
	return 4 * 32 + 100 + 32; // hardcodin' this shit
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

}

void Slider::SetChangeAction( const ruDelegate & action ) {
	mGUIIncreaseButton->RemoveAllActions();
	mGUIIncreaseButton->AddAction(ruGUIAction::OnClick, [this] { OnIncreaseClick(); });
	mGUIIncreaseButton->AddAction( ruGUIAction::OnClick, action );

	mGUIDecreaseButton->RemoveAllActions();
	mGUIDecreaseButton->AddAction(ruGUIAction::OnClick, [this] { OnDecreaseClick(); });
	mGUIDecreaseButton->AddAction( ruGUIAction::OnClick, action );
}

void Slider::AttachTo( shared_ptr<ruGUINode> node ) {
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
