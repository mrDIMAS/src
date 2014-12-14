#include "Inventory.h"
#include "GUI.h"
#include "Player.h"


Inventory::Inventory()
{
    mLocalization.ParseFile( localizationPath + "inventory.loc" );

    mBackgroundTexture = ruGetTexture( "data/gui/inventory/background.png");
    mCellTexture = ruGetTexture( "data/gui/inventory/cell.png" );
    mButtonTexture = ruGetTexture( "data/gui/inventory/button.png" );

    mOpened = 0;
    mpSelected = 0;

    mCellWidth = 64;
    mCellHeight = 64;
    mCellCountWidth = 5;
    mCellCountHeight = 4;

    mFont = ruCreateGUIFont( 14, "data/fonts/font1.otf", 0, 0 );

    mpCombineItemFirst = 0;
    mpCombineItemSecond = 0;

    mpItemForUse = 0;

    mPickSound = ruLoadSound2D( "data/sounds/menupick.ogg" );
}

bool Inventory::IsMouseInside( int x, int y, int w, int h )
{
    return ruGetMouseX() > x && ruGetMouseX() < ( x + w ) && ruGetMouseY() > y && ruGetMouseY() < ( y + h );
}

void Inventory::DoCombine()
{
	Item * pUsedItem = nullptr;
    if( mpCombineItemFirst->Combine( mpCombineItemSecond, pUsedItem )) // combine successfull 
	{
		mpCombineItemFirst = nullptr;
		mpCombineItemSecond = nullptr;
		mpSelected = nullptr;
		if( pUsedItem ) {
			ThrowItem( pUsedItem );
		}
	}
}

void Inventory::Update()
{
    ruVector3 whiteColor = ruVector3( 255, 255, 255 );
    int screenCenterX = ruGetResolutionWidth() / 2;
    int screenCenterY = ruGetResolutionHeight() / 2;

    if( mpItemForUse )
    {
        ruHideCursor();
        ruDrawGUIRect( screenCenterX, screenCenterY, 64, 64, mpItemForUse->GetPictogram(), whiteColor, 255 );
        if( ruIsMouseHit( MB_Left ))
        {
            mpItemForUse = nullptr;
            mpSelected = nullptr;
        }
        if( mOpened )
            mpItemForUse = 0;
        return;
    }

    if( !mOpened )
    {
        ruHideCursor();
        return;
    }

    ruShowCursor();

    float distMult = 1.1f;
    int cellSpaceX = distMult * mCellWidth / (float)mCellCountWidth;
    int cellSpaceY = distMult * mCellHeight / (float)mCellCountHeight;
    int coordX = screenCenterX - 0.5f * (float)mCellCountWidth * (float)mCellWidth - cellSpaceX;
    int coordY = screenCenterY - 0.5f * (float)mCellCountHeight * (float)mCellHeight - cellSpaceY;
    // draw background
    int backGroundSpace = 20;
    int backgroundX = coordX - backGroundSpace;
    int backgroundY = coordY - backGroundSpace;
    int backgroundW = mCellCountWidth * mCellWidth + 2.5 * backGroundSpace + cellSpaceX;
    int backgroundH = mCellCountHeight * mCellHeight + 2.5 * backGroundSpace + cellSpaceY;
    ruDrawGUIRect( backgroundX, backgroundY, backgroundW, backgroundH, mBackgroundTexture );
    // draw item description
    int combineH = 128;
    int combineY = backgroundY + backgroundH;
    int descriptionY = combineY + 10;
    ruDrawGUIRect( backgroundX, combineY, backgroundW, combineH, mBackgroundTexture );
    ruDrawGUIText( mLocalization.GetString( "desc" ), backgroundX + 10, descriptionY, backgroundW, combineH, mFont, whiteColor, 0 );
    // draw item actions
    int actionsW = 128;
    int actionsX = backgroundX + backgroundW;
    ruDrawGUIRect( actionsX, backgroundY, actionsW, backgroundH + combineH, mBackgroundTexture );
    // draw actions buttons
    int buttonSpace = 10;
    int buttonsX = actionsX + buttonSpace;
    int buttonY = backgroundY + 2 * buttonSpace;
    int buttonH = 30;
    int buttonW = actionsW - 2 * buttonSpace;
    bool canCombine = ( mpCombineItemFirst != 0 && mpCombineItemSecond != 0 );
    int useAlpha = mpSelected ? 255 : 60;
    ruGUIState buttonUse = ruDrawGUIButton( buttonsX, buttonY, buttonW, buttonH, mButtonTexture, mLocalization.GetString( "use" ), mFont, whiteColor, 1, useAlpha );
    ruGUIState buttonThrow = ruDrawGUIButton( buttonsX, buttonY + 1.2f * buttonH, buttonW, buttonH, mButtonTexture, mLocalization.GetString( "throw" ), mFont, whiteColor, 1, useAlpha );
    int combineAlpha = canCombine ? 255 : 60;
    ruGUIState buttonCombine = ruDrawGUIButton( buttonsX, buttonY + 2.4f * buttonH, buttonW, buttonH, mButtonTexture, mLocalization.GetString( "combine" ), mFont, whiteColor, 1, combineAlpha );
    // draw combine items
    int combineBoxY = buttonY + 3.6f * buttonH;
    ruVector3 combineColor1 = whiteColor;
    ruVector3 combineColor2 = whiteColor;
    int combineBoxSpacing = 5;
    buttonsX += mCellWidth / 2 - 2 * combineBoxSpacing;
    if( mpCombineItemFirst )
    {
        ruDrawGUIRect( buttonsX + combineBoxSpacing, combineBoxY + combineBoxSpacing, mCellWidth - 2 * combineBoxSpacing, mCellHeight - 2 * combineBoxSpacing, mpCombineItemFirst->GetPictogram() );
        if( IsMouseInside( buttonsX, combineBoxY, mCellWidth, mCellHeight ))
        {
            combineColor1 = ruVector3( 255, 0, 0 );
            if( ruIsMouseHit( MB_Left ))
                mpCombineItemFirst = 0;
        }
    }
    if( mpCombineItemSecond )
    {
        ruDrawGUIRect( buttonsX + combineBoxSpacing, combineBoxY + 1.2f * mCellHeight + combineBoxSpacing, mCellWidth - 2 * combineBoxSpacing, mCellHeight - 2 * combineBoxSpacing, mpCombineItemSecond->GetPictogram() );
        if( IsMouseInside( buttonsX, combineBoxY + 1.2f * mCellHeight, mCellWidth, mCellHeight ))
        {
            combineColor2 = ruVector3( 255, 0, 0 );
            if( ruIsMouseHit( MB_Left ))
                mpCombineItemSecond = 0;
        }
    }
    ruDrawGUIRect( buttonsX, combineBoxY, mCellWidth, mCellHeight, mCellTexture, combineColor1, 255 );
    ruDrawGUIRect( buttonsX, combineBoxY + 1.2f * mCellHeight, mCellWidth, mCellHeight, mCellTexture, combineColor2, 255 );
    // do combine
    if( buttonCombine.mouseLeftClicked )
    {
        if( canCombine )
            DoCombine();
    }
    // use item
    if( buttonUse.mouseLeftClicked )
    {
        if( mpSelected )
        {
            mpItemForUse = mpSelected;
            mOpened = false;
        }
    }
    // throw item
    if( buttonThrow.mouseLeftClicked )
    {
        if( mpSelected )
        {
            if( mpSelected->IsThrowable() )
                ThrowItem( mpSelected );
        }
    }
    // characteristics
    ruDrawGUIText( mLocalization.GetString( "characteristics" ), actionsX + buttonSpace, combineBoxY + 2.2f * mCellHeight, backgroundW, combineH, mFont, whiteColor, 0 );
    // draw cells and item image
    bool combinePick = true;
    for( int cw = 0; cw < mCellCountWidth; cw++ )
    {
        for( int ch = 0; ch < mCellCountHeight; ch++ )
        {
            Item * pPicked = 0;
            ruVector3 color = whiteColor;
            int alpha = 180;
            // get item for draw
            int itemNum = cw * mCellCountHeight + ch;
            Item * pItem = nullptr;
            if( itemNum < mItemList.size() )
                pItem = mItemList[ itemNum ];
            if( pItem )
                if( mpSelected == pItem )
                {
                    color = ruVector3( 0, 200, 0 );
                    alpha = 255;
                }
            bool pressed = false;
            int cellX = coordX + distMult * mCellWidth * cw;
            int cellY = coordY + distMult * mCellHeight * ch;
            if( IsMouseInside( cellX, cellY, mCellWidth, mCellHeight ) )
            {
                color = ruVector3( 255, 0, 0 );
                alpha = 255;
                if( pItem != mpCombineItemFirst && pItem != mpCombineItemSecond )
                {
                    pPicked = pItem;
                    if( ruIsMouseHit( MB_Left ))
                    {
                        pressed = true;
                        mpSelected = pItem;
                    }
                }
            }
            ruDrawGUIRect( cellX, cellY, mCellWidth, mCellHeight, mCellTexture, color, alpha );
            if( pPicked )
            {
                if( ruIsMouseHit( MB_Right ) && combinePick )
                {
                    if( mpCombineItemFirst == 0 )
                    {
                        if( pPicked != mpCombineItemFirst )
                            mpCombineItemFirst = pPicked;
                    }
                    else if( mpCombineItemSecond == 0 )
                    {
                        if( pPicked != mpCombineItemSecond )
                            mpCombineItemSecond = pPicked;
                    }

                    combinePick = false;
                    mpSelected = 0;
                }
            }

            if( pItem )
            {
                if( pItem != mpCombineItemFirst && pItem != mpCombineItemSecond )
                {
                    int itemSpacing = 5;
                    ruDrawGUIRect( cellX + itemSpacing, cellY + itemSpacing, mCellWidth - 2 * itemSpacing, mCellHeight - 2 * itemSpacing, pItem->GetPictogram(), ruVector3( 255, 255, 255 ), alpha );

                    if( pItem == pPicked )
                    {
                        int offset = combineBoxY + 2.2f * mCellHeight;
                        ruDrawGUIText( pItem->GetDescription(), backgroundX + 2 * itemSpacing, descriptionY + 2.5 * itemSpacing, backgroundW - 2 * itemSpacing , combineH - 2 * itemSpacing, pGUI->mFont, ruVector3( 200, 200, 200 ), 0, alpha );
                        // draw characteristics of item
                        int charSpace = 28;
						string contentTypeFormatted = Format( "%s : %s", mLocalization.GetString( "contentType" ), pItem->GetContentType());
						string contentFormatted = Format( "%s: %f", mLocalization.GetString( "content" ), pItem->GetContent() );
						string volumeFormatted = Format( "%s: %f", mLocalization.GetString( "volume" ), pItem->GetVolume() );
						string massFormatted = Format( "%s: %f", mLocalization.GetString( "mass" ), pItem->GetMass() );
                        ruDrawGUIText( contentTypeFormatted.c_str(), actionsX + buttonSpace, offset + charSpace * 1, actionsW - itemSpacing, 100, mFont, whiteColor, 0 );
                        ruDrawGUIText( contentFormatted.c_str(), actionsX + buttonSpace, offset + charSpace * 2.1, actionsW - itemSpacing, 100, mFont, whiteColor, 0 );
                        ruDrawGUIText( volumeFormatted.c_str(), actionsX + buttonSpace, offset + charSpace * 3.1, actionsW - itemSpacing, 100, mFont, whiteColor, 0 );
                        ruDrawGUIText( massFormatted.c_str(), actionsX + buttonSpace, offset + charSpace * 4.1, actionsW - itemSpacing, 100, mFont, whiteColor, 0 );
                    }
                }
            }
        }
    }
}

void Inventory::RemoveItem( Item * pItem )
{
    mItemList.erase( find( mItemList.begin(), mItemList.end(), pItem ));
}

void Inventory::ThrowItem( Item * pItem )
{
    pItem->MarkAsFree();
    ruSetNodePosition( pItem->mObject, pPlayer->GetCurrentPosition() + 2 * pPlayer->GetLookDirection() + ruVector3( 0, 1, 0 ));
    ruUnfreeze( pItem->mObject );
    RemoveItem( pItem );
    pItem->SetContent( 0.0f );
    mpSelected = nullptr;
}

Inventory::~Inventory()
{

}

void Inventory::Open( bool val )
{
	mOpened = val;
}

bool Inventory::IsOpened() const
{
	return mOpened;
}

void Inventory::Deserialize( TextFileStream & in )
{

}

void Inventory::Serialize( TextFileStream & out )
{
	out.WriteInteger( GetItemCount() );
	for( auto pItem : pPlayer->mInventory.mItemList ) {
		// write object name for further identification
		out.WriteString( ruGetNodeName( pItem->mObject ) );
	}
}

void Inventory::AddItem( Item * pItem )
{
	mItemList.push_back( pItem );
}

bool Inventory::Contains( Item * pItem )
{
	for( auto npItem : mItemList )
	{
		if( npItem == pItem )
			return true;
	}
	return false;
}

void Inventory::ResetSelectedForUse()
{
	mpItemForUse = nullptr;
}

Item * Inventory::GetItemSelectedForUse()
{
	return mpItemForUse;
}

int Inventory::GetItemCount()
{
	return mItemList.size();
}

int Inventory::GetItemCount( Item::Type type )
{
	int count = 0;
	for( auto pItem : mItemList )
		if( pItem->GetType() == type )
			count++;
	return count;
}
