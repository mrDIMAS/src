#pragma  once

#include "Game.h"
#include "Item.h"
#include "Parser.h"
#include "SaveFile.h"
#include "ScrollList.h"


class Inventory {
private:
	enum class Tab : int {
		Items, Notes
	};
	shared_ptr<ruGUIScene> mScene;
	// item-count map
	map<Item,int> mItemMap;
    shared_ptr<ruTexture> mBackgroundTexture;
    shared_ptr<ruTexture> mCellTexture;
    shared_ptr<ruTexture> mButtonTexture;
    bool mOpen;
    Item* mpItemForUse;
    Item* mpSelectedItem;
    Item* mpCombineItemFirst;
    Item* mpCombineItemSecond;
    static const int mCellCountWidth = 5;
    static const int mCellCountHeight = 4;
    static const int mCellWidth = 64;
    static const int mCellHeight = 64;
    shared_ptr<ruSound> mPickSound;
    shared_ptr<ruFont> mFont;
    Parser mLocalization;
    
	shared_ptr<ruText> mItemDescriptionText;
    shared_ptr<ruRect> mItemsBackground;
    shared_ptr<ruText> mItemDescription;
    shared_ptr<ruButton> mUseButton;
    shared_ptr<ruButton> mCombineButton;
    shared_ptr<ruRect> mFirstCombineItem;
    shared_ptr<ruRect> mSecondCombineItem;
    shared_ptr<ruRect> mFirstCombineItemCell;
    shared_ptr<ruRect> mSecondCombineItemCell;
    shared_ptr<ruText> mItemCharacteristics;
    shared_ptr<ruRect> mItem[mCellCountWidth][mCellCountHeight];
    shared_ptr<ruRect> mItemCell[mCellCountWidth][mCellCountHeight];
	shared_ptr<ruText> mItemCountText[mCellCountWidth][mCellCountHeight];
    shared_ptr<ruText> mItemMass;
    shared_ptr<ruText> mItemContent;
    shared_ptr<ruText> mItemContentType;
    shared_ptr<ruText> mItemVolume;
	shared_ptr<ruButton> mPageItems;
	shared_ptr<ruButton> mPageNotes;

	shared_ptr<ruRect> mNotesBackground;
	unique_ptr<ScrollList> mNotesList;
	shared_ptr<ruText> mNoteText;

	void OnPageItemsClick() {
		mTab = Tab::Items;
	}

	void OnPageNotesClick() {
		mTab = Tab::Notes;
	}

	Tab mTab;
	vector<pair<string, string>> mReadedNotes;
public:
	void SetItems( map<Item,int> & items );
    bool IsOpened() const;
    void Open(bool val);
    explicit Inventory();
    virtual ~Inventory();
    void Update();
    void DoCombine();
    void RemoveItem( Item::Type type, int count );
    Item * GetItemSelectedForUse( );
    void ResetSelectedForUse();
    void AddItem( Item::Type type );
    int GetItemCount( Item::Type type );
    void Serialize( SaveFile & out );
    void SetVisible( bool state );
	void GetItems( map<Item,int> & itemMap );

	void AddReadedNote(const string & desc, const string & text) {
		auto existing = find(mReadedNotes.begin(), mReadedNotes.end(), pair<string,string>(desc, text));
		if (existing == mReadedNotes.end()) {
			mNotesList->AddValue(desc);
			mReadedNotes.push_back(pair<string, string>(desc, text));
		}
	}
};