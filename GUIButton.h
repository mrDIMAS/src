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

#include "GUIRect.h"
#include "GUIText.h"

class GUIButton : public GUIRect, public IButton {
private:
	bool mPicked;
	bool mLeftPressed;
	bool mRightPressed;
	bool mLeftHit;
	bool mRightHit;
	bool mActive;
	Vector3 mInitColor;
	Vector3 pickedColor;
	shared_ptr<GUIText> mpText;
	virtual void OnClick( ) {
		// action defined in Update()
	}
public:

	explicit GUIButton( const weak_ptr<GUIScene> & scene, int x, int y, int w, int h, shared_ptr<Texture> texture, const string & text, const shared_ptr<BitmapFont> & font, Vector3 color, TextAlignment textAlign, int alpha );
	virtual ~GUIButton( );
	void Update( ); // must be called inside ruInputUpdate

	virtual bool IsRightPressed( );
	virtual Vector3 GetPickedColor( ) const;
	virtual bool IsRightHit( ) const;
	virtual void SetPickedColor( Vector3 val );
	virtual bool IsHit( ) const;
	virtual void SetAlpha( int alpha );
	virtual void SetActive( bool state );
	virtual bool IsPicked( );
	virtual shared_ptr<IText> GetText( );
	virtual bool IsPressed( );
};