#include "Precompiled.h"

#include "Inventory.h"
#include "GUIProperties.h"
#include "Player.h"
#include "Level.h"

Inventory::Inventory() :
	mOpen(false),
	mpSelectedItem(nullptr),
	mpCombineItemFirst(nullptr),
	mpCombineItemSecond(nullptr),
	mpItemForUse(nullptr)
{
	mLocalization.ParseFile(gLocalizationPath + "inventory.loc");

	mScene = ruGUIScene::Create();

	mBackgroundTexture = ruTexture::Request("data/gui/inventory/back.tga");
	mCellTexture = ruTexture::Request("data/gui/inventory/item.tga");
	mButtonTexture = ruTexture::Request("data/gui/inventory/button.tga");
	mFont = ruFont::LoadFromFile(14, "data/fonts/font5.ttf");
	mPickSound = ruSound::Load2D("data/sounds/menupick.ogg");

	int screenCenterX = ruVirtualScreenWidth / 2;
	int screenCenterY = ruVirtualScreenHeight / 2;

	

	float distMult = 1.1f;
	int cellSpaceX = distMult * mCellWidth / (float)mCellCountWidth;
	int cellSpaceY = distMult * mCellHeight / (float)mCellCountHeight;
	int coordX = screenCenterX - 0.5f * (float)mCellCountWidth * (float)mCellWidth - cellSpaceX - 64;
	int coordY = screenCenterY - 0.5f * (float)mCellCountHeight * (float)mCellHeight - cellSpaceY - 64;


	// background
	int backGroundSpace = 20;
	int backgroundX = coordX - backGroundSpace;
	int backgroundY = coordY - backGroundSpace;
	int backgroundW = mCellCountWidth * mCellWidth + 2.5 * backGroundSpace + cellSpaceX + 128;
	int backgroundH = mCellCountHeight * mCellHeight + 2.5 * backGroundSpace + cellSpaceY + 128;

	mPageItems = mScene->CreateButton(backgroundX, backgroundY - 32, 120, 32, ruTexture::Request("data/gui/inventory/button2.png"), mLocalization.GetString("pageItems"), mFont, pGUIProp->mForeColor);
	mPageNotes = mScene->CreateButton(backgroundX + 120, backgroundY - 32, 120, 32, ruTexture::Request("data/gui/inventory/button2.png"), mLocalization.GetString("pageNotes"), mFont, pGUIProp->mForeColor);


	mGUICanvas = mScene->CreateRect(backgroundX, backgroundY, backgroundW, backgroundH, mBackgroundTexture, pGUIProp->mBackColor);
	int combineH = 128;
	int combineY = backgroundY + backgroundH - 128;
	int descriptionY = combineY + 10;
	mGUIDescription = mScene->CreateText(mLocalization.GetString("desc"), backgroundX + 10, descriptionY, backgroundW - 128, combineH, mFont, pGUIProp->mForeColor, ruTextAlignment::Left);
	// item actions
	int actionsW = 128;
	int actionsX = backgroundX + backgroundW - 128;

	//  actions buttons
	int buttonSpace = 10;
	int buttonsX = actionsX + buttonSpace;
	int buttonY = backgroundY + 2 * buttonSpace;
	int buttonH = 30;
	int buttonW = actionsW - 2 * buttonSpace;
	mGUIButtonUse = mScene->CreateButton(buttonsX, buttonY, buttonW, buttonH, mButtonTexture, mLocalization.GetString("use"), mFont, pGUIProp->mForeColor, ruTextAlignment::Center, 255);
	mGUIButtonCombine = mScene->CreateButton(buttonsX, buttonY + 1.5f * buttonH, buttonW, buttonH, mButtonTexture, mLocalization.GetString("combine"), mFont, pGUIProp->mForeColor, ruTextAlignment::Center, 255);
	// combine items
	int combineBoxY = buttonY + 3.6f * buttonH;
	ruVector3 combineColor1 = pGUIProp->mForeColor;
	ruVector3 combineColor2 = pGUIProp->mForeColor;
	int combineBoxSpacing = 5;
	buttonsX += mCellWidth / 2 - 2 * combineBoxSpacing;
	mGUIFirstCombineItem = mScene->CreateRect(buttonsX + combineBoxSpacing, combineBoxY + combineBoxSpacing, mCellWidth - 2 * combineBoxSpacing, mCellHeight - 2 * combineBoxSpacing, nullptr);
	mGUISecondCombineItem = mScene->CreateRect(buttonsX + combineBoxSpacing, combineBoxY + 1.2f * mCellHeight + combineBoxSpacing, mCellWidth - 2 * combineBoxSpacing, mCellHeight - 2 * combineBoxSpacing, nullptr);
	mGUIFirstCombineItemCell = mScene->CreateRect(buttonsX, combineBoxY, mCellWidth, mCellHeight, mCellTexture, combineColor1, 255);
	mGUISecondCombineItemCell = mScene->CreateRect(buttonsX, combineBoxY + 1.2f * mCellHeight, mCellWidth, mCellHeight, mCellTexture, combineColor2, 255);
	mGUICharacteristics = mScene->CreateText(mLocalization.GetString("characteristics"), actionsX, combineBoxY + 1.5f * mCellHeight, 128, combineH, mFont, pGUIProp->mForeColor, ruTextAlignment::Center);

	int itemSpacing = 5;
	for (int cw = 0; cw < mCellCountWidth; cw++) {
		for (int ch = 0; ch < mCellCountHeight; ch++) {
			int cellX = coordX + distMult * mCellWidth * cw;
			int cellY = coordY + distMult * mCellHeight * ch;
			mGUIItemCell[cw][ch] = mScene->CreateRect(cellX, cellY, mCellWidth, mCellHeight, mCellTexture, pGUIProp->mForeColor, 255);
			mGUIItem[cw][ch] = mScene->CreateRect(cellX + itemSpacing, cellY + itemSpacing, mCellWidth - 2 * itemSpacing, mCellHeight - 2 * itemSpacing, nullptr, ruVector3(255, 255, 255), 255);
			mGUIItemCountText[cw][ch] = mScene->CreateText("0", cellX + distMult * mCellWidth - 18, cellY + distMult * mCellHeight - 24, 8, 8, mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
		}
	}

	int offset = combineBoxY + 2.2f * mCellHeight;
	mGUIItemDescription = mScene->CreateText("Desc", backgroundX + 2 * itemSpacing, descriptionY + 2.5 * itemSpacing, backgroundW - 2 * itemSpacing - 128, combineH - 2 * itemSpacing, pGUIProp->mFont, ruVector3(200, 200, 200), ruTextAlignment::Left, 255);
	// characteristics of item
	int charSpace = 28;
	mGUIItemContentType = mScene->CreateText("ContentType", actionsX + buttonSpace, offset + charSpace * 1, actionsW - itemSpacing, 100, mFont, pGUIProp->mForeColor, ruTextAlignment::Left);
	mGUIItemContent = mScene->CreateText("Content", actionsX + buttonSpace, offset + charSpace * 2.1, actionsW - itemSpacing, 100, mFont, pGUIProp->mForeColor, ruTextAlignment::Left);
	mGUIItemVolume = mScene->CreateText("Volume", actionsX + buttonSpace, offset + charSpace * 3.1, actionsW - itemSpacing, 100, mFont, pGUIProp->mForeColor, ruTextAlignment::Left);
	mGUIItemMass = mScene->CreateText("Mass", actionsX + buttonSpace, offset + charSpace * 4.1, actionsW - itemSpacing, 100, mFont, pGUIProp->mForeColor, ruTextAlignment::Left);

	SetVisible(false);
}

void Inventory::SetVisible(bool state) {
	mOpen = state;
	mScene->SetVisible(state);
}

void Inventory::DoCombine() {
	if (mpCombineItemFirst->Combine(mpCombineItemSecond->GetType())) { // combine successfull
		mpCombineItemFirst = nullptr;
		mpCombineItemSecond = nullptr;
		mpSelectedItem = nullptr;
	}
}

void Inventory::Update() {
	ruVector3 whiteColor = ruVector3(255, 255, 255);
	int screenCenterX = ruVirtualScreenWidth / 2;
	int screenCenterY = ruVirtualScreenHeight / 2;

	if (mpItemForUse) {
		ruEngine::HideCursor();
		SetVisible(false);
		Level::Current()->GetPlayer()->GetHUD()->ShowUsedItem(mpItemForUse);

		if (ruInput::IsMouseHit(ruInput::MouseButton::Left)) {
			Level::Current()->GetPlayer()->GetHUD()->ShowUsedItem(nullptr);
			mpItemForUse = nullptr;
			mpSelectedItem = nullptr;
			SetVisible(true);
		}
		if (mOpen) {
			mpItemForUse = nullptr;
		}
		return;
	} else {
		Level::Current()->GetPlayer()->GetHUD()->ShowUsedItem(nullptr); // HAAAAX!!!!
	}

	if (!mOpen) {
		ruEngine::HideCursor();
		return;
	}

	ruEngine::ShowCursor();

	int useAlpha = mpSelectedItem ? 255 : 60;
	mGUIButtonUse->SetAlpha(useAlpha);
	mGUIButtonUse->GetText()->SetAlpha(useAlpha);

	bool canCombine = (mpCombineItemFirst != nullptr && mpCombineItemSecond != nullptr);

	int combineAlpha = canCombine ? 255 : 60;
	mGUIButtonCombine->SetAlpha(combineAlpha);
	mGUIButtonCombine->GetText()->SetAlpha(combineAlpha);

	// draw combine items
	ruVector3 combineColor1 = pGUIProp->mForeColor;
	ruVector3 combineColor2 = pGUIProp->mForeColor;
	int combineBoxSpacing = 5;

	mGUIFirstCombineItem->SetTexture(nullptr);
	mGUISecondCombineItem->SetTexture(nullptr);

	// show first combining item
	if (mpCombineItemFirst) {
		mGUIFirstCombineItem->SetVisible(true);
		mGUIFirstCombineItem->SetTexture(mpCombineItemFirst->GetPictogram());
		if (mGUIFirstCombineItem->IsMouseInside()) {
			combineColor1 = ruVector3(255, 0, 0);
			if (ruInput::IsMouseHit(ruInput::MouseButton::Left)) {
				mpCombineItemFirst = nullptr;
			}
		}
	} else {
		mGUIFirstCombineItem->SetVisible(false);
	};
	mGUIFirstCombineItemCell->SetColor(combineColor1);

	// show second combining item
	if (mpCombineItemSecond) {
		mGUISecondCombineItem->SetVisible(true);
		mGUISecondCombineItem->SetTexture(mpCombineItemSecond->GetPictogram());
		if (mGUISecondCombineItem->IsMouseInside()) {
			combineColor2 = ruVector3(255, 0, 0);
			if (ruInput::IsMouseHit(ruInput::MouseButton::Left)) {
				mpCombineItemSecond = nullptr;
			}
		}
	} else {
		mGUISecondCombineItem->SetVisible(false);
	}
	mGUISecondCombineItemCell->SetColor(combineColor2);

	// do combine
	if (mGUIButtonCombine->IsHit()) {
		if (canCombine) {
			DoCombine();
		}
	}

	// use item
	if (mGUIButtonUse->IsHit()) {
		if (mpSelectedItem) {
			mpItemForUse = mpSelectedItem;
			mOpen = false;
		}
	}

	mGUIItemDescription->SetVisible(false);
	mGUIItemContentType->SetVisible(false);
	mGUIItemContent->SetVisible(false);
	mGUIItemVolume->SetVisible(false);
	mGUIItemMass->SetVisible(false);


	bool combinePick = true;
	for (int cw = 0; cw < mCellCountWidth; cw++) {
		for (int ch = 0; ch < mCellCountHeight; ch++) {			
			ruVector3 color = pGUIProp->mForeColor;

			int itemNum = cw * mCellCountHeight + ch;
			
			int i = 0;

			Item * pItem = nullptr;
			int curItemCount = 0;
			for (auto & itemCountPair = mItemMap.begin(); itemCountPair != mItemMap.end(); itemCountPair++) {
				if (i == itemNum) {
					pItem = const_cast<Item*>(&itemCountPair->first);
					curItemCount = itemCountPair->second;
					break;
				}
				i++;
			}
			if (pItem) {
				if (mpSelectedItem == pItem) {
					color = ruVector3(0, 200, 0);
				}
				mGUIItemCountText[cw][ch]->SetText(StringBuilder() << curItemCount);
				if (pItem != mpCombineItemFirst && pItem != mpCombineItemSecond) {
					mGUIItemCountText[cw][ch]->SetVisible(true);
				} else {
					mGUIItemCountText[cw][ch]->SetVisible(false);
				}
			} else {
				mGUIItemCountText[cw][ch]->SetVisible(false);
			}

			Item * pPicked = nullptr;
			if (mGUIItemCell[cw][ch]->IsMouseInside()) {
				color = ruVector3(255, 0, 0);
				if (pItem != mpCombineItemFirst && pItem != mpCombineItemSecond) {
					pPicked = pItem;
					if (ruInput::IsMouseHit(ruInput::MouseButton::Left)) {
						mpSelectedItem = pItem;
					}
				}
			}

			mGUIItemCell[cw][ch]->SetColor(color);
			if (pPicked) {
				if (ruInput::IsMouseHit(ruInput::MouseButton::Right) && combinePick) {
					if (mpCombineItemFirst == nullptr) {
						if (pPicked != mpCombineItemFirst) {
							mpCombineItemFirst = pPicked;
						}
					} else if (mpCombineItemSecond == nullptr) {
						if (pPicked != mpCombineItemSecond) {
							mpCombineItemSecond = pPicked;
						}
					}

					combinePick = false;
					mpSelectedItem = nullptr;
				}
			}
			if (pItem) {
				if (pItem != mpCombineItemFirst && pItem != mpCombineItemSecond) {
					mGUIItem[cw][ch]->SetVisible(true);
					mGUIItem[cw][ch]->SetTexture(pItem->GetPictogram());
					if (pItem == pPicked) {
						mGUIItemDescription->SetVisible(true);
						mGUIItemContentType->SetVisible(true);
						mGUIItemContent->SetVisible(true);
						mGUIItemVolume->SetVisible(true);
						mGUIItemMass->SetVisible(true);
						mGUIItemDescription->SetText(pItem->GetDescription());
						mGUIItemContentType->SetText(StringBuilder() << mLocalization.GetString("contentType") << ": " << pItem->GetContentType());
						mGUIItemContent->SetText(StringBuilder() << mLocalization.GetString("content") << ": " << pItem->GetContent());
						mGUIItemVolume->SetText(StringBuilder() << mLocalization.GetString("volume") << ": " << pItem->GetVolume());
						mGUIItemMass->SetText(StringBuilder() << mLocalization.GetString("mass") << ": " << pItem->GetMass());
					}
				} else {
					mGUIItem[cw][ch]->SetVisible(false);
				}
			} else {
				mGUIItem[cw][ch]->SetVisible(false);
			}
		}
	}
}

void Inventory::RemoveItem(Item::Type type, int count) {
	for (auto & itemCountPair = mItemMap.begin(); itemCountPair != mItemMap.end(); itemCountPair++) {
		if (itemCountPair->first.GetType() == type) {
			itemCountPair->second -= count;
			if (itemCountPair->second <= 0) {
				mItemMap.erase(itemCountPair);
				break;
			}
		}
	}
}

Inventory::~Inventory() {

}

void Inventory::Open(bool val) {
	mOpen = val;
	SetVisible(val);
}

bool Inventory::IsOpened() const {
	return mOpen;
}

// grim code ahead
void Inventory::Serialize(SaveFile & s) {
	int count = mItemMap.size();
	s & count;
	if (s.IsLoading()) {
		for (int i = 0; i < count; ++i) {
			int value;
			int type;

			s & type;
			s & value;

			mItemMap[Item((Item::Type)type)] = value;
		}
	} else {
		for (auto p : mItemMap) {
			auto key = (int)p.first.GetType();
			auto value = p.second;

			s & key;
			s & value;
		}
	}

	/*
	// serialize readed notes
	count = mReadedNotes.size();
	s & count;
	for (int i = 0; i < count; ++i) {
		string desc, text;

		if (s.IsLoading()) {
			s & desc;
			s & text;
			mReadedNotes.push_back(pair<string, string>(desc, text));
		} else {
			desc = mReadedNotes[i].first;
			text = mReadedNotes[i].second;

			s & desc;
			s & text;
		}
	}*/
}

void Inventory::AddItem(Item::Type type) {
	if (type != Item::Type::Unknown) {
		bool found = false;
		for (auto & itemCountPair = mItemMap.begin(); itemCountPair != mItemMap.end(); itemCountPair++) {
			if (itemCountPair->first.GetType() == type) {
				found = true;
				if (!itemCountPair->first.mSingleInstance) {
					itemCountPair->second++;
				}
			}
		}
		if (!found) {
			mItemMap[Item(type)] = 1;
		}
	}
}

void Inventory::ResetSelectedForUse() {
	mpItemForUse = nullptr;
}

Item * Inventory::GetItemSelectedForUse() {
	return mpItemForUse;
}

int Inventory::GetItemCount(Item::Type type) {
	for (auto & itemCountPair = mItemMap.begin(); itemCountPair != mItemMap.end(); itemCountPair++) {
		if (itemCountPair->first.GetType() == type) {
			return itemCountPair->second;
		}
	}
	return 0;
}

void Inventory::GetItems(map<Item, int> & itemMap) {
	itemMap = mItemMap;
}

void Inventory::SetItems(map<Item, int> & items) {
	mItemMap = items;
}

