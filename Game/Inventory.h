#pragma  once

#include "Game.h"
#include "Item.h"
#include "Parser.h"
#include "SaveFile.h"

class Inventory {
private:
	// item-count map
	map<Item,int> mItemMap;
    ruTextureHandle mBackgroundTexture;
    ruTextureHandle mCellTexture;
    ruTextureHandle mButtonTexture;
    bool mOpen;
    Item* mpItemForUse;
    Item* mpSelectedItem;
    Item* mpCombineItemFirst;
    Item* mpCombineItemSecond;
    static const int mCellCountWidth = 5;
    static const int mCellCountHeight = 4;
    static const int mCellWidth = 64;
    static const int mCellHeight = 64;
    ruSound mPickSound;
    ruFontHandle mFont;
    Parser mLocalization;
    ruRectHandle mGUIRectItemForUse;
    ruRectHandle mGUICanvas;
    ruRectHandle mGUIRightPanel;
    ruTextHandle mGUIDescription;
    ruRectHandle mGUIActions;
    ruButtonHandle mGUIButtonUse;
    ruButtonHandle mGUIButtonCombine;
    ruRectHandle mGUIFirstCombineItem;
    ruRectHandle mGUISecondCombineItem;
    ruRectHandle mGUIFirstCombineItemCell;
    ruRectHandle mGUISecondCombineItemCell;
    ruTextHandle mGUICharacteristics;
    ruRectHandle mGUIItem[mCellCountWidth][mCellCountHeight];
    ruRectHandle mGUIItemCell[mCellCountWidth][mCellCountHeight];
	ruTextHandle mGUIItemCountText[mCellCountWidth][mCellCountHeight];
    ruTextHandle mGUIItemDescription;
    ruTextHandle mGUIItemMass;
    ruTextHandle mGUIItemContent;
    ruTextHandle mGUIItemContentType;
    ruTextHandle mGUIItemVolume;
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