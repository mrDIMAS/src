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
	mpItemForUse(nullptr),
	mTab(Inventory::Tab::Items)
{
	mLocalization.ParseFile(gLocalizationPath + "inventory.loc");

	mScene = ruGUIScene::Create();

	mBackgroundTexture = ruTexture::Request("data/gui/inventory/back.tga");
	mCellTexture = ruTexture::Request("data/gui/inventory/item.tga");
	mButtonTexture = ruTexture::Request("data/gui/inventory/button.tga");
	mFont = ruFont::LoadFromFile(14, "data/fonts/font5.ttf");
	mPickSound = ruSound::Load2D("data/sounds/menupick.ogg");

	float distMult = 1.1f;
	int cellSpaceX = distMult * mCellWidth / (float)mCellCountWidth;
	int cellSpaceY = distMult * mCellHeight / (float)mCellCountHeight;
	
	int backGroundSpace = 20;
	int backgroundW = mCellCountWidth * mCellWidth + 2.5 * backGroundSpace + cellSpaceX + 128;
	int backgroundH = mCellCountHeight * mCellHeight + 2.5 * backGroundSpace + cellSpaceY + 128;
	int backgroundX = (ruVirtualScreenWidth - backgroundW) / 2;
	int backgroundY = (ruVirtualScreenHeight - backgroundH) / 2;

	// Tab buttons 
	{
		mPageItems = mScene->CreateButton(backgroundX, backgroundY - 32, 120, 32, ruTexture::Request("data/gui/inventory/button2.png"), mLocalization.GetString("pageItems"), mFont, pGUIProp->mForeColor);
		mPageItems->AddAction(ruGUIAction::OnClick, ruDelegate::Bind(this, &Inventory::OnPageItemsClick));

		mPageNotes = mScene->CreateButton(backgroundX + 120, backgroundY - 32, 120, 32, ruTexture::Request("data/gui/inventory/button2.png"), mLocalization.GetString("pageNotes"), mFont, pGUIProp->mForeColor);
		mPageNotes->AddAction(ruGUIAction::OnClick, ruDelegate::Bind(this, &Inventory::OnPageNotesClick));
	}

	// background
	mItemsBackground = mScene->CreateRect(backgroundX, backgroundY, backgroundW, backgroundH, mBackgroundTexture, pGUIProp->mBackColor);
	int combineH = 128;
	int combineY = backgroundH - 128;
	int descriptionY = combineY + 10;

	mItemDescriptionText = mScene->CreateText(mLocalization.GetString("desc"), 10, descriptionY, backgroundW - 128, combineH, mFont, pGUIProp->mForeColor, ruTextAlignment::Left);
	mItemDescriptionText->Attach(mItemsBackground);

	// item actions
	int actionsW = 128;
	int actionsX = backgroundW - 128;

	//  actions buttons
	int buttonSpace = 10;
	int buttonsX = actionsX + buttonSpace;
	int buttonY = 2 * buttonSpace;
	int buttonH = 30;
	int buttonW = actionsW - 2 * buttonSpace;

	mUseButton = mScene->CreateButton(buttonsX, buttonY, buttonW, buttonH, mButtonTexture, mLocalization.GetString("use"), mFont, pGUIProp->mForeColor, ruTextAlignment::Center, 255);
	mUseButton->Attach(mItemsBackground);

	mCombineButton = mScene->CreateButton(buttonsX, buttonY + 1.5f * buttonH, buttonW, buttonH, mButtonTexture, mLocalization.GetString("combine"), mFont, pGUIProp->mForeColor, ruTextAlignment::Center, 255);
	mCombineButton->Attach(mItemsBackground);

	// combine items
	int combineBoxY = buttonY + 3.6f * buttonH;
	ruVector3 combineColor1 = pGUIProp->mForeColor;
	ruVector3 combineColor2 = pGUIProp->mForeColor;
	int combineBoxSpacing = 5;
	buttonsX += mCellWidth / 2 - 2 * combineBoxSpacing;

	mFirstCombineItem = mScene->CreateRect(buttonsX + combineBoxSpacing, combineBoxY + combineBoxSpacing, mCellWidth - 2 * combineBoxSpacing, mCellHeight - 2 * combineBoxSpacing, nullptr);
	mFirstCombineItem->Attach(mItemsBackground);

	mSecondCombineItem = mScene->CreateRect(buttonsX + combineBoxSpacing, combineBoxY + 1.2f * mCellHeight + combineBoxSpacing, mCellWidth - 2 * combineBoxSpacing, mCellHeight - 2 * combineBoxSpacing, nullptr);
	mSecondCombineItem->Attach(mItemsBackground);

	mFirstCombineItemCell = mScene->CreateRect(buttonsX, combineBoxY, mCellWidth, mCellHeight, mCellTexture, combineColor1, 255);
	mFirstCombineItemCell->Attach(mItemsBackground);

	mSecondCombineItemCell = mScene->CreateRect(buttonsX, combineBoxY + 1.2f * mCellHeight, mCellWidth, mCellHeight, mCellTexture, combineColor2, 255);
	mSecondCombineItemCell->Attach(mItemsBackground);

	mItemCharacteristics = mScene->CreateText(mLocalization.GetString("characteristics"), actionsX, combineBoxY + 1.5f * mCellHeight, 128, combineH, mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
	mItemCharacteristics->Attach(mItemsBackground);

	int itemSpacing = 5;
	for (int cw = 0; cw < mCellCountWidth; cw++) {
		for (int ch = 0; ch < mCellCountHeight; ch++) {
			int cellX = 20 + distMult * mCellWidth * cw;
			int cellY = 20 + distMult * mCellHeight * ch;

			mItemCell[cw][ch] = mScene->CreateRect(cellX, cellY, mCellWidth, mCellHeight, mCellTexture, pGUIProp->mForeColor, 255);
			mItemCell[cw][ch]->Attach(mItemsBackground);

			mItem[cw][ch] = mScene->CreateRect(cellX + itemSpacing, cellY + itemSpacing, mCellWidth - 2 * itemSpacing, mCellHeight - 2 * itemSpacing, nullptr, ruVector3(255, 255, 255), 255);
			mItem[cw][ch]->Attach(mItemsBackground);

			mItemCountText[cw][ch] = mScene->CreateText("0", cellX + distMult * mCellWidth - 18, cellY + distMult * mCellHeight - 24, 8, 8, mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
			mItemCountText[cw][ch]->Attach(mItemsBackground);
		}
	}

	int offset = combineBoxY + 2.2f * mCellHeight;
	mItemDescription = mScene->CreateText("Desc",  2 * itemSpacing, descriptionY + 2.5 * itemSpacing, backgroundW - 2 * itemSpacing - 128, combineH - 2 * itemSpacing, pGUIProp->mFont, ruVector3(200, 200, 200), ruTextAlignment::Left, 255);
	mItemDescription->Attach(mItemsBackground);

	// characteristics of item
	int charSpace = 28;
	mItemContentType = mScene->CreateText("ContentType", actionsX + buttonSpace, offset + charSpace * 1, actionsW - itemSpacing, 100, mFont, pGUIProp->mForeColor, ruTextAlignment::Left);
	mItemContentType->Attach(mItemsBackground);

	mItemContent = mScene->CreateText("Content", actionsX + buttonSpace, offset + charSpace * 2.1, actionsW - itemSpacing, 100, mFont, pGUIProp->mForeColor, ruTextAlignment::Left);
	mItemContent->Attach(mItemsBackground);

	mItemVolume = mScene->CreateText("Volume", actionsX + buttonSpace, offset + charSpace * 3.1, actionsW - itemSpacing, 100, mFont, pGUIProp->mForeColor, ruTextAlignment::Left);
	mItemVolume->Attach(mItemsBackground);

	mItemMass = mScene->CreateText("Mass", actionsX + buttonSpace, offset + charSpace * 4.1, actionsW - itemSpacing, 100, mFont, pGUIProp->mForeColor, ruTextAlignment::Left);
	mItemMass->Attach(mItemsBackground);


	// Notes page
	{
		mNotesBackground = mScene->CreateRect(backgroundX, backgroundY, backgroundW, backgroundH, ruTexture::Request("data/gui/inventory/back_note.tga"), pGUIProp->mBackColor);

		mNotesList = unique_ptr<ScrollList>(new ScrollList(mScene, 80, 20, ruTexture::Request("data/gui/menu/button.tga"), " " ));
		mNotesList->AttachTo(mNotesBackground);

		mNoteText = mScene->CreateText("(nothing)", 30, 60, backgroundW - 60, backgroundH - 90, mFont, pGUIProp->mForeColor, ruTextAlignment::Left);
		mNoteText->Attach(mNotesBackground);
	}

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


	if (mTab == Tab::Items) {
		mItemsBackground->SetVisible(true);
		mNotesBackground->SetVisible(false);
	} else {
		mNotesBackground->SetVisible(true);
		mItemsBackground->SetVisible(false);
	}

	if (mReadedNotes.size() > 0) {
		try {
			mNoteText->SetText(mReadedNotes.at(mNotesList->GetCurrentValue()).second);
		} catch (std::exception & e) {
			// nothing
		}
	}
	mNotesList->Update();

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
	mUseButton->SetAlpha(useAlpha);
	mUseButton->GetText()->SetAlpha(useAlpha);

	bool canCombine = (mpCombineItemFirst != nullptr && mpCombineItemSecond != nullptr);

	int combineAlpha = canCombine ? 255 : 60;
	mCombineButton->SetAlpha(combineAlpha);
	mCombineButton->GetText()->SetAlpha(combineAlpha);

	// draw combine items
	ruVector3 combineColor1 = pGUIProp->mForeColor;
	ruVector3 combineColor2 = pGUIProp->mForeColor;
	int combineBoxSpacing = 5;

	mFirstCombineItem->SetTexture(nullptr);
	mSecondCombineItem->SetTexture(nullptr);

	// show first combining item
	if (mpCombineItemFirst) {
		mFirstCombineItem->SetVisible(true);
		mFirstCombineItem->SetTexture(mpCombineItemFirst->GetPictogram());
		if (mFirstCombineItem->IsMouseInside()) {
			combineColor1 = ruVector3(255, 0, 0);
			if (ruInput::IsMouseHit(ruInput::MouseButton::Left)) {
				mpCombineItemFirst = nullptr;
			}
		}
	} else {
		mFirstCombineItem->SetVisible(false);
	};
	mFirstCombineItemCell->SetColor(combineColor1);

	// show second combining item
	if (mpCombineItemSecond) {
		mSecondCombineItem->SetVisible(true);
		mSecondCombineItem->SetTexture(mpCombineItemSecond->GetPictogram());
		if (mSecondCombineItem->IsMouseInside()) {
			combineColor2 = ruVector3(255, 0, 0);
			if (ruInput::IsMouseHit(ruInput::MouseButton::Left)) {
				mpCombineItemSecond = nullptr;
			}
		}
	} else {
		mSecondCombineItem->SetVisible(false);
	}
	mSecondCombineItemCell->SetColor(combineColor2);

	// do combine
	if (mCombineButton->IsHit()) {
		if (canCombine) {
			DoCombine();
		}
	}

	// use item
	if (mUseButton->IsHit()) {
		if (mpSelectedItem) {
			mpItemForUse = mpSelectedItem;
			mOpen = false;
		}
	}

	mItemDescription->SetVisible(false);
	mItemContentType->SetVisible(false);
	mItemContent->SetVisible(false);
	mItemVolume->SetVisible(false);
	mItemMass->SetVisible(false);


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
				mItemCountText[cw][ch]->SetText(StringBuilder() << curItemCount);
				if (pItem != mpCombineItemFirst && pItem != mpCombineItemSecond) {
					mItemCountText[cw][ch]->SetVisible(true);
				} else {
					mItemCountText[cw][ch]->SetVisible(false);
				}
			} else {
				mItemCountText[cw][ch]->SetVisible(false);
			}

			Item * pPicked = nullptr;
			if (mItemCell[cw][ch]->IsMouseInside()) {
				color = ruVector3(255, 0, 0);
				if (pItem != mpCombineItemFirst && pItem != mpCombineItemSecond) {
					pPicked = pItem;
					if (ruInput::IsMouseHit(ruInput::MouseButton::Left)) {
						mpSelectedItem = pItem;
					}
				}
			}

			mItemCell[cw][ch]->SetColor(color);
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
					mItem[cw][ch]->SetVisible(true);
					mItem[cw][ch]->SetTexture(pItem->GetPictogram());
					if (pItem == pPicked) {
						mItemDescription->SetVisible(true);
						mItemContentType->SetVisible(true);
						mItemContent->SetVisible(true);
						mItemVolume->SetVisible(true);
						mItemMass->SetVisible(true);
						mItemDescription->SetText(pItem->GetDescription());
						mItemContentType->SetText(StringBuilder() << mLocalization.GetString("contentType") << ": " << pItem->GetContentType());
						mItemContent->SetText(StringBuilder() << mLocalization.GetString("content") << ": " << pItem->GetContent());
						mItemVolume->SetText(StringBuilder() << mLocalization.GetString("volume") << ": " << pItem->GetVolume());
						mItemMass->SetText(StringBuilder() << mLocalization.GetString("mass") << ": " << pItem->GetMass());
					}
				} else {
					mItem[cw][ch]->SetVisible(false);
				}
			} else {
				mItem[cw][ch]->SetVisible(false);
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
		
	// serialize readed notes
	count = mReadedNotes.size();
	s & count;
	for (int i = 0; i < count; ++i) {
		string desc, text;

		if (s.IsLoading()) {
			s & desc;
			s & text;
			
			AddReadedNote(desc, text);
		} else {
			desc = mReadedNotes[i].first;
			text = mReadedNotes[i].second;

			s & desc;
			s & text;
		}
	}
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

