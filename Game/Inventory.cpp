#include "Precompiled.h"

#include "Inventory.h"
#include "GUI.h"
#include "Player.h"


Inventory::Inventory() {
    mLocalization.ParseFile( localizationPath + "inventory.loc" );

    mBackgroundTexture = ruGetTexture( "data/gui/inventory/inventoryBackground.png");
    mCellTexture = ruGetTexture( "data/gui/inventory/inventoryCell.png" );
    mButtonTexture = ruGetTexture( "data/gui/inventory/inventoryButton.png" );

    mOpen = false;
    mpSelectedItem = nullptr;

    mFont = ruCreateGUIFont( 14, "data/fonts/font1.otf" );

    mpCombineItemFirst = nullptr;
    mpCombineItemSecond = nullptr;

    mpItemForUse = nullptr;

    mPickSound = ruLoadSound2D( "data/sounds/menupick.ogg" );

    ruVector3 whiteColor = ruVector3( 255, 255, 255 );
    int screenCenterX = ruGetResolutionWidth() / 2;
    int screenCenterY = ruGetResolutionHeight() / 2;

    mGUIRectItemForUse = ruCreateGUIRect( screenCenterX, screenCenterY, 64, 64, ruTextureHandle::Empty(), whiteColor, 255 );

    float distMult = 1.1f;
    int cellSpaceX = distMult * mCellWidth / (float)mCellCountWidth;
    int cellSpaceY = distMult * mCellHeight / (float)mCellCountHeight;
    int coordX = screenCenterX - 0.5f * (float)mCellCountWidth * (float)mCellWidth - cellSpaceX;
    int coordY = screenCenterY - 0.5f * (float)mCellCountHeight * (float)mCellHeight - cellSpaceY;
    // background
    int backGroundSpace = 20;
    int backgroundX = coordX - backGroundSpace;
    int backgroundY = coordY - backGroundSpace;
    int backgroundW = mCellCountWidth * mCellWidth + 2.5 * backGroundSpace + cellSpaceX;
    int backgroundH = mCellCountHeight * mCellHeight + 2.5 * backGroundSpace + cellSpaceY;
    mGUICanvas = ruCreateGUIRect( backgroundX, backgroundY, backgroundW, backgroundH, mBackgroundTexture );
    int combineH = 128;
    int combineY = backgroundY + backgroundH;
    int descriptionY = combineY + 10;
    mGUIRightPanel = ruCreateGUIRect( backgroundX, combineY, backgroundW, combineH, mBackgroundTexture );
	//ruAttachGUINode( mGUIRightPanel, mGUICanvas );
    mGUIDescription = ruCreateGUIText( mLocalization.GetString( "desc" ), backgroundX + 10, descriptionY, backgroundW, combineH, mFont, whiteColor, 0 );
	//ruAttachGUINode( mGUIDescription, mGUICanvas );
    // item actions
    int actionsW = 128;
    int actionsX = backgroundX + backgroundW;
    mGUIActions = ruCreateGUIRect( actionsX, backgroundY, actionsW, backgroundH + combineH, mBackgroundTexture );
    //  actions buttons
    int buttonSpace = 10;
    int buttonsX = actionsX + buttonSpace;
    int buttonY = backgroundY + 2 * buttonSpace;
    int buttonH = 30;
    int buttonW = actionsW - 2 * buttonSpace;
    mGUIButtonUse = ruCreateGUIButton( buttonsX, buttonY, buttonW, buttonH, mButtonTexture, mLocalization.GetString( "use" ), mFont, whiteColor, 1, 255 );
    mGUIButtonThrow = ruCreateGUIButton( buttonsX, buttonY + 1.2f * buttonH, buttonW, buttonH, mButtonTexture, mLocalization.GetString( "throw" ), mFont, whiteColor, 1, 255 );
    mGUIButtonCombine = ruCreateGUIButton( buttonsX, buttonY + 2.4f * buttonH, buttonW, buttonH, mButtonTexture, mLocalization.GetString( "combine" ), mFont, whiteColor, 1, 255 );
    // combine items
    int combineBoxY = buttonY + 3.6f * buttonH;
    ruVector3 combineColor1 = whiteColor;
    ruVector3 combineColor2 = whiteColor;
    int combineBoxSpacing = 5;
    buttonsX += mCellWidth / 2 - 2 * combineBoxSpacing;
    mGUIFirstCombineItem = ruCreateGUIRect( buttonsX + combineBoxSpacing, combineBoxY + combineBoxSpacing, mCellWidth - 2 * combineBoxSpacing, mCellHeight - 2 * combineBoxSpacing, ruTextureHandle::Empty() );
    mGUISecondCombineItem = ruCreateGUIRect( buttonsX + combineBoxSpacing, combineBoxY + 1.2f * mCellHeight + combineBoxSpacing, mCellWidth - 2 * combineBoxSpacing, mCellHeight - 2 * combineBoxSpacing, ruTextureHandle::Empty() );
    mGUIFirstCombineItemCell = ruCreateGUIRect( buttonsX, combineBoxY, mCellWidth, mCellHeight, mCellTexture, combineColor1, 255 );
    mGUISecondCombineItemCell = ruCreateGUIRect( buttonsX, combineBoxY + 1.2f * mCellHeight, mCellWidth, mCellHeight, mCellTexture, combineColor2, 255 );
    mGUICharacteristics = ruCreateGUIText( mLocalization.GetString( "characteristics" ), actionsX + buttonSpace, combineBoxY + 2.2f * mCellHeight, backgroundW, combineH, mFont, whiteColor, 0 );

    int itemSpacing = 5;
    // cells and item image
    for( int cw = 0; cw < mCellCountWidth; cw++ ) {
        for( int ch = 0; ch < mCellCountHeight; ch++ ) {
            // get item for draw
            int cellX = coordX + distMult * mCellWidth * cw;
            int cellY = coordY + distMult * mCellHeight * ch;
            mGUIItem[cw][ch] = ruCreateGUIRect( cellX + itemSpacing, cellY + itemSpacing, mCellWidth - 2 * itemSpacing, mCellHeight - 2 * itemSpacing, ruTextureHandle::Empty(), ruVector3( 255, 255, 255 ), 255 );
            mGUIItemCell[cw][ch] = ruCreateGUIRect( cellX, cellY, mCellWidth, mCellHeight, mCellTexture, whiteColor, 255 );
        }
    }

    int offset = combineBoxY + 2.2f * mCellHeight;
    mGUIItemDescription = ruCreateGUIText( "Desc", backgroundX + 2 * itemSpacing, descriptionY + 2.5 * itemSpacing, backgroundW - 2 * itemSpacing , combineH - 2 * itemSpacing, pGUI->mFont, ruVector3( 200, 200, 200 ), 0, 255 );
    // characteristics of item
    int charSpace = 28;
    mGUIItemContentType = ruCreateGUIText( "ContentType", actionsX + buttonSpace, offset + charSpace * 1, actionsW - itemSpacing, 100, mFont, whiteColor, 0 );
    mGUIItemContent = ruCreateGUIText( "Content", actionsX + buttonSpace, offset + charSpace * 2.1, actionsW - itemSpacing, 100, mFont, whiteColor, 0 );
    mGUIItemVolume = ruCreateGUIText( "Volume", actionsX + buttonSpace, offset + charSpace * 3.1, actionsW - itemSpacing, 100, mFont, whiteColor, 0 );
    mGUIItemMass = ruCreateGUIText( "Mass", actionsX + buttonSpace, offset + charSpace * 4.1, actionsW - itemSpacing, 100, mFont, whiteColor, 0 );

    SetVisible( false );
}

void Inventory::SetVisible( bool state ) {
	mOpen = state;
    ruSetGUINodeVisible( mGUICanvas, state );
    ruSetGUINodeVisible( mGUIRectItemForUse, state );
    ruSetGUINodeVisible( mGUICanvas, state );
    ruSetGUINodeVisible( mGUIRightPanel, state );
    ruSetGUINodeVisible( mGUIDescription, state );
    ruSetGUINodeVisible( mGUIActions, state );
    ruSetGUINodeVisible( mGUIButtonUse, state );
    ruSetGUINodeVisible( mGUIButtonCombine, state );
    ruSetGUINodeVisible( mGUIButtonThrow, state );
    ruSetGUINodeVisible( mGUIFirstCombineItem, state );
    ruSetGUINodeVisible( mGUISecondCombineItem, state );
    ruSetGUINodeVisible( mGUIFirstCombineItemCell, state );
    ruSetGUINodeVisible( mGUISecondCombineItemCell, state );
    ruSetGUINodeVisible( mGUICharacteristics, state );
    ruSetGUINodeVisible( mGUIItem[mCellCountWidth][mCellCountHeight], state );
    ruSetGUINodeVisible( mGUIItemCell[mCellCountWidth][mCellCountHeight], state );
    ruSetGUINodeVisible( mGUIItemDescription, state );
    ruSetGUINodeVisible( mGUIItemMass, state );
    ruSetGUINodeVisible( mGUIItemContent, state );
    ruSetGUINodeVisible( mGUIItemContentType, state );
    ruSetGUINodeVisible( mGUIItemVolume, state );
    for( int cw = 0; cw < mCellCountWidth; cw++ ) {
        for( int ch = 0; ch < mCellCountHeight; ch++ ) {
            ruSetGUINodeVisible( mGUIItemCell[cw][ch], state );
            ruSetGUINodeVisible( mGUIItem[cw][ch], state );
        }
    }
}

bool Inventory::IsMouseInside( int x, int y, int w, int h ) {
    return ruGetMouseX() > x && ruGetMouseX() < ( x + w ) && ruGetMouseY() > y && ruGetMouseY() < ( y + h );
}

void Inventory::DoCombine() {
    Item * pUsedItem = nullptr;
    if( mpCombineItemFirst->Combine( mpCombineItemSecond, pUsedItem )) { // combine successfull
        mpCombineItemFirst = nullptr;
        mpCombineItemSecond = nullptr;
        mpSelectedItem = nullptr;
        if( pUsedItem ) {
            ThrowItem( pUsedItem );
        }
    }
}

void Inventory::Update() {
	for( int i = 0; i < mItemList.size(); i++ ) {
		if( mItemList[i]->mCanBeDeleted ) {
			delete mItemList[i];
		}
	}

    ruVector3 whiteColor = ruVector3( 255, 255, 255 );
    int screenCenterX = ruGetResolutionWidth() / 2;
    int screenCenterY = ruGetResolutionHeight() / 2;

    if( mpItemForUse ) {
        ruHideCursor();
        SetVisible( false );
        ruSetGUINodeTexture( mGUIRectItemForUse, mpItemForUse->GetPictogram() );
        ruSetGUINodeVisible( mGUIRectItemForUse, true );

        if( ruIsMouseHit( MB_Left )) {
            mpItemForUse = nullptr;
            mpSelectedItem = nullptr;
            SetVisible( true );
        }
        if( mOpen ) {
            mpItemForUse = nullptr;
        }
        return;
    } else {
        ruSetGUINodeVisible( mGUIRectItemForUse, false );
    }

    if( !mOpen ) {
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
    // draw item actions
    int actionsX = backgroundX + backgroundW;
    // draw actions buttons
    int buttonSpace = 10;
    int buttonsX = actionsX + buttonSpace;
    int buttonY = backgroundY + 2 * buttonSpace;
    int buttonH = 30;


    bool canCombine = ( mpCombineItemFirst != 0 && mpCombineItemSecond != 0 );
    int useAlpha = mpSelectedItem ? 255 : 60;
	int throwAlpha = 255;
	if( mpSelectedItem ) {
		if( mpSelectedItem->IsThrowable() ) {
			throwAlpha = 60;
		}
	}
    ruSetGUINodeAlpha( mGUIButtonUse, useAlpha );
    ruSetGUINodeAlpha( ruGetButtonText( mGUIButtonUse ), useAlpha );
    ruSetGUINodeAlpha( mGUIButtonThrow, throwAlpha );
    ruSetGUINodeAlpha( ruGetButtonText( mGUIButtonThrow ), useAlpha );
    int combineAlpha = canCombine ? 255 : 60;
    ruSetGUINodeAlpha( mGUIButtonCombine, combineAlpha );
    ruSetGUINodeAlpha( ruGetButtonText( mGUIButtonCombine ), combineAlpha );

    // draw combine items
    int combineBoxY = buttonY + 3.6f * buttonH;
    ruVector3 combineColor1 = whiteColor;
    ruVector3 combineColor2 = whiteColor;
    int combineBoxSpacing = 5;
    buttonsX += mCellWidth / 2 - 2 * combineBoxSpacing;

	ruSetGUINodeTexture( mGUIFirstCombineItem, ruTextureHandle::Empty() );
	ruSetGUINodeTexture( mGUISecondCombineItem, ruTextureHandle::Empty() );

    if( mpCombineItemFirst ) {				
        ruSetGUINodeVisible( mGUIFirstCombineItem, true );
		ruSetGUINodeTexture( mGUIFirstCombineItem, mpCombineItemFirst->GetPictogram() );
        if( IsMouseInside( buttonsX, combineBoxY, mCellWidth, mCellHeight )) {
            combineColor1 = ruVector3( 255, 0, 0 );
            if( ruIsMouseHit( MB_Left )) {
                mpCombineItemFirst = 0;
            }
        }
    } else {
        ruSetGUINodeVisible( mGUIFirstCombineItem, false );
    };

    if( mpCombineItemSecond ) {
        ruSetGUINodeVisible( mGUISecondCombineItem, true );
		ruSetGUINodeTexture( mGUISecondCombineItem, mpCombineItemSecond->GetPictogram() );
        if( IsMouseInside( buttonsX, combineBoxY + 1.2f * mCellHeight, mCellWidth, mCellHeight )) {
            combineColor2 = ruVector3( 255, 0, 0 );
            if( ruIsMouseHit( MB_Left )) {
                mpCombineItemSecond = 0;
            }
        }
    } else {
        ruSetGUINodeVisible( mGUISecondCombineItem, false );
    }

    ruSetGUINodeColor( mGUISecondCombineItemCell, combineColor2 );
    ruSetGUINodeColor( mGUIFirstCombineItemCell, combineColor1 );

    // do combine
    if( ruIsButtonHit( mGUIButtonCombine )) {
        if( canCombine ) {
            DoCombine();
        }
    }

    // use item
    if( ruIsButtonHit( mGUIButtonUse )) {
        if( mpSelectedItem ) {
            mpItemForUse = mpSelectedItem;
            mOpen = false;
        }
    }

    // throw item
    if( ruIsButtonHit( mGUIButtonThrow )) {
        if( mpSelectedItem ) {
            if( mpSelectedItem->IsThrowable() ) {
                ThrowItem( mpSelectedItem );
            }
        }
    }

	ruSetGUINodeVisible( mGUIItemDescription, false );
	ruSetGUINodeVisible( mGUIItemContentType, false );
	ruSetGUINodeVisible( mGUIItemContent, false );
	ruSetGUINodeVisible( mGUIItemVolume, false );
	ruSetGUINodeVisible( mGUIItemMass, false );

    // draw cells and item image
    bool combinePick = true;
    for( int cw = 0; cw < mCellCountWidth; cw++ ) {
        for( int ch = 0; ch < mCellCountHeight; ch++ ) {
            Item * pPicked = 0;
            ruVector3 color = whiteColor;
            int alpha = 180;
            // get item for draw
            int itemNum = cw * mCellCountHeight + ch;
            Item * pItem = nullptr;
            if( itemNum < mItemList.size() ) {
                pItem = mItemList[ itemNum ];
            }
            if( pItem )
                if( mpSelectedItem == pItem ) {
                    color = ruVector3( 0, 200, 0 );
                    alpha = 255;
                }
            bool pressed = false;
            int cellX = coordX + distMult * mCellWidth * cw;
            int cellY = coordY + distMult * mCellHeight * ch;
            if( IsMouseInside( cellX, cellY, mCellWidth, mCellHeight ) ) {
                color = ruVector3( 255, 0, 0 );
                alpha = 255;
                if( pItem != mpCombineItemFirst && pItem != mpCombineItemSecond ) {
                    pPicked = pItem;
                    if( ruIsMouseHit( MB_Left )) {
                        pressed = true;
                        mpSelectedItem = pItem;
                    }
                }
            }
            ruSetGUINodeColor( mGUIItemCell[cw][ch], color );
            if( pPicked ) {
                if( ruIsMouseHit( MB_Right ) && combinePick ) {
                    if( mpCombineItemFirst == 0 ) {
                        if( pPicked != mpCombineItemFirst ) {
                            mpCombineItemFirst = pPicked;
                        }
                    } else if( mpCombineItemSecond == 0 ) {
                        if( pPicked != mpCombineItemSecond ) {
                            mpCombineItemSecond = pPicked;
                        }
                    }

                    combinePick = false;
                    mpSelectedItem = 0;
                }
            }
			
            if( pItem ) {				
                if( pItem != mpCombineItemFirst && pItem != mpCombineItemSecond ) {
					ruSetGUINodeVisible( mGUIItem[cw][ch], true );
                    ruSetGUINodeTexture( mGUIItem[cw][ch], pItem->GetPictogram());
                    if( pItem == pPicked ) {
						ruSetGUINodeVisible( mGUIItemDescription, true );
						ruSetGUINodeVisible( mGUIItemContentType, true );
						ruSetGUINodeVisible( mGUIItemContent, true );
						ruSetGUINodeVisible( mGUIItemVolume, true );
						ruSetGUINodeVisible( mGUIItemMass, true );
                        ruSetGUINodeText( mGUIItemDescription, pItem->GetDescription());
                        ruSetGUINodeText( mGUIItemContentType, StringBuilder() << mLocalization.GetString( "contentType" ) << ": " << pItem->GetContentType() );
                        ruSetGUINodeText( mGUIItemContent, StringBuilder() << mLocalization.GetString( "content" ) << ": " << pItem->GetContent() );
                        ruSetGUINodeText( mGUIItemVolume, StringBuilder() << mLocalization.GetString( "volume" ) << ": " << pItem->GetVolume() );
                        ruSetGUINodeText( mGUIItemMass, StringBuilder() << mLocalization.GetString( "mass" ) << ": " << pItem->GetMass() );
                    }
                } else {
					ruSetGUINodeVisible( mGUIItem[cw][ch], false );
				}
			} else {
				ruSetGUINodeVisible( mGUIItem[cw][ch], false );
			}
        }
    }
}

void Inventory::RemoveItem( Item * pItem ) {
	auto iter = find( mItemList.begin(), mItemList.end(), pItem );
	if( iter != mItemList.end() ) {
		mItemList.erase( iter );
	}
}

void Inventory::ThrowItem( Item * pItem ) {	
    pItem->MarkAsFree();
	ruVector3 pickPoint, playerPos = pPlayer->GetCurrentPosition();
	ruNodeHandle handle = ruCastRay( playerPos - ruVector3( 0,0.1,0), playerPos - ruVector3(0,100,0), &pickPoint );
	if( handle.IsValid()) {
		ruSetNodePosition( pItem->mObject, pickPoint );
	} else {
		ruSetNodePosition( pItem->mObject, playerPos );
	}
    RemoveItem( pItem );
    pItem->SetContent( 0.0f );
    mpSelectedItem = nullptr;
}

Inventory::~Inventory() {
	ruFreeGUINode( mGUIRectItemForUse );
	ruFreeGUINode( mGUICanvas );
	ruFreeGUINode( mGUIRightPanel );
	ruFreeGUINode( mGUIDescription );
	ruFreeGUINode( mGUIActions );
	ruFreeGUINode( mGUIButtonUse );
	ruFreeGUINode( mGUIButtonCombine );
	ruFreeGUINode( mGUIButtonThrow );
	ruFreeGUINode( mGUIFirstCombineItem );
	ruFreeGUINode( mGUISecondCombineItem );
	ruFreeGUINode( mGUIFirstCombineItemCell );
	ruFreeGUINode( mGUISecondCombineItemCell );
	ruFreeGUINode( mGUICharacteristics );
	for( int i = 0; i < mCellCountWidth; i++ ) {
		for( int j = 0; j < mCellCountHeight; j++ ) {
			ruFreeGUINode( mGUIItem[i][j] );
			ruFreeGUINode( mGUIItemCell[i][j] );
		}
	}
	ruFreeGUINode( mGUIItemDescription );
	ruFreeGUINode( mGUIItemMass );
	ruFreeGUINode( mGUIItemContent );
	ruFreeGUINode( mGUIItemContentType );
	ruFreeGUINode( mGUIItemVolume );
}

void Inventory::Open( bool val ) {
    mOpen = val;
    SetVisible( val );
}

bool Inventory::IsOpened() const {
    return mOpen;
}

void Inventory::Deserialize( TextFileStream & in ) {

}

void Inventory::Serialize( TextFileStream & out ) {
    out.WriteInteger( GetItemCount() );
    for( auto pItem : pPlayer->mInventory.mItemList ) {
        // write object name for further identification
        out.WriteString( ruGetNodeName( pItem->mObject ) );
    }
}

void Inventory::AddItem( Item * pItem ) {
    mItemList.push_back( pItem );
}

bool Inventory::Contains( Item * pItem ) {
    for( auto npItem : mItemList ) {
        if( npItem == pItem ) {
            return true;
        }
    }
    return false;
}

void Inventory::ResetSelectedForUse() {
    mpItemForUse = nullptr;
}

Item * Inventory::GetItemSelectedForUse() {
    return mpItemForUse;
}

int Inventory::GetItemCount() {
    return mItemList.size();
}

int Inventory::GetItemCount( Item::Type type ) {
    int count = 0;
    for( auto pItem : mItemList )
        if( pItem->GetType() == type ) {
            count++;
        }
    return count;
}

void Inventory::Repair() {
	for( auto pItem : mItemList ) {
		pItem->Repair();
	}
}

bool Inventory::GotAnyItemOfType( Item::Type type ) {
	for( auto pItem : mItemList ) {
		if( pItem->GetType() == type ) {
			return true;
		}
	}
	return false;
}

void Inventory::GetItemList( vector<Item::Type> & itemList )
{
	for( auto pItem : mItemList ) {
		itemList.push_back( pItem->GetType() );
	}
}

