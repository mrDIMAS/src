#include "Inventory.h"
#include "GUI.h"
#include "Player.h"

Inventory::Inventory() {
    localization.ParseFile( localizationPath + "inventory.loc" );

    backgroundTexture = ruGetTexture( "data/gui/inventory/background.png");
    cellTexture = ruGetTexture( "data/gui/inventory/cell.png" );
    buttonTexture = ruGetTexture( "data/gui/inventory/button.png" );

    opened = 0;
    selected = 0;

    cellWidth = 64;
    cellHeight = 64;
    cellCountWidth = 5;
    cellCountHeight = 4;

    font = ruCreateGUIFont( 14, "data/fonts/font1.otf", 0, 0 );

    combineItemFirst = 0;
    combineItemSecond = 0;

    forUse = 0;

    pickSound = ruLoadSound2D( "data/sounds/menupick.ogg" );
}

bool Inventory::IsMouseInside( int x, int y, int w, int h ) {
    return ruGetMouseX() > x && ruGetMouseX() < ( x + w ) && ruGetMouseY() > y && ruGetMouseY() < ( y + h );
}

void Inventory::DoCombine() {
    int type1 = combineItemFirst->type;
    int type2 = combineItemSecond->type;

    if( combineItemSecond->combinesWith == combineItemFirst->type  ) {
        if( combineItemFirst->combinesWith == combineItemSecond->type ) {
            if( ( type1 == Item::FuelCanister && type2 == Item::Flashlight ) || ( type1 == Item::Flashlight && type2 == Item::FuelCanister ) ) {
                Item * canister   = ( type1 == Item::FuelCanister ? combineItemFirst : combineItemSecond );
                Item * flashlight = ( type2 == Item::Flashlight ? combineItemSecond : combineItemFirst );

                if( canister->content > 0 ) {
                    flashlight->content = 1.0f;

                    player->ChargeFlashLight( canister );
                }

                ThrowItem( canister );
            } else {
                combineItemFirst->SetType( combineItemSecond->onCombineBecomes );

                items.erase( find( items.begin(), items.end(), combineItemSecond ));
            }

            combineItemFirst = 0;
            combineItemSecond = 0;

            selected = 0;
        }
    }
}

