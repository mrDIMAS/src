#include "Precompiled.h"

#include "Inventory.h"
#include "GUIProperties.h"
#include "Player.h"
#include "Level.h"

Inventory::Inventory(unique_ptr<Game> & game) :
	mGame(game),
	mOpen(false),
	mpSelectedItem(nullptr),
	mpItemForUse(nullptr),
	mTab(Inventory::Tab::Items) {
	mLocalization.Load(Game::Instance()->GetLocalizationPath() + "inventory.loc");

	mScene = mGame->GetEngine()->CreateGUIScene();

	auto renderer = mGame->GetEngine()->GetRenderer();
	mBackgroundTexture = renderer->GetTexture("data/gui/inventory/back.tga");
	mCellTexture = renderer->GetTexture("data/gui/inventory/item.tga");
	mButtonTexture = renderer->GetTexture("data/gui/inventory/button.tga");
	mFont = mGame->GetEngine()->CreateBitmapFont(14, "data/fonts/font5.ttf");

	mPickUpSound = mGame->GetEngine()->GetSoundSystem()->LoadSound2D("data/sounds/backpack.ogg");
	mPickUpSound->SetVolume(0.6);

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
		mPageItems = mScene->CreateButton(backgroundX, backgroundY - 32, 120, 32, renderer->GetTexture("data/gui/inventory/button2.png"), mLocalization.GetString("pageItems"), mFont, pGUIProp->mForeColor);
		mPageItems->AddAction(GUIAction::OnClick, [this] { OnPageItemsClick(); });

		mPageNotes = mScene->CreateButton(backgroundX + 120, backgroundY - 32, 120, 32, renderer->GetTexture("data/gui/inventory/button2.png"), mLocalization.GetString("pageNotes"), mFont, pGUIProp->mForeColor);
		mPageNotes->AddAction(GUIAction::OnClick, [this] { OnPageNotesClick(); });
	}

	// background
	mItemsBackground = mScene->CreateRect(backgroundX, backgroundY, backgroundW, backgroundH, mBackgroundTexture, pGUIProp->mBackColor);
	int combineH = 128;
	int combineY = backgroundH - 128;
	int descriptionY = combineY + 10;

	mItemDescriptionText = mScene->CreateText(mLocalization.GetString("desc"), 10, descriptionY, backgroundW - 128, combineH, mFont, pGUIProp->mForeColor, TextAlignment::Left);
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

	mUseButton = mScene->CreateButton(buttonsX, buttonY, buttonW, buttonH, mButtonTexture, mLocalization.GetString("use"), mFont, pGUIProp->mForeColor, TextAlignment::Center, 255);
	mUseButton->Attach(mItemsBackground);

	// combine items
	int combineBoxY = buttonY + 3.6f * buttonH;
	Vector3 combineColor1 = pGUIProp->mForeColor;
	Vector3 combineColor2 = pGUIProp->mForeColor;
	int combineBoxSpacing = 5;
	buttonsX += mCellWidth / 2 - 2 * combineBoxSpacing;

	mItemCharacteristics = mScene->CreateText(mLocalization.GetString("characteristics"), actionsX, combineBoxY + 1.5f * mCellHeight, 128, combineH, mFont, pGUIProp->mForeColor, TextAlignment::Center);
	mItemCharacteristics->Attach(mItemsBackground);

	int itemSpacing = 5;
	for(int cw = 0; cw < mCellCountWidth; cw++) {
		for(int ch = 0; ch < mCellCountHeight; ch++) {
			int cellX = 20 + distMult * mCellWidth * cw;
			int cellY = 20 + distMult * mCellHeight * ch;

			mItemCell[cw][ch] = mScene->CreateRect(cellX, cellY, mCellWidth, mCellHeight, mCellTexture, pGUIProp->mForeColor, 255);
			mItemCell[cw][ch]->Attach(mItemsBackground);

			mItem[cw][ch] = mScene->CreateRect(cellX + itemSpacing, cellY + itemSpacing, mCellWidth - 2 * itemSpacing, mCellHeight - 2 * itemSpacing, nullptr, Vector3(255, 255, 255), 255);
			mItem[cw][ch]->Attach(mItemsBackground);

			mItemCountText[cw][ch] = mScene->CreateText("0", cellX + distMult * mCellWidth - 18, cellY + distMult * mCellHeight - 24, 8, 8, mFont, pGUIProp->mForeColor, TextAlignment::Center);
			mItemCountText[cw][ch]->Attach(mItemsBackground);
		}
	}

	int offset = combineBoxY + 2.2f * mCellHeight;
	mItemDescription = mScene->CreateText("Desc", 2 * itemSpacing, descriptionY + 5 * itemSpacing, backgroundW - 2 * itemSpacing - 128, combineH - 2 * itemSpacing, pGUIProp->mFont, Vector3(200, 200, 200), TextAlignment::Left, 255);
	mItemDescription->Attach(mItemsBackground);

	// characteristics of item
	int charSpace = 28;
	mItemContentType = mScene->CreateText("ContentType", actionsX + buttonSpace, offset + charSpace * 1, actionsW - itemSpacing, 100, mFont, pGUIProp->mForeColor, TextAlignment::Left);
	mItemContentType->Attach(mItemsBackground);

	mItemContent = mScene->CreateText("Content", actionsX + buttonSpace, offset + charSpace * 2.1, actionsW - itemSpacing, 100, mFont, pGUIProp->mForeColor, TextAlignment::Left);
	mItemContent->Attach(mItemsBackground);

	mItemVolume = mScene->CreateText("Volume", actionsX + buttonSpace, offset + charSpace * 3.1, actionsW - itemSpacing, 100, mFont, pGUIProp->mForeColor, TextAlignment::Left);
	mItemVolume->Attach(mItemsBackground);

	mItemMass = mScene->CreateText("Mass", actionsX + buttonSpace, offset + charSpace * 4.1, actionsW - itemSpacing, 100, mFont, pGUIProp->mForeColor, TextAlignment::Left);
	mItemMass->Attach(mItemsBackground);


	// Notes page
	{
		mNotesBackground = mScene->CreateRect(backgroundX, backgroundY, backgroundW, backgroundH, renderer->GetTexture("data/gui/inventory/back_note.tga"), pGUIProp->mBackColor);

		mNotesList = make_unique<ScrollList>(mScene, 80, 20, renderer->GetTexture("data/gui/menu/button.tga"), " ");
		mNotesList->AttachTo(mNotesBackground);

		mNoteText = mScene->CreateText(" ", 30, 60, backgroundW - 60, backgroundH - 90, mFont, pGUIProp->mForeColor, TextAlignment::Left);
		mNoteText->Attach(mNotesBackground);
	}

	SetVisible(false);
}

