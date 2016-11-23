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

#include "GUIText.h"
#include "GUIButton.h"
#include "GUIRect.h"
#include "Engine.h"

class GUIScene : public IGUIScene, public enable_shared_from_this<GUIScene> {
private:
	Engine * const mEngine;
	vector<shared_ptr<GUINode>> mNodeList;
	vector<shared_ptr<GUIText>> mTextList;
	vector<shared_ptr<GUIRect>> mRectList;
	vector<shared_ptr<GUIButton>> mButtonList;
	bool mVisible;
	float mOpacity;
	static bool LayerSorter(const shared_ptr<GUINode> & node1, const shared_ptr<GUINode> & node2);
public:
	GUIScene(Engine * engine);
	~GUIScene();

	virtual bool IsVisible() const;
	virtual void SetVisible(bool visible);

	virtual void SetOpacity(float opacity);
	virtual float GetOpacity() const;

	void SortNodes();

	virtual Engine * const GetEngine() const override final;

	vector<shared_ptr<GUINode>> & GetNodeList();
	vector<shared_ptr<GUIText>> & GetTextList();
	vector<shared_ptr<GUIButton>> & GetButtonList();
	vector<shared_ptr<GUIRect>> & GetRectList();

	virtual shared_ptr<IGUINode> CreateNode();
	virtual shared_ptr<IText> CreateText(const string & theText, float theX, float theY, float theWidth, float theHeight, const shared_ptr<IFont> & theFont, Vector3 theColor, TextAlignment theTextAlign, int theAlpha);
	virtual shared_ptr<IRect> CreateRect(float theX, float theY, float theWidth, float theHeight, const shared_ptr<ITexture> & theTexture, Vector3, int theAlpha);
	virtual shared_ptr<IButton> CreateButton(int x, int y, int w, int h, const shared_ptr<ITexture> & texture, const string & text, const shared_ptr<IFont> & font, Vector3 color, TextAlignment textAlign, int alpha);
};
