/*******************************************************************************
*                               Ruthenium Engine                               *
*            Copyright (c) 2013-2016 Stepanov Dmitriy aka mrDIMAS              *
*                                                                              *
* This file is part of Ruthenium Engine.                                      *
*                                                                              *
* Ruthenium Engine is free software: you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as published by  *
* the Free Software Foundation, either version 3 of the License, or            *
* (at your option) any later version.                                          *
*                                                                              *
* Ruthenium Engine is distributed in the hope that it will be useful,         *
* but WITHOUT ANY WARRANTY; without even the implied warranty of               *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                *
* GNU Lesser General Public License for more details.                          *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with Ruthenium Engine.  If not, see <http://www.gnu.org/licenses/>.   *
*                                                                              *
*******************************************************************************/

#pragma once

#include "BitmapFont.h"

class Texture;

class GUINode : public virtual ruGUINode, public std::enable_shared_from_this<GUINode> {
protected:
	friend class GUIFactory;
	explicit GUINode();
	float mGlobalX;
	float mGlobalY;
    float mX;
    float mY;
    float mWidth;
    float mHeight;
    int mAlpha;
    bool mVisible;
	bool mControlChildAlpha;
	bool mLastMouseInside;
    ruVector3 mColor;
    shared_ptr<Texture> mpTexture;
	vector<shared_ptr<GUINode>> mChildList;
	weak_ptr<GUINode> mParent;
	unordered_map<ruGUIAction, ruEvent> mEventList;
	bool IsGotAction( ruGUIAction act );
		
	virtual void OnClick();
	virtual void OnMouseEnter();
	virtual void OnMouseLeave();	
public:
	void CalculateTransform();
	void DoActions();
    virtual ~GUINode();
    
    float GetX();
    float GetY();
    float GetWidth();
    float GetHeight();
	shared_ptr<GUINode> GetParent();
	bool GotParent();
	virtual void SetColor( ruVector3 color );
	virtual void SetAlpha( int alpha );
    virtual void SetSize( float w, float h );
    virtual void SetVisible( bool visible );
    virtual bool IsVisible();
    virtual shared_ptr<ruTexture> GetTexture( );
    virtual void SetTexture( const shared_ptr<ruTexture> & pTexture );    
    virtual ruVector2 GetPosition( );
	virtual ruVector2 GetGlobalPosition() {
		CalculateTransform();
		return ruVector2( mGlobalX, mGlobalY );
	}
    virtual ruVector2 GetSize( );
    virtual ruVector3 GetColor();
    virtual int GetAlpha();
	virtual bool IsMouseInside();
	virtual void Attach( const shared_ptr<ruGUINode> & parent );
	virtual void SetPosition( float x, float y );
	virtual void SetChildAlphaControl( bool control );
	virtual void AddAction( ruGUIAction act, const ruDelegate & delegat );
	virtual void RemoveAction( ruGUIAction act );
	virtual void RemoveAllActions();
};