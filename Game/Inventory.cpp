#include "Precompiled.h"

#include "Inventory.h"
#include "GUIProperties.h"
#include "Player.h"


Inventory::Inventory() {
    mLocalization.ParseFile( localizationPath + "inventory.loc" );

    mBackgroundTexture = ruGetTexture( "data/gui/inventory/back.tga");
    mCellTexture = ruGetTexture( "data/gui/inventory/item.tga" );
    mButtonTexture = ruGetTexture( "data/gui/inventory/button.tga" );

    mOpen = false;
    mpSelectedItem = nullptr;

    mFont = ruCreateGUIFont( 14, "data/fonts/font1.otf" );

    mpCombineItemFirst = nullptr;
    mpCombineItemSecond = nullptr;

    mpItemForUse = nullptr;

    mPickSound = ruSound::Load2D( "data/sounds/menupick.ogg" );

    int screenCenterX = ruEngine::GetResolutionWidth() / 2;
    int screenCenterY = ruEngine::GetResolutionHeight() / 2;

    mGUIRectItemForUse = ruCreateGUIRect( screenCenterX, screenCenterY, 64, 64, ruTextureHandle::Empty(), pGUIProp->mForeColor, 255 );

    float distMult = 1.1f;
    int cellSpaceX = distMult * mCellWidth / (float)mCellCountWidth;
    int cellSpaceY = distMult * mCellHeight / (float)mCellCountHeight;
    int coordX = screenCenterX - 0.5f * (float)mCellCountWidth * (float)mCellWidth - cellSpaceX - 64;
    int coordY = screenCenterY - 0.5f * (float)mCellCountHeight * (float)mCellHeight - cellSpaceY - 64;
    // background
    int backGroundSpace = 20;
    int backgroundX = coordX - backGroundSpace;
    int backgroundY = coordY - backGroundSpace;
    int backgroundW = mCellCountWidth * mCellWidth + 2.5 * backGroundSpace + cellSpaceX + 128;
    int backgroundH = mCellCountHeight * mCellHeight + 2.5 * backGroundSpace + cellSpaceY + 128;
    mGUICanvas = ruCreateGUIRect( backgroundX, backgroundY, backgroundW, backgroundH, mBackgroundTexture, pGUIProp->mBackColor );
    int combineH = 128;
    int combineY = backgroundY + backgroundH - 128;
    int descriptionY = combineY + 10;
    mGUIDescription = ruCreateGUIText( mLocalization.GetString( "desc" ), backgroundX + 10, descriptionY, backgroundW - 128, combineH, mFont, pGUIProp->mForeColor, 0 );
    // item actions
    int actionsW = 128;
    int actionsX = backgroundX + backgroundW - 128;
    //mGUIActions = ruCreateGUIRect( actionsX, backgroundY, actionsW, backgroundH + combineH, mBackgroundTexture );
    //  actions buttons
    int buttonSpace = 10;
    int buttonsX = actionsX + buttonSpace;
    int buttonY = backgroundY + 2 * buttonSpace;
    int buttonH = 30;
    int buttonW = actionsW - 2 * buttonSpace;
    mGUIButtonUse = ruCreateGUIButton( buttonsX, buttonY, buttonW, buttonH, mButtonTexture, mLocalization.GetString( "use" ), mFont, pGUIProp->mForeColor, 1, 255 );
    mGUIButtonCombine = ruCreateGUIButton( buttonsX, buttonY + 1.5f * buttonH, buttonW, buttonH, mButtonTexture, mLocalization.GetString( "combine" ), mFont, pGUIProp->mForeColor, 1, 255 );
    // combine items
    int combineBoxY = buttonY + 3.6f * buttonH;
    ruVector3 combineColor1 = pGUIProp->mForeColor;
    ruVector3 combineColor2 = pGUIProp->mForeColor;
    int combineBoxSpacing = 5;
    buttonsX += mCellWidth / 2 - 2 * combineBoxSpacing;
    mGUIFirstCombineItem = ruCreateGUIRect( buttonsX + combineBoxSpacing, combineBoxY + combineBoxSpacing, mCellWidth - 2 * combineBoxSpacing, mCellHeight - 2 * combineBoxSpacing, ruTextureHandle::Empty() );
    mGUISecondCombineItem = ruCreateGUIRect( buttonsX + combineBoxSpacing, combineBoxY + 1.2f * mCellHeight + combineBoxSpacing, mCellWidth - 2 * combineBoxSpacing, mCellHeight - 2 * combineBoxSpacing, ruTextureHandle::Empty() );
    mGUIFirstCombineItemCell = ruCreateGUIRect( buttonsX, combineBoxY, mCellWidth, mCellHeight, mCellTexture, combineColor1, 255 );
    mGUISecondCombineItemCell = ruCreateGUIRect( buttonsX, combineBoxY + 1.2f * mCellHeight, mCellWidth, mCellHeight, mCellTexture, combineColor2, 255 );
    mGUICharacteristics = ruCreateGUIText( mLocalization.GetString( "characteristics" ), actionsX, combineBoxY + 1.5f * mCellHeight, 128, combineH, mFont, pGUIProp->mForeColor, 1 );

    int itemSpacing = 5;
    for( int cw = 0; cw < mCellCountWidth; cw++ ) {
        for( int ch = 0; ch < mCellCountHeight; ch++ ) {
            int cellX = coordX + distMult * mCellWidth * cw;
            int cellY = coordY + distMult * mCellHeight * ch;           
            mGUIItemCell[cw][ch] = ruCreateGUIRect( cellX, cellY, mCellWidth, mCellHeight, mCellTexture, pGUIProp->mForeColor, 255 );
			mGUIItem[cw][ch] = ruCreateGUIRect( cellX + itemSpacing, cellY + itemSpacing, mCellWidth - 2 * itemSpacing, mCellHeight - 2 * itemSpacing, ruTextureHandle::Empty(), ruVector3( 255, 255, 255 ), 255 );
			mGUIItemCountText[cw][ch] = ruCreateGUIText( "0", cellX + distMult * mCellWidth - 18, cellY + distMult * mCellHeight - 24, 8, 8, mFont, pGUIProp->mForeColor, 1 );
        }
    }

    int offset = combineBoxY + 2.2f * mCellHeight;
    mGUIItemDescription = ruCreateGUIText( "Desc", backgroundX + 2 * itemSpacing, descriptionY + 2.5 * itemSpacing, backgroundW - 2 * itemSpacing - 128 , combineH - 2 * itemSpacing, pGUIProp->mFont, ruVector3( 200, 200, 200 ), 0, 255 );
    // characteristics of item
    int charSpace = 28;
	mGUIItemContentType = ruCreateGUIText( "ContentType", actionsX + buttonSpace, offset + charSpace * 1, actionsW - itemSpacing, 100, mFont, pGUIProp->mForeColor, 0 );
	mGUIItemContent = ruCreateGUIText( "Content", actionsX + buttonSpace, offset + charSpace * 2.1, actionsW - itemSpacing, 100, mFont, pGUIProp->mForeColor, 0 );
    mGUIItemVolume = ruCreateGUIText( "Volume", actionsX + buttonSpace, offset + charSpace * 3.1, actionsW - itemSpacing, 100, mFont, pGUIProp->mForeColor, 0 );
    mGUIItemMass = ruCreateGUIText( "Mass", actionsX + buttonSpace, offset + charSpace * 4.1, actionsW - itemSpacing, 100, mFont, pGUIProp->mForeColor, 0 );

    SetVisible( false );
}

