#pragma  once

#include "Game.h"
#include "Item.h"
#include "Parser.h"
#include "SaveFile.h"



class Inventory {
private:
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
    
    shared_ptr<ruRect> mGUICanvas;
    shared_ptr<ruText> mGUIDescription;
    shared_ptr<ruButton> mGUIButtonUse;
    shared_ptr<ruButton> mGUIButtonCombine;
    shared_ptr<ruRect> mGUIFirstCombineItem;
    shared_ptr<ruRect> mGUISecondCombineItem;
    shared_ptr<ruRect> mGUIFirstCombineItemCell;
    shared_ptr<ruRect> mGUISecondCombineItemCell;
    shared_ptr<ruText> mGUICharacteristics;
    shared_ptr<ruRect> mGUIItem[mCellCountWidth][mCellCountHeight];
    shared_ptr<ruRect> mGUIItemCell[mCellCountWidth][mCellCountHeight];
	shared_ptr<ruText> mGUIItemCountText[mCellCountWidth][mCellCountHeight];
    shared_ptr<ruText> mGUIItemDescription;
    shared_ptr<ruText> mGUIItemMass;
    shared_ptr<ruText> mGUIItemContent;
    shared_ptr<ruText> mGUIItemContentType;
    shared_ptr<ruText> mGUIItemVolume;
	shared_ptr<ruButton> mPageItems;
	shared_ptr<ruButton> mPageNotes;

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
			mReadedNotes.push_back(pair<string, string>(desc, text));
		}
	}
};