void Inventory::SetVisible(bool state) {
	mOpen = state;
	mScene->SetVisible(state);
}

void Inventory::Update() {
	Vector3 whiteColor = Vector3(255, 255, 255);
	int screenCenterX = ruVirtualScreenWidth / 2;
	int screenCenterY = ruVirtualScreenHeight / 2;


	if(mTab == Tab::Items) {
		mItemsBackground->SetVisible(true);
		mNotesBackground->SetVisible(false);

		mPageItems->SetSize(mPageItems->GetSize().x, 40);
		mPageNotes->SetSize(mPageNotes->GetSize().x, 32);
	} else {
		mNotesBackground->SetVisible(true);
		mItemsBackground->SetVisible(false);

		mPageNotes->SetSize(mPageNotes->GetSize().x, 40);
		mPageItems->SetSize(mPageItems->GetSize().x, 32);
	}

	if(mReadedNotes.size() > 0) {
		try {
			mNoteText->SetText(mReadedNotes.at(mNotesList->GetCurrentValue()).second);
		} catch(std::exception) {
			// nothing
		}
	}

	if(mpItemForUse) {
		mGame->GetEngine()->GetRenderer()->SetCursorVisible(false);
		SetVisible(false);
		mGame->GetLevel()->GetPlayer()->GetHUD()->ShowUsedItem(mpItemForUse);

		if(mGame->GetEngine()->GetInput()->IsMouseHit(IInput::MouseButton::Left)) {
			mGame->GetLevel()->GetPlayer()->GetHUD()->ShowUsedItem(nullptr);
			mpItemForUse = nullptr;
			mpSelectedItem = nullptr;
			SetVisible(true);
		}
		if(mOpen) {
			mpItemForUse = nullptr;
		}
		return;
	} else {
		mGame->GetLevel()->GetPlayer()->GetHUD()->ShowUsedItem(nullptr); // HAAAAX!!!!
	}

	if(!mOpen) {
		mGame->GetEngine()->GetRenderer()->SetCursorVisible(false);
		return;
	}

	mGame->GetEngine()->GetRenderer()->SetCursorVisible(true);

	int useAlpha = mpSelectedItem ? 255 : 60;
	mUseButton->SetAlpha(useAlpha);
	mUseButton->GetText()->SetAlpha(useAlpha);

	// use item
	if(mUseButton->IsHit()) {
		if(mpSelectedItem) {
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
	for(int cw = 0; cw < mCellCountWidth; cw++) {
		for(int ch = 0; ch < mCellCountHeight; ch++) {
			Vector3 color = pGUIProp->mForeColor;

			int itemNum = cw * mCellCountHeight + ch;

			int i = 0;

			Item * pItem = nullptr;
			int curItemCount = 0;
			for(auto & itemCountPair = mItemMap.begin(); itemCountPair != mItemMap.end(); itemCountPair++) {
				if(i == itemNum) {
					pItem = const_cast<Item*>(&itemCountPair->first);
					curItemCount = itemCountPair->second;
					break;
				}
				i++;
			}
			if(pItem) {
				if(mpSelectedItem == pItem) {
					color = Vector3(0, 200, 0);
				}
				mItemCountText[cw][ch]->SetText(StringBuilder() << curItemCount);
				mItemCountText[cw][ch]->SetVisible(true);
			} else {
				mItemCountText[cw][ch]->SetVisible(false);
			}

			Item * pPicked = nullptr;
			if(mItemCell[cw][ch]->IsMouseInside()) {
				color = Vector3(255, 0, 0);
				pPicked = pItem;
				if(mGame->GetEngine()->GetInput()->IsMouseHit(IInput::MouseButton::Left)) {
					mpSelectedItem = pItem;
				}
			}

			mItemCell[cw][ch]->SetColor(color);
			if(pPicked) {
				if(mGame->GetEngine()->GetInput()->IsMouseHit(IInput::MouseButton::Right) && combinePick) {
					combinePick = false;
					mpSelectedItem = nullptr;
				}
			}
			if(pItem) {
				mItem[cw][ch]->SetVisible(true);
				mItem[cw][ch]->SetTexture(pItem->GetPictogram());
				if(pItem == pPicked) {
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
		}
	}
}

void Inventory::RemoveItem(Item::Type type, int count) {
	for(auto & itemCountPair = mItemMap.begin(); itemCountPair != mItemMap.end(); itemCountPair++) {
		if(itemCountPair->first.GetType() == type) {
			itemCountPair->second -= count;
			if(itemCountPair->second <= 0) {
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
	if(s.IsLoading()) {
		for(int i = 0; i < count; ++i) {
			int value;
			int type;

			s & type;
			s & value;

			mItemMap[Item((Item::Type)type)] = value;
		}
	} else {
		for(auto p : mItemMap) {
			auto key = (int)p.first.GetType();
			auto value = p.second;

			s & key;
			s & value;
		}
	}

	// serialize readed notes
	count = mReadedNotes.size();
	s & count;
	for(int i = 0; i < count; ++i) {
		string desc, text;

		if(s.IsLoading()) {
			s & desc;
			s & text;

			AddReadedNote(desc, text);

			mTab = Tab::Items;
			SetVisible(false);
		} else {
			desc = mReadedNotes[i].first;
			text = mReadedNotes[i].second;

			s & desc;
			s & text;
		}
	}


}

void Inventory::AddItem(Item::Type type) {
	if(type != Item::Type::Unknown) {
		bool found = false;
		for(auto & itemCountPair = mItemMap.begin(); itemCountPair != mItemMap.end(); itemCountPair++) {
			if(itemCountPair->first.GetType() == type) {
				found = true;
				if(!itemCountPair->first.mSingleInstance) {
					itemCountPair->second++;
				}
			}
		}
		if(!found) {
			mItemMap[Item(type)] = 1;
		}

		mPickUpSound->Play();
	}
}

void Inventory::ResetSelectedForUse() {
	mpItemForUse = nullptr;
	mpSelectedItem = nullptr;	
}

Item * Inventory::GetItemSelectedForUse() {
	return mpItemForUse;
}

int Inventory::GetItemCount(Item::Type type) {
	for(auto & itemCountPair = mItemMap.begin(); itemCountPair != mItemMap.end(); itemCountPair++) {
		if(itemCountPair->first.GetType() == type) {
			return itemCountPair->second;
		}
	}
	return 0;
}

void Inventory::GetItems(map<Item, int> & itemMap) {
	itemMap = mItemMap;
}

void Inventory::AddReadedNote(const string & desc, const string & text) {
	auto existing = find(mReadedNotes.begin(), mReadedNotes.end(), pair<string, string>(desc, text));
	if(existing == mReadedNotes.end()) {
		mNotesList->AddValue(desc);
		mNotesList->SetCurrentValue(mNotesList->GetValueCount() - 1);
		mReadedNotes.push_back(pair<string, string>(desc, text));
	}
	mTab = Tab::Notes;
	SetVisible(true);
}

void Inventory::SetItems(map<Item, int> & items) {
	mItemMap = items;
}

