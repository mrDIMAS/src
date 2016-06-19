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

#include "GUIText.h"
#include "GUIButton.h"
#include "GUIRect.h"

class GUIScene : public ruGUIScene, public enable_shared_from_this<GUIScene> {
private:
	template<typename Type>
	static void RemoveUnreferenced(vector<weak_ptr<Type>> & objList);
	vector<shared_ptr<GUINode>> mNodeList;
	vector<shared_ptr<GUIText>> mTextList;
	vector<shared_ptr<GUIRect>> mRectList;
	vector<shared_ptr<GUIButton>> mButtonList;
	static vector<weak_ptr<GUIScene>> msSceneList;
	bool mVisible;
	float mOpacity;
	static bool LayerSorter(const shared_ptr<GUINode> & node1, const shared_ptr<GUINode> & node2);
private:
	GUIScene(); // use Create instead
public:
	~GUIScene();

	static shared_ptr<GUIScene> Create();
	static const vector<weak_ptr<GUIScene>> & GetSceneList();

	virtual bool IsVisible() const;
	virtual void SetVisible(bool visible);

	virtual void SetOpacity(float opacity);
	virtual float GetOpacity() const;

	void SortNodes();

	vector<shared_ptr<GUINode>> & GetNodeList();
	vector<shared_ptr<GUIText>> & GetTextList();
	vector<shared_ptr<GUIButton>> & GetButtonList();
	vector<shared_ptr<GUIRect>> & GetRectList();

	virtual shared_ptr<ruGUINode> CreateNode();
	virtual shared_ptr<ruText> CreateText(const string & theText, float theX, float theY, float theWidth, float theHeight, const shared_ptr<ruFont> & theFont, ruVector3 theColor, ruTextAlignment theTextAlign, int theAlpha);
	virtual shared_ptr<ruRect> CreateRect(float theX, float theY, float theWidth, float theHeight, const shared_ptr<ruTexture> & theTexture, ruVector3, int theAlpha);
	virtual shared_ptr<ruButton> CreateButton(int x, int y, int w, int h, const shared_ptr<ruTexture> & texture, const string & text, const shared_ptr<ruFont> & font, ruVector3 color, ruTextAlignment textAlign, int alpha);
};

template<typename Type>
inline void GUIScene::RemoveUnreferenced(vector<weak_ptr<Type>>& objList) {
	for (auto iter = objList.begin(); iter != objList.end(); ) {
		if ((*iter).use_count()) {
			++iter;
		} else {
			iter = objList.erase(iter);
		}
	}
}
