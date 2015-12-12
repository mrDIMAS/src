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

#include "Precompiled.h"
#include "Engine.h"

#include "GUIRenderer.h"
#include "Cursor.h"





bool ruGUINodeHandle::operator == ( const ruGUINodeHandle & node ) {
    return pointer == node.pointer;
}

bool ruButtonHandle::operator == ( const ruButtonHandle & node ) {
    return pointer == node.pointer;
}

bool ruRectHandle::operator == ( const ruRectHandle & node ) {
    return pointer == node.pointer;
}

bool ruTextHandle::operator == ( const ruTextHandle & node ) {
    return pointer == node.pointer;
}

ruFontHandle ruCreateGUIFont( int size, const string & name ) {
    ruFontHandle font;
    font.pointer = new BitmapFont( name, size );
    return font;
}

ruRectHandle ruCreateGUIRect( float x, float y, float w, float h, ruTextureHandle texture, ruVector3 color, int alpha ) {
    ruRectHandle rect;
    rect.pointer = new GUIRect( x, y, w, h, (Texture*)texture.pointer, color, alpha, true );
    return rect;
}

ruTextHandle ruCreateGUIText( const string & text, int x, int y, int w, int h, ruFontHandle font, ruVector3 color, int textAlign, int alpha ) {
    ruTextHandle t;
    t.pointer = new GUIText( text, x, y, w, h, color, alpha, textAlign, (BitmapFont*)font.pointer );
    return t;
}

ruButtonHandle ruCreateGUIButton( int x, int y, int w, int h, ruTextureHandle texture, const string & text, ruFontHandle font, ruVector3 color, int textAlign, int alpha ) {
    ruButtonHandle button;
    button.pointer = new GUIButton( x, y, w, h, (Texture*)texture.pointer, text, (BitmapFont*)font.pointer, color, textAlign, alpha );
    return button;
}

void ruSetGUINodePosition( ruGUINodeHandle node, float x, float y ) {
    ((GUINode*)node.pointer)->SetPosition( x, y );
}

void ruSetGUINodeSize( ruGUINodeHandle node, float w, float h ) {
    ((GUINode*)node.pointer)->SetSize( w, h );
}

void ruSetGUINodeColor( ruGUINodeHandle node, ruVector3 color ) {
    ((GUINode*)node.pointer)->SetColor( color );
}

void ruSetGUINodeAlpha( ruGUINodeHandle node, int alpha ) {
    ((GUINode*)node.pointer)->SetAlpha( alpha );
}

void ruSetGUINodeVisible( ruGUINodeHandle node, bool visible ) {
    ((GUINode*)node.pointer)->SetVisible( visible );
}

void ruSetGUINodeText( ruTextHandle node, const string & text ) {
    ((GUIText*)node.pointer)->SetText( text );
}

bool ruIsGUINodeVisible( ruGUINodeHandle node ) {
    return ((GUINode*)node.pointer)->IsVisible();
}

ruVector2 ruGetGUINodePosition( ruGUINodeHandle node ) {
    return ((GUINode*)node.pointer)->GetPosition();
}

ruVector2 ruGetGUINodeSize( ruGUINodeHandle node ) {
    return ((GUINode*)node.pointer)->GetSize();
}

ruVector3 ruGetGUINodeColor( ruGUINodeHandle node ) {
    return ((GUINode*)node.pointer)->GetColor();
}

void ruSetGUINodeTexture( ruGUINodeHandle node, ruTextureHandle texture ) {
    ((GUINode*)node.pointer)->SetTexture( (Texture*)texture.pointer );
}

int ruGetGUINodeAlpha( ruGUINodeHandle node ) {
    return ((GUINode*)node.pointer)->GetAlpha();
}

bool ruIsButtonPressed( ruButtonHandle node ) {
    return ((GUIButton*)node.pointer)->IsLeftPressed();
}

bool ruIsButtonPicked( ruButtonHandle node ) {
    return ((GUIButton*)node.pointer)->IsPicked();
}

bool ruIsButtonHit( ruButtonHandle node ) {
    return ((GUIButton*)node.pointer)->IsLeftHit();
}

ruTextHandle ruGetButtonText( ruButtonHandle node ) {
    ruTextHandle text;
    text.pointer = ((GUIButton*)node.pointer)->GetText();
    return text;
}

void ruFreeGUINode( ruGUINodeHandle node ) {
	delete ((GUINode*)node.pointer);
}

void ruAttachGUINode( ruGUINodeHandle node, ruGUINodeHandle parent ) {
	((GUINode*)node.pointer)->AttachTo( (GUINode*)parent.pointer );
}

void ruSetGUIButtonActive( ruButtonHandle button, bool state ) {
	((GUIButton*)button.pointer)->SetActive( state );
}

void ruAddGUINodeAction( ruGUINodeHandle node, ruGUIAction action, const ruDelegate & delegat ) {
	((GUINode*)node.pointer)->AddAction( action, delegat );
}

void ruRemoveGUINodeAction( ruGUINodeHandle node, ruGUIAction action ) {
	((GUINode*)node.pointer)->RemoveAction( action );
}

void ruRemoveAllGUINodeActions( ruGUINodeHandle node ) {
	((GUINode*)node.pointer)->RemoveAllActions();
}

void ruSetGUINodeChildAlphaControl( ruGUINodeHandle node, bool controlChildAlpha ) {
	((GUINode*)node.pointer)->SetControlChildAlpha( controlChildAlpha );
}