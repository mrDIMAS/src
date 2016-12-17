#pragma  once

#include "Game.h"
#include "Item.h"
#include "Parser.h"
#include "SaveFile.h"
#include "ScrollList.h"


class Inventory {
private:
	unique_ptr<Game> & mGame;
	enum class Tab : int {
		Items, Notes
	};
	shared_ptr<IGUIScene> mScene;
	// item-count map
	map<Item, int> mItemMap;
	shared_ptr<ITexture> mBackgroundTexture;
	shared_ptr<ITexture> mCellTexture;
	shared_ptr<ITexture> mButtonTexture;
	bool mOpen;
	Item* mpItemForUse;
	Item* mpSelectedItem;
	static const int mCellCountWidth = 5;
	static const int mCellCountHeight = 4;
	static const int mCellWidth = 64;
	static const int mCellHeight = 64;
	shared_ptr<IFont> mFont;
	Config mLocalization;

	shared_ptr<IText> mItemDescriptionText;
	shared_ptr<IRect> mItemsBackground;
	shared_ptr<IText> mItemDescription;
	shared_ptr<IButton> mUseButton;
	shared_ptr<IText> mItemCharacteristics;
	shared_ptr<IRect> mItem[mCellCountWidth][mCellCountHeight];
	shared_ptr<IRect> mItemCell[mCellCountWidth][mCellCountHeight];
	shared_ptr<IText> mItemCountText[mCellCountWidth][mCellCountHeight];
	shared_ptr<IText> mItemMass;
	shared_ptr<IText> mItemContent;
	shared_ptr<IText> mItemContentType;
	shared_ptr<IText> mItemVolume;
	shared_ptr<IButton> mPageItems;
	shared_ptr<IButton> mPageNotes;
	shared_ptr<ISound> mPickUpSound;
	shared_ptr<IRect> mNotesBackground;
	unique_ptr<ScrollList> mNotesList;
	shared_ptr<IText> mNoteText;

	void OnPageItemsClick() {
		mTab = Tab::Items;
	}

	void OnPageNotesClick() {
		mTab = Tab::Notes;
	}

	Tab mTab;
	vector<pair<string, string>> mReadedNotes;
public:
	void SetItems(map<Item, int> & items);
	bool IsOpened() const;
	void Open(bool val);
	explicit Inventory(unique_ptr<Game> & game);
	virtual ~Inventory();
	void Update();
	void RemoveItem(Item::Type type, int count);
	Item * GetItemSelectedForUse();
	void ResetSelectedForUse();
	void AddItem(Item::Type type);
	int GetItemCount(Item::Type type);
	void Serialize(SaveFile & out);
	void SetVisible(bool state);
	void GetItems(map<Item, int> & itemMap);
	void AddReadedNote(const string & desc, const string & text);
};