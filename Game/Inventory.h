#pragma  once

#include "Game.h"
#include "Item.h"
#include "Parser.h"
#include "TextFileStream.h"

class Inventory {
private:
    vector<Item *> mItemList;
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
    ruSoundHandle mPickSound;
    ruFontHandle mFont;
    Parser mLocalization;
    ruRectHandle mGUIRectItemForUse;
    ruRectHandle mGUIBackground;
    ruRectHandle mGUIRightPanel;
    ruTextHandle mGUIDescription;
    ruRectHandle mGUIActions;
    ruButtonHandle mGUIButtonUse;
    ruButtonHandle mGUIButtonCombine;
    ruButtonHandle mGUIButtonThrow;
    ruRectHandle mGUIFirstCombineItem;
    ruRectHandle mGUISecondCombineItem;
    ruRectHandle mGUIFirstCombineItemCell;
    ruRectHandle mGUISecondCombineItemCell;
    ruTextHandle mGUICharacteristics;
    ruRectHandle mGUIItem[mCellCountWidth][mCellCountHeight];
    ruRectHandle mGUIItemCell[mCellCountWidth][mCellCountHeight];
    ruTextHandle mGUIItemDescription;
    ruTextHandle mGUIItemMass;
    ruTextHandle mGUIItemContent;
    ruTextHandle mGUIItemContentType;
    ruTextHandle mGUIItemVolume;
public:
    bool IsOpened() const;
    void Open(bool val);
	bool GotAnyItemOfType( Item::Type type );
	void Repair();
    explicit Inventory();
    virtual ~Inventory();
    void Update();
    bool IsMouseInside( int x, int y, int w, int h );
    void DoCombine();
    void RemoveItem( Item * item );
    void ThrowItem( Item * item );
    int GetItemCount();
    Item * GetItemSelectedForUse( );
    void ResetSelectedForUse();
    bool Contains( Item * pItem );
    void AddItem( Item * pItem );
    int GetItemCount( Item::Type type );
    void Serialize( TextFileStream & out );
    void Deserialize( TextFileStream & in );
    void SetVisible( bool state );
};