#include "Precompiled.h"

#include "Inventory.h"
#include "GUIProperties.h"
#include "Player.h"


Inventory::Inventory() {
    mLocalization.ParseFile( localizationPath + "inventory.loc" );

    mBackgroundTexture = ruTexture::Request( "data/gui/inventory/back.tga");
    mCellTexture = ruTexture::Request( "data/gui/inventory/item.tga" );
    mButtonTexture = ruTexture::Request( "data/gui/inventory/button.tga" );

    mOpen = false;
    mpSelectedItem = nullptr;

    mFont = ruFont::LoadFromFile( 14, "data/fonts/font1.otf" );

    mpCombineItemFirst = nullptr;
    mpCombineItemSecond = nullptr;

    mpItemForUse = nullptr;

    mPickSound = ruSound::Load2D( "data/sounds/menupick.ogg" );

    int screenCenterX = ruVirtualScreenWidth / 2;
    int screenCenterY = ruVirtualScreenHeight / 2;

    mGUIRectItemForUse = ruRect::Create( screenCenterX, screenCenterY, 64, 64, nullptr, pGUIProp->mForeColor, 255 );

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
    mGUICanvas = ruRect::Create( backgroundX, backgroundY, backgroundW, backgroundH, mBackgroundTexture, pGUIProp->mBackColor );
    int combineH = 128;
    int combineY = backgroundY + backgroundH - 128;
    int descriptionY = combineY + 10;
    mGUIDescription = ruText::Create( mLocalization.GetString( "desc" ), backgroundX + 10, descriptionY, backgroundW - 128, combineH, mFont, pGUIProp->mForeColor, ruTextAlignment::Left );
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
    mGUIButtonUse = ruButton::Create( buttonsX, buttonY, buttonW, buttonH, mButtonTexture, mLocalization.GetString( "use" ), mFont, pGUIProp->mForeColor, ruTextAlignment::Center, 255 );
    mGUIButtonCombine = ruButton::Create( buttonsX, buttonY + 1.5f * buttonH, buttonW, buttonH, mButtonTexture, mLocalization.GetString( "combine" ), mFont, pGUIProp->mForeColor, ruTextAlignment::Center, 255 );
    // combine items
    int combineBoxY = buttonY + 3.6f * buttonH;
    ruVector3 combineColor1 = pGUIProp->mForeColor;
    ruVector3 combineColor2 = pGUIProp->mForeColor;
    int combineBoxSpacing = 5;
    buttonsX += mCellWidth / 2 - 2 * combineBoxSpacing;
    mGUIFirstCombineItem = ruRect::Create( buttonsX + combineBoxSpacing, combineBoxY + combineBoxSpacing, mCellWidth - 2 * combineBoxSpacing, mCellHeight - 2 * combineBoxSpacing, nullptr );
    mGUISecondCombineItem = ruRect::Create( buttonsX + combineBoxSpacing, combineBoxY + 1.2f * mCellHeight + combineBoxSpacing, mCellWidth - 2 * combineBoxSpacing, mCellHeight - 2 * combineBoxSpacing, nullptr );
    mGUIFirstCombineItemCell = ruRect::Create( buttonsX, combineBoxY, mCellWidth, mCellHeight, mCellTexture, combineColor1, 255 );
    mGUISecondCombineItemCell = ruRect::Create( buttonsX, combineBoxY + 1.2f * mCellHeight, mCellWidth, mCellHeight, mCellTexture, combineColor2, 255 );
    mGUICharacteristics = ruText::Create( mLocalization.GetString( "characteristics" ), actionsX, combineBoxY + 1.5f * mCellHeight, 128, combineH, mFont, pGUIProp->mForeColor, ruTextAlignment::Center );

    int itemSpacing = 5;
    for( int cw = 0; cw < mCellCountWidth; cw++ ) {
        for( int ch = 0; ch < mCellCountHeight; ch++ ) {
            int cellX = coordX + distMult * mCellWidth * cw;
            int cellY = coordY + distMult * mCellHeight * ch;           
            mGUIItemCell[cw][ch] = ruRect::Create( cellX, cellY, mCellWidth, mCellHeight, mCellTexture, pGUIProp->mForeColor, 255 );
			mGUIItem[cw][ch] = ruRect::Create( cellX + itemSpacing, cellY + itemSpacing, mCellWidth - 2 * itemSpacing, mCellHeight - 2 * itemSpacing, nullptr, ruVector3( 255, 255, 255 ), 255 );
			mGUIItemCountText[cw][ch] = ruText::Create( "0", cellX + distMult * mCellWidth - 18, cellY + distMult * mCellHeight - 24, 8, 8, mFont, pGUIProp->mForeColor, ruTextAlignment::Center );
        }
    }

    int offset = combineBoxY + 2.2f * mCellHeight;
    mGUIItemDescription =  ruText::Create( "Desc", backgroundX + 2 * itemSpacing, descriptionY + 2.5 * itemSpacing, backgroundW - 2 * itemSpacing - 128 , combineH - 2 * itemSpacing, pGUIProp->mFont, ruVector3( 200, 200, 200 ), ruTextAlignment::Left, 255 );
    // characteristics of item
    int charSpace = 28;
	mGUIItemContentType = ruText::Create( "ContentType", actionsX + buttonSpace, offset + charSpace * 1, actionsW - itemSpacing, 100, mFont, pGUIProp->mForeColor, ruTextAlignment::Left );
	mGUIItemContent =  ruText::Create( "Content", actionsX + buttonSpace, offset + charSpace * 2.1, actionsW - itemSpacing, 100, mFont, pGUIProp->mForeColor, ruTextAlignment::Left );
    mGUIItemVolume =  ruText::Create( "Volume", actionsX + buttonSpace, offset + charSpace * 3.1, actionsW - itemSpacing, 100, mFont, pGUIProp->mForeColor, ruTextAlignment::Left );
    mGUIItemMass =  ruText::Create( "Mass", actionsX + buttonSpace, offset + charSpace * 4.1, actionsW - itemSpacing, 100, mFont, pGUIProp->mForeColor, ruTextAlignment::Left );

    SetVisible( false );
}

void Inventory::SetVisible( bool state ) {
	mOpen = state;
    mGUICanvas->SetVisible( state );
    mGUIRectItemForUse->SetVisible( state );
    mGUICanvas->SetVisible( state );
    mGUIDescription->SetVisible( state );
    mGUIButtonUse->SetVisible( state );
    mGUIButtonCombine->SetVisible( state );
    mGUIFirstCombineItem->SetVisible( state );
    mGUISecondCombineItem->SetVisible( state );
    mGUIFirstCombineItemCell->SetVisible( state );
    mGUISecondCombineItemCell->SetVisible( state );
    mGUICharacteristics->SetVisible( state );
    mGUIItemDescription->SetVisible( state );
    mGUIItemMass->SetVisible( state );
    mGUIItemContent->SetVisible( state );
    mGUIItemContentType->SetVisible( state );
     mGUIItemVolume->SetVisible( state );
    for( int cw = 0; cw < mCellCountWidth; cw++ ) {
        for( int ch = 0; ch < mCellCountHeight; ch++ ) {
            mGUIItemCell[cw][ch]->SetVisible( state );
            mGUIItem[cw][ch]->SetVisible( state );
			mGUIItemCountText[cw][ch]->SetVisible( state );
        }
    }
}

bool Inventory::IsMouseInside( int x, int y, int w, int h ) {
    return ruInput::GetMouseX() > x && ruInput::GetMouseX() < ( x + w ) && ruInput::GetMouseY() > y && ruInput::GetMouseY() < ( y + h );
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
    int screenCenterX = ruVirtualScreenWidth / 2;
    int screenCenterY = ruVirtualScreenHeight / 2;

    if( mpItemForUse ) {
        ruEngine::HideCursor();
        SetVisible( false );
        mGUIRectItemForUse->SetTexture( mpItemForUse->GetPictogram() );
        mGUIRectItemForUse->SetVisible( true );

        if( ruInput::IsMouseHit( ruInput::MouseButton::Left )) {
            mpItemForUse = nullptr;
            mpSelectedItem = nullptr;
            SetVisible( true );
        }
        if( mOpen ) {
            mpItemForUse = nullptr;
        }
        return;
    } else {
        mGUIRectItemForUse->SetVisible( false );
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

    mGUIButtonUse->SetAlpha( useAlpha );
    mGUIButtonUse->GetText()->SetAlpha( useAlpha );
    int combineAlpha = canCombine ? 255 : 60;
    mGUIButtonCombine->SetAlpha( combineAlpha );
    mGUIButtonCombine->GetText()->SetAlpha( combineAlpha );

    // draw combine items
    int combineBoxY = buttonY + 3.6f * buttonH;
    ruVector3 combineColor1 = pGUIProp->mForeColor;
    ruVector3 combineColor2 = pGUIProp->mForeColor;
    int combineBoxSpacing = 5;
    buttonsX += mCellWidth / 2 - 2 * combineBoxSpacing;

	mGUIFirstCombineItem->SetTexture( nullptr );
	mGUISecondCombineItem->SetTexture( nullptr );

    if( mpCombineItemFirst ) {				
        mGUIFirstCombineItem->SetVisible( true );
		mGUIFirstCombineItem->SetTexture( mpCombineItemFirst->GetPictogram() );
        if( IsMouseInside( buttonsX, combineBoxY, mCellWidth, mCellHeight )) {
            combineColor1 = ruVector3( 255, 0, 0 );
            if( ruInput::IsMouseHit( ruInput::MouseButton::Left )) {
                mpCombineItemFirst = 0;
            }
        }
    } else {
        mGUIFirstCombineItem->SetVisible( false );
    };

    if( mpCombineItemSecond ) {
        mGUISecondCombineItem->SetVisible( true );
		mGUISecondCombineItem->SetTexture( mpCombineItemSecond->GetPictogram() );
        if( IsMouseInside( buttonsX, combineBoxY + 1.2f * mCellHeight, mCellWidth, mCellHeight )) {
            combineColor2 = ruVector3( 255, 0, 0 );
            if( ruInput::IsMouseHit( ruInput::MouseButton::Left )) {
                mpCombineItemSecond = 0;
            }
        }
    } else {
        mGUISecondCombineItem->SetVisible( false );
    }

    mGUISecondCombineItemCell->SetColor( combineColor2 );
    mGUIFirstCombineItemCell->SetColor( combineColor1 );

    // do combine
    if( mGUIButtonCombine->IsHit() ) {
        if( canCombine ) {
            DoCombine();
        }
    }

    // use item
    if( mGUIButtonUse->IsHit() ) {
        if( mpSelectedItem ) {
            mpItemForUse = mpSelectedItem;
            mOpen = false;
        }
    }

	mGUIItemDescription->SetVisible( false );
	mGUIItemContentType->SetVisible( false );
	mGUIItemContent->SetVisible( false );
	mGUIItemVolume->SetVisible( false );
	mGUIItemMass->SetVisible( false );

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
				mGUIItemCountText[cw][ch]->SetText( StringBuilder() << curItemCount );
				if( pItem != mpCombineItemFirst && pItem != mpCombineItemSecond ) {
					mGUIItemCountText[cw][ch]->SetVisible( true );
				} else {
					mGUIItemCountText[cw][ch]->SetVisible( false );
				}
			} else {
				mGUIItemCountText[cw][ch]->SetVisible( false );
			}

            bool pressed = false;
            int cellX = coordX + distMult * mCellWidth * cw;
            int cellY = coordY + distMult * mCellHeight * ch;

            if( mGUIItemCell[cw][ch]->IsMouseInside() ) {
                color = ruVector3( 255, 0, 0 );
                alpha = 255;
                if( pItem != mpCombineItemFirst && pItem != mpCombineItemSecond ) {
                    pPicked = pItem;
                    if( ruInput::IsMouseHit( ruInput::MouseButton::Left )) {
                        pressed = true;
                        mpSelectedItem = pItem;
                    }
                }
            }
            mGUIItemCell[cw][ch]->SetColor( color );
            if( pPicked ) {
                if( ruInput::IsMouseHit( ruInput::MouseButton::Right ) && combinePick ) {
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
					mGUIItem[cw][ch]->SetVisible( true );
                    mGUIItem[cw][ch]->SetTexture( pItem->GetPictogram());
                    if( pItem == pPicked ) {
						mGUIItemDescription->SetVisible( true );
						mGUIItemContentType->SetVisible( true );
						mGUIItemContent->SetVisible( true );
						mGUIItemVolume->SetVisible( true );
						mGUIItemMass->SetVisible( true );
                        mGUIItemDescription->SetText( pItem->GetDescription());
                        mGUIItemContentType->SetText( StringBuilder() << mLocalization.GetString( "contentType" ) << ": " << pItem->GetContentType() );
                        mGUIItemContent->SetText( StringBuilder() << mLocalization.GetString( "content" ) << ": " << pItem->GetContent() );
                        mGUIItemVolume->SetText( StringBuilder() << mLocalization.GetString( "volume" ) << ": " << pItem->GetVolume() );
                        mGUIItemMass->SetText( StringBuilder() << mLocalization.GetString( "mass" ) << ": " << pItem->GetMass() );
                    }
                } else {
					mGUIItem[cw][ch]->SetVisible( false );
				}
			} else {
				mGUIItem[cw][ch]->SetVisible( false );
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