void Inventory::SetVisible( bool state ) {
	mOpen = state;
    ruSetGUINodeVisible( mGUICanvas, state );
    ruSetGUINodeVisible( mGUIRectItemForUse, state );
    ruSetGUINodeVisible( mGUICanvas, state );
    ruSetGUINodeVisible( mGUIDescription, state );
    ruSetGUINodeVisible( mGUIButtonUse, state );
    ruSetGUINodeVisible( mGUIButtonCombine, state );
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
			ruSetGUINodeVisible( mGUIItemCountText[cw][ch], state );
        }
    }
}

bool Inventory::IsMouseInside( int x, int y, int w, int h ) {
    return ruGetMouseX() > x && ruGetMouseX() < ( x + w ) && ruGetMouseY() > y && ruGetMouseY() < ( y + h );
}

void Inventory::DoCombine() {
    if( mpCombineItemFirst->Combine( mpCombineItemSecond->GetType() )) { // combine successfull
        mpCombineItemFirst = nullptr;
        mpCombineItemSecond = nullptr;
        mpSelectedItem = nullptr;
    }
}

void Inventory::Update() {
    ruVector3 whiteColor = ruVector3( 255, 255, 255 );
    int screenCenterX = ruEngine::GetResolutionWidth() / 2;
    int screenCenterY = ruEngine::GetResolutionHeight() / 2;

    if( mpItemForUse ) {
        ruEngine::HideCursor();
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
        ruEngine::HideCursor();
        return;
    }

    ruEngine::ShowCursor();

    float distMult = 1.1f;
    int cellSpaceX = distMult * mCellWidth / (float)mCellCountWidth;
    int cellSpaceY = distMult * mCellHeight / (float)mCellCountHeight;
    int coordX = screenCenterX - 0.5f * (float)mCellCountWidth * (float)mCellWidth - cellSpaceX - 64;
    int coordY = screenCenterY - 0.5f * (float)mCellCountHeight * (float)mCellHeight - cellSpaceY - 64;
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

    ruSetGUINodeAlpha( mGUIButtonUse, useAlpha );
    ruSetGUINodeAlpha( ruGetButtonText( mGUIButtonUse ), useAlpha );
    int combineAlpha = canCombine ? 255 : 60;
    ruSetGUINodeAlpha( mGUIButtonCombine, combineAlpha );
    ruSetGUINodeAlpha( ruGetButtonText( mGUIButtonCombine ), combineAlpha );

    // draw combine items
    int combineBoxY = buttonY + 3.6f * buttonH;
    ruVector3 combineColor1 = pGUIProp->mForeColor;
    ruVector3 combineColor2 = pGUIProp->mForeColor;
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
            ruVector3 color = pGUIProp->mForeColor;
            int alpha = 180;
            // get item for draw
            int itemNum = cw * mCellCountHeight + ch;
            Item * pItem = nullptr;
			int i = 0;
			//cout << mItemMap.size() << endl;
			int curItemCount = 0;
			for( auto & itemCountPair = mItemMap.begin(); itemCountPair != mItemMap.end(); itemCountPair++ ) {
				if( i == itemNum ) {
					pItem = const_cast<Item*>( &itemCountPair->first );
					curItemCount  = itemCountPair->second;
					break;
				}
				i++;
			}
            if( pItem ) {
                if( mpSelectedItem == pItem ) {
                    color = ruVector3( 0, 200, 0 );
                    alpha = 255;
                }
				ruSetGUINodeText( mGUIItemCountText[cw][ch], StringBuilder() << curItemCount );
				if( pItem != mpCombineItemFirst && pItem != mpCombineItemSecond ) {
					ruSetGUINodeVisible( mGUIItemCountText[cw][ch], true );
				} else {
					ruSetGUINodeVisible( mGUIItemCountText[cw][ch], false );
				}
			} else {
				ruSetGUINodeVisible( mGUIItemCountText[cw][ch], false );
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

void Inventory::RemoveItem( Item::Type type, int count ) {
	for( auto & itemCountPair = mItemMap.begin(); itemCountPair != mItemMap.end(); itemCountPair++ ) {
		if( itemCountPair->first.GetType() == type ) {
			itemCountPair->second -= count;
			if( itemCountPair->second <= 0 ) {
				mItemMap.erase( itemCountPair );
				break;
			}
		}
	}
}

Inventory::~Inventory() {
	mPickSound.Free();
	ruFreeGUINode( mGUIRectItemForUse );
	ruFreeGUINode( mGUICanvas );
	ruFreeGUINode( mGUIDescription );
	ruFreeGUINode( mGUIButtonUse );
	ruFreeGUINode( mGUIButtonCombine );
	ruFreeGUINode( mGUIFirstCombineItem );
	ruFreeGUINode( mGUISecondCombineItem );
	ruFreeGUINode( mGUIFirstCombineItemCell );
	ruFreeGUINode( mGUISecondCombineItemCell );
	ruFreeGUINode( mGUICharacteristics );
	for( int i = 0; i < mCellCountWidth; i++ ) {
		for( int j = 0; j < mCellCountHeight; j++ ) {
			ruFreeGUINode( mGUIItem[i][j] );
			ruFreeGUINode( mGUIItemCell[i][j] );
			ruFreeGUINode( mGUIItemCountText[i][j] );
		}
	}
	ruFreeGUINode( mGUIItemDescription );
	ruFreeGUINode( mGUIItemMass );
	ruFreeGUINode( mGUIItemContent );
	ruFreeGUINode( mGUIItemContentType );
	ruFreeGUINode( mGUIItemVolume );
	mFont.Free();
}

void Inventory::Open( bool val ) {
    mOpen = val;
    SetVisible( val );
}

bool Inventory::IsOpened() const {
    return mOpen;
}

void Inventory::Deserialize( SaveFile & in ) {
	int count = in.ReadInteger();
	for( int i = 0; i < count; i++ ) {
		Item::Type type = static_cast<Item::Type>( in.ReadInteger());
		mItemMap[ Item( type ) ] = in.ReadInteger();	
	}
}

void Inventory::Serialize( SaveFile & out ) {
	out.WriteInteger( mItemMap.size() );
	for( auto & itemCountPair = mItemMap.begin(); itemCountPair != mItemMap.end(); itemCountPair++ ) {
		// write item type
		out.WriteInteger( static_cast<int>( itemCountPair->first.GetType() ));
		// write count of this items
		out.WriteInteger( static_cast<int>( itemCountPair->second )); 
	}
}

void Inventory::AddItem( Item::Type type ) {
	if( type != Item::Type::Unknown ) {
		bool found = false;
		for( auto & itemCountPair = mItemMap.begin(); itemCountPair != mItemMap.end(); itemCountPair++ ) {
			if( itemCountPair->first.GetType() == type ) {
				found = true;
				if( !itemCountPair->first.mSingleInstance ) {
					itemCountPair->second++;
				} 
			}
		}
		if( !found ) {			
			mItemMap[ Item( type ) ] = 1;
			cout << "Added item!" << mItemMap.size() << endl;
		}	
	}
}

void Inventory::ResetSelectedForUse() {
    mpItemForUse = nullptr;
}

Item * Inventory::GetItemSelectedForUse() {
    return mpItemForUse;
}

int Inventory::GetItemCount( Item::Type type ) {
    for( auto & itemCountPair = mItemMap.begin(); itemCountPair != mItemMap.end(); itemCountPair++ ) {
        if( itemCountPair->first.GetType() == type ) {
            return itemCountPair->second;
        }
	}
    return 0;
}

void Inventory::GetItems( map<Item,int> & itemMap ) {
	itemMap = mItemMap;
}

void Inventory::SetItems( map<Item,int> & items )
{
	mItemMap = items;
}