void Inventory::Update() {
    ruVector3 whiteColor = ruVector3( 255, 255, 255 );
    int screenCenterX = ruGetResolutionWidth() / 2;
    int screenCenterY = ruGetResolutionHeight() / 2;

    if( forUse ) {
        ruHideCursor();
        ruDrawGUIRect( screenCenterX, screenCenterY, 64, 64, forUse->img, whiteColor, 255 );
        if( ruIsMouseHit( MB_Left )) {
            forUse = 0;
            selected = 0;
        }
        if( opened ) {
            forUse = 0;
        }
        return;
    }

    if( !opened ) {
        ruHideCursor();
        return;
    }

    ruShowCursor();

    float distMult = 1.1f;
    int cellSpaceX = distMult * cellWidth / (float)cellCountWidth;
    int cellSpaceY = distMult * cellHeight / (float)cellCountHeight;
    int coordX = screenCenterX - 0.5f * (float)cellCountWidth * (float)cellWidth - cellSpaceX;
    int coordY = screenCenterY - 0.5f * (float)cellCountHeight * (float)cellHeight - cellSpaceY;
    // draw background
    int backGroundSpace = 20;
    int backgroundX = coordX - backGroundSpace;
    int backgroundY = coordY - backGroundSpace;
    int backgroundW = cellCountWidth * cellWidth + 2.5 * backGroundSpace + cellSpaceX;
    int backgroundH = cellCountHeight * cellHeight + 2.5 * backGroundSpace + cellSpaceY;
    ruDrawGUIRect( backgroundX, backgroundY, backgroundW, backgroundH, backgroundTexture );
    // draw item description
    int combineH = 128;
    int combineY = backgroundY + backgroundH;
    int descriptionY = combineY + 10;
    ruDrawGUIRect( backgroundX, combineY, backgroundW, combineH, backgroundTexture );
    ruDrawGUIText( localization.GetString( "desc" ), backgroundX + 10, descriptionY, backgroundW, combineH, font, whiteColor, 0 );
    // draw item actions
    int actionsW = 128;
    int actionsX = backgroundX + backgroundW;
    ruDrawGUIRect( actionsX, backgroundY, actionsW, backgroundH + combineH, backgroundTexture );
    // draw actions buttons
    int buttonSpace = 10;
    int buttonsX = actionsX + buttonSpace;
    int buttonY = backgroundY + 2 * buttonSpace;
    int buttonH = 30;
    int buttonW = actionsW - 2 * buttonSpace;
    bool canCombine = ( combineItemFirst != 0 && combineItemSecond != 0 );
    int useAlpha = selected ? 255 : 60;
    ruGUIState buttonUse = ruDrawGUIButton( buttonsX, buttonY, buttonW, buttonH, buttonTexture, localization.GetString( "use" ), font, whiteColor, 1, useAlpha );
    ruGUIState buttonThrow = ruDrawGUIButton( buttonsX, buttonY + 1.2f * buttonH, buttonW, buttonH, buttonTexture, localization.GetString( "throw" ), font, whiteColor, 1, useAlpha );
    int combineAlpha = canCombine ? 255 : 60;
    ruGUIState buttonCombine = ruDrawGUIButton( buttonsX, buttonY + 2.4f * buttonH, buttonW, buttonH, buttonTexture, localization.GetString( "combine" ), font, whiteColor, 1, combineAlpha );
    // draw combine items
    int combineBoxY = buttonY + 3.6f * buttonH;
    ruVector3 combineColor1 = whiteColor;
    ruVector3 combineColor2 = whiteColor;
    int combineBoxSpacing = 5;
    buttonsX += cellWidth / 2 - 2 * combineBoxSpacing;
    if( combineItemFirst ) {
        ruDrawGUIRect( buttonsX + combineBoxSpacing, combineBoxY + combineBoxSpacing, cellWidth - 2 * combineBoxSpacing, cellHeight - 2 * combineBoxSpacing, combineItemFirst->img );
        if( IsMouseInside( buttonsX, combineBoxY, cellWidth, cellHeight )) {
            combineColor1 = ruVector3( 255, 0, 0 );
            if( ruIsMouseHit( MB_Left )) {
                combineItemFirst = 0;
            }
        }
    }
    if( combineItemSecond ) {
        ruDrawGUIRect( buttonsX + combineBoxSpacing, combineBoxY + 1.2f * cellHeight + combineBoxSpacing, cellWidth - 2 * combineBoxSpacing, cellHeight - 2 * combineBoxSpacing, combineItemSecond->img );
        if( IsMouseInside( buttonsX, combineBoxY + 1.2f * cellHeight, cellWidth, cellHeight )) {
            combineColor2 = ruVector3( 255, 0, 0 );
            if( ruIsMouseHit( MB_Left )) {
                combineItemSecond = 0;
            }
        }
    }
    ruDrawGUIRect( buttonsX, combineBoxY, cellWidth, cellHeight, cellTexture, combineColor1, 255 );
    ruDrawGUIRect( buttonsX, combineBoxY + 1.2f * cellHeight, cellWidth, cellHeight, cellTexture, combineColor2, 255 );
    // do combine
    if( buttonCombine.mouseLeftClicked ) {
        if( canCombine ) {
            DoCombine();
        }
    }
    // use item
    if( buttonUse.mouseLeftClicked ) {
        if( selected ) {
            forUse = selected;
            opened = false;
        }
    }
    // throw item
    if( buttonThrow.mouseLeftClicked ) {
        if( selected ) {
            if( selected->throwable ) {
                ThrowItem( selected );
            }
        }
    }
    // characteristics
    ruDrawGUIText( localization.GetString( "characteristics" ), actionsX + buttonSpace, combineBoxY + 2.2f * cellHeight, backgroundW, combineH, font, whiteColor, 0 );
    // draw cells and item image
    bool combinePick = true;
    for( int cw = 0; cw < cellCountWidth; cw++ ) {
        for( int ch = 0; ch < cellCountHeight; ch++ ) {
            Item * picked = 0;
            ruVector3 color = whiteColor;
            int alpha = 180;
            // get item for draw
            int itemNum = cw * cellCountHeight + ch;
            Item * item = nullptr;
            if( itemNum < items.size() ) {
                item = items[ itemNum ];
            }
            if( item )
                if( selected == item ) {
                    color = ruVector3( 0, 200, 0 );
                    alpha = 255;
                }
            bool pressed = false;
            int cellX = coordX + distMult * cellWidth * cw;
            int cellY = coordY + distMult * cellHeight * ch;
            if( IsMouseInside( cellX, cellY, cellWidth, cellHeight ) ) {
                color = ruVector3( 255, 0, 0 );
                alpha = 255;
                if( item != combineItemFirst && item != combineItemSecond ) {
                    picked = item;
                    if( ruIsMouseHit( MB_Left )) {
                        pressed = true;
                        selected = item;
                    }
                }
            }
            ruDrawGUIRect( cellX, cellY, cellWidth, cellHeight, cellTexture, color, alpha );
            if( picked ) {
                if( ruIsMouseHit( MB_Right ) && combinePick ) {
                    if( combineItemFirst == 0 ) {
                        if( picked != combineItemFirst ) {
                            combineItemFirst = picked;
                        }
                    } else if( combineItemSecond == 0 ) {
                        if( picked != combineItemSecond ) {
                            combineItemSecond = picked;
                        }
                    }

                    combinePick = false;
                    selected = 0;
                }
            }

            if( item ) {
                if( item != combineItemFirst && item != combineItemSecond ) {
                    int itemSpacing = 5;
                    ruDrawGUIRect( cellX + itemSpacing, cellY + itemSpacing, cellWidth - 2 * itemSpacing, cellHeight - 2 * itemSpacing, item->img, ruVector3( 255, 255, 255 ), alpha );

                    if( item == picked ) {
                        int offset = combineBoxY + 2.2f * cellHeight;
                        ruDrawGUIText( item->desc.c_str(), backgroundX + 2 * itemSpacing, descriptionY + 2.5 * itemSpacing, backgroundW - 2 * itemSpacing , combineH - 2 * itemSpacing, gui->font, ruVector3( 200, 200, 200 ), 0, alpha );
                        // draw characteristics of item
                        int charSpace = 28;
                        ruDrawGUIText( item->contentType, actionsX + buttonSpace, offset + charSpace * 1, actionsW - itemSpacing, 100, font, whiteColor, 0 );
                        ruDrawGUIText( item->content, actionsX + buttonSpace, offset + charSpace * 2.1, actionsW - itemSpacing, 100, font, whiteColor, 0 );
                        ruDrawGUIText( item->volume, actionsX + buttonSpace, offset + charSpace * 3.1, actionsW - itemSpacing, 100, font, whiteColor, 0 );
                        ruDrawGUIText( item->mass, actionsX + buttonSpace, offset + charSpace * 4.1, actionsW - itemSpacing, 100, font, whiteColor, 0 );
                    }
                }
            }
        }
    }
}

void Inventory::RemoveItem( Item * item ) {
    items.erase( find( items.begin(), items.end(), item ));
}

void Inventory::ThrowItem( Item * item ) {
    item->inInventory = false;
    ruSetNodePosition( item->object, player->GetCurrentPosition() + 2 * ruGetNodeLookVector( player->body ) + ruVector3( 0, 1, 0 ));
    ruUnfreeze( item->object );
    RemoveItem( item );
    item->content = 0.0f;
    selected = 0;
}
