#include "Precompiled.h"

#include "GUINode.h"

vector<GUINode*> GUINode::msNodeList;

void GUINode::SetAlpha( int alpha ) {
    mAlpha = alpha;
	if( mControlChildAlpha ) {
		for( auto pChild : mChildList ) {
			pChild->SetAlpha( alpha );
		}
	}
    PackColor();
}

void GUINode::SetColor( ruVector3 color ) {
    mColor = color;
    PackColor();
}

void GUINode::PackColor() {
    mColorPacked = D3DCOLOR_ARGB( mAlpha, (int)mColor.x, (int)mColor.y, (int)mColor.z );
}

GUINode::GUINode() {
    mX = 0;
    mY = 0;
    mWidth = 0;
    mHeight = 0;
    mVisible = true;
	mGlobalX = 0.0f;
	mGlobalY = 0.0f;
	mControlChildAlpha = false;
	mLastMouseInside = false;
    SetColor( ruVector3( 255, 255, 255 ));
    SetAlpha( 255 );
	mParent = nullptr;
    msNodeList.push_back( this );
}

GUINode::~GUINode() {
	for( auto pNode : msNodeList ) {
		if( pNode->mParent == this ) {
			pNode->mParent = nullptr;
		}
	}
    msNodeList.erase( find( msNodeList.begin(), msNodeList.end(), this ));
}

void GUINode::SetTexture( Texture * pTexture ) {
    mpTexture = pTexture;
}

Texture * GUINode::GetTexture() {
    return mpTexture;
}

bool GUINode::IsVisible() {
	bool visibility = mVisible;
	if( mParent ) {
		visibility &= mParent->IsVisible();
	}
    return visibility;
}

void GUINode::SetVisible( bool visible ) {
    mVisible = visible;
}

void GUINode::SetSize( float w, float h ) {
    mWidth = w;
    mHeight = h;
}

float GUINode::GetHeight() {
    return mHeight;
}

float GUINode::GetWidth() {
    return mWidth;
}

float GUINode::GetY() {
    return mY;
}

float GUINode::GetX() {
    return mX;
}

int GUINode::GetAlpha() {
    return mAlpha;
}

ruVector3 GUINode::GetColor() {
    return mColor;
}

ruVector2 GUINode::GetSize() {
    return ruVector2( mWidth, mHeight );
}

ruVector2 GUINode::GetPosition() {
    return ruVector2( mX, mY );
}

void GUINode::SetPosition( float x, float y ) {
    mX = x;
    mY = y;
}

int GUINode::GetPackedColor() {
    return mColorPacked;
}

void GUINode::AttachTo( GUINode * parent ) {
	parent->mChildList.push_back( this );
	mParent = parent;
}

void GUINode::CalculateTransform() {
	mGlobalX = 0.0f;
	mGlobalY = 0.0f;
	if( mParent ) {
		mParent->CalculateTransform();
		mGlobalX = mX + mParent->mGlobalX;
		mGlobalY = mY + mParent->mGlobalY;
	} else {
		mGlobalX = mX;
		mGlobalY = mY;
	}
}

void GUINode::AddAction( ruGUIAction act, const ruDelegate & delegat ) {
	mEventList[ act ].AddListener( delegat );
}

bool GUINode::IsGotAction( ruGUIAction act ) {
	auto iter = mEventList.find( act );
	return iter != mEventList.end();
}

bool GUINode::IsMouseInside() {
	CalculateTransform();
	int mouseX = ruGetMouseX();
	int mouseY = ruGetMouseY();
	return mouseX > mGlobalX && mouseX < ( mGlobalX + mWidth ) && mouseY > mGlobalY && mouseY < ( mGlobalY + mHeight );
}

void GUINode::OnMouseEnter() {
	if( IsGotAction( ruGUIAction::OnMouseEnter )) {
		mEventList[ ruGUIAction::OnMouseEnter ].DoActions();
	}
}

void GUINode::OnMouseLeave() {
	if( IsGotAction( ruGUIAction::OnMouseLeave )) {
		mEventList[ ruGUIAction::OnMouseLeave ].DoActions();
	}
}

void GUINode::DoActions() {
	if( IsMouseInside() ) {
		if( ruIsMouseHit( MB_Left )) {
			OnClick();
		}
		if( !mLastMouseInside ) {
			OnMouseEnter();
		} 
	} else {
		if( mLastMouseInside ) {
			OnMouseLeave();
		}
	}
}

void GUINode::OnClick() {
	if( IsGotAction( ruGUIAction::OnClick )) {
		mEventList[ ruGUIAction::OnClick ].DoActions();
	}
}

void GUINode::RemoveAllActions()
{
	mEventList.clear();
}

void GUINode::RemoveAction( ruGUIAction act )
{
	mEventList.erase( act );
}

void GUINode::SetControlChildAlpha( bool control )
{
	mControlChildAlpha = control;
}
