/*******************************************************************************
*                               Ruthenium Engine                               *
*            Copyright (c) 2013-2017 Stepanov Dmitriy aka mrDIMAS              *
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
class GUIScene;

class GUINode : public virtual IGUINode, public std::enable_shared_from_this<GUINode> {
protected:
	friend class GUIScene;
	explicit GUINode( const weak_ptr<GUIScene> & scene );
	weak_ptr<GUIScene> mScene;
	float mGlobalX;
	float mGlobalY;
	float mX;
	float mY;
	float mWidth;
	float mHeight;
	int mAlpha;
	int mLayer;
	bool mVisible;
	bool mLastMouseInside;
	Vector3 mColor;
	shared_ptr<Texture> mpTexture;
	vector<shared_ptr<GUINode>> mChildList;
	weak_ptr<GUINode> mParent;
	unordered_map<GUIAction, Event> mEventList;
	bool IsGotAction( GUIAction act );
	bool mIndependentAlpha;
	virtual void OnClick( );
	virtual void OnMouseEnter( );
	virtual void OnMouseLeave( );

public:
	void CalculateTransform( );
	void DoActions( );
	virtual ~GUINode( );

	float GetX( );
	float GetY( );
	float GetWidth( );
	float GetHeight( );
	shared_ptr<GUINode> GetParent( );
	bool GotParent( );
	virtual void SetColor( Vector3 color );
	virtual void SetAlpha( int alpha );
	virtual void SetSize( float w, float h );
	virtual void SetVisible( bool visible );
	virtual bool IsVisible( ) const;
	virtual shared_ptr<ITexture> GetTexture( );
	virtual void SetTexture( const shared_ptr<ITexture> & pTexture );
	virtual Vector2 GetPosition( );
	virtual Vector2 GetGlobalPosition( );
	virtual void SetIndependentAlpha( bool useIndependent );
	virtual bool IsIndependentAlpha( ) const;
	virtual Vector2 GetSize( );
	virtual Vector3 GetColor( );
	virtual int GetAlpha( );
	virtual bool IsMouseInside( );
	virtual void Attach( const shared_ptr<IGUINode> & parent );
	virtual void SetPosition( float x, float y );
	virtual void AddAction( GUIAction act, const Delegate & delegat );
	virtual void RemoveAction( GUIAction act );
	virtual void RemoveAllActions( );
	virtual weak_ptr<class IGUIScene> GetScene( );
	virtual void SetLayer( int layer );
	virtual int GetLayer( ) const;
	virtual void Move( const Vector2 & speed ) override;
};