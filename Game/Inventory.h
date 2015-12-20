#pragma  once

#include "Game.h"
#include "Item.h"
#include "Parser.h"
#include "SaveFile.h"

class Inventory {
private:
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
    ruFont * mFont;
    Parser mLocalization;
    ruRect * mGUIRectItemForUse;
    ruRect * mGUICanvas;
    //ruRect * mGUIRightPanel;
    ruText * mGUIDescription;
    //ruRect * mGUIActions;
    ruButton * mGUIButtonUse;
    ruButton * mGUIButtonCombine;
    ruRect * mGUIFirstCombineItem;
    ruRect * mGUISecondCombineItem;
    ruRect * mGUIFirstCombineItemCell;
    ruRect * mGUISecondCombineItemCell;
    ruText * mGUICharacteristics;
    ruRect * mGUIItem[mCellCountWidth][mCellCountHeight];
    ruRect * mGUIItemCell[mCellCountWidth][mCellCountHeight];
	ruText * mGUIItemCountText[mCellCountWidth][mCellCountHeight];
    ruText * mGUIItemDescription;
    ruText * mGUIItemMass;
    ruText * mGUIItemContent;
    ruText * mGUIItemContentType;
    ruText * mGUIItemVolume;
public:
	void SetItems( map<Item,int> & items );
    bool IsOpened() const;
    void Open(bool val);
    explicit Inventory();
    virtual ~Inventory();
    void Update();
    bool IsMouseInside( int x, int y, int w, int h );
    void DoCombine();
    void RemoveItem( Item::Type type, int count );
    Item * GetItemSelectedForUse( );
    void ResetSelectedForUse();
    void AddItem( Item::Type type );
    int GetItemCount( Item::Type type );
    void Serialize( SaveFile & out );
    void Deserialize( SaveFile & in );
    void SetVisible( bool state );
	void GetItems( map<Item,int> & itemMap );
};