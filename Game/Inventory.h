#pragma  once

#include "Game.h"
#include "Item.h"
#include "Parser.h"
#include "TextFileStream.h"

class Inventory
{
private:
	vector<Item *> mItemList;
	ruTextureHandle mBackgroundTexture;
	ruTextureHandle mCellTexture;
	ruTextureHandle mButtonTexture;
	bool mOpened;
	Item* mpItemForUse;
	Item* mpSelected;
	Item* mpCombineItemFirst;
	Item* mpCombineItemSecond;
	int mCellCountWidth;
	int mCellCountHeight;
	int mCellWidth;
	int mCellHeight;
	ruSoundHandle mPickSound;
	ruFontHandle mFont;
	Parser mLocalization;
public:
	bool IsOpened() const;
	void Open(bool val);
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
};