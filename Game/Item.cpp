#include "Precompiled.h"

#include "Item.h"
#include "Player.h"
#include "Weapon.h"

vector<Item*> Item::msItemList;
Parser Item::msLocalization;

Item::Item( Type type ) {
	if( !msLocalization.IsParsed() ) {
		msLocalization.ParseFile( localizationPath + "items.loc" );
	}
	mMorphType = Type::Unknown;
	mCombinePair = Type::Unknown;
	msItemList.push_back( this );
	mInInventory = false;
	mCanBeDeleted = false;

	mType = type;
	mSingleInstance = false;
	mContent = 1.0f;
	mMass = 1.0f;
	mVolume = 1.0f;
	if( mType == Type::Detonator ) {
		mDesc = msLocalization.GetString( "detonatorDesc" );
		mName = GetNameByType( mType );
		mPic = ruGetTexture( "data/gui/inventory/items/detonator.png" );
		mCombinePair = Type::Unknown;
		mMorphType = Type::Unknown;
		mContentTypeDesc = msLocalization.GetString( "detonatorContentType" );
	} else if( mType == Type::FuelCanister ) {
		mDesc = msLocalization.GetString( "fuelDesc" );
		mName = GetNameByType( mType );
		mPic = ruGetTexture( "data/gui/inventory/items/fuel.png" );
		mCombinePair = Type::Lighter;
		mMorphType = Type::Lighter;
		mVolume = mContent;
		mContentTypeDesc = msLocalization.GetString( "fuelContentType" );
	} else if( mType == Type::Wires ) {
		mDesc = msLocalization.GetString( "wiresDesc" );
		mName = GetNameByType( mType );
		mPic = ruGetTexture( "data/gui/inventory/items/wires.png" );
		mCombinePair = Type::Unknown;
		mMorphType = Type::Unknown;
		mContentTypeDesc = msLocalization.GetString( "wiresContentType" );
	} else if( mType == Type::Explosives ) {
		mDesc = msLocalization.GetString( "explosivesDesc" );
		mName = GetNameByType( mType );
		mPic = ruGetTexture( "data/gui/inventory/items/ammonit.png" );
		mCombinePair = Type::Unknown;
		mMass = 0.3f;
		mContentTypeDesc = msLocalization.GetString( "explosivesContentType" );
	} else if( mType == Type::Crowbar ) {
		mDesc = msLocalization.GetString( "crowbarDesc" );
		mName = GetNameByType( mType );
		mPic = ruGetTexture( "data/gui/inventory/items/crowbarpic.png" );
		mCombinePair = Type::Unknown;
		mMass = 4.0f;
		mContentTypeDesc = msLocalization.GetString( "crowbarContentType" );
	} else if( mType == Type::Pistol ) {
		mDesc = msLocalization.GetString( "pistolDesc" );
		mName = GetNameByType( mType );
		mPic = ruGetTexture( "data/gui/inventory/items/pistol.png" );
		mCombinePair = Type::Bullet;
		mSingleInstance = true;
		mMass = 4.0f;
	} else if( mType == Type::Bullet ) {
		mDesc = msLocalization.GetString( "bulletDesc" );
		mName = GetNameByType( mType );
		mPic = ruGetTexture( "data/gui/inventory/items/bullet.png" );
		mCombinePair = Type::Pistol;
		mMass = 0.12f;
	} else if( mType == Type::Lighter ) {
		mDesc = msLocalization.GetString( "flashlightDesc" );
		mName = GetNameByType( mType );
		mPic = ruGetTexture( "data/gui/inventory/items/lighter.png" );
		mCombinePair = Type::FuelCanister;
		mSingleInstance = true;
		mMorphType = Type::Lighter;
		mMass = 1.3f;
		mVolume = 0.6;
		mContentTypeDesc = msLocalization.GetString( "flashlightContentType" );
	} else if( mType == Type::Fuse ) {
		mDesc = msLocalization.GetString( "fuseDesc" );
		mName = GetNameByType( mType );
		mPic = ruGetTexture( "data/gui/inventory/items/fuse.png" );
		mCombinePair = Type::Unknown;
		mMass = 5.0f;
		mContentTypeDesc = msLocalization.GetString( "fuseContentType" );
	} else if( mType == Type::Syringe ) {
		mDesc = msLocalization.GetString( "syringeDesc" );
		mName = GetNameByType( mType );
		mPic = ruGetTexture( "data/gui/inventory/items/syringe.png" );
		mSingleInstance = true;
		mCombinePair = Type::Unknown;
		mMass = 1.5f;
		mContentTypeDesc = msLocalization.GetString( "syringeContentType" );
	} else if( mType == Type::Beaker ) {
		mDesc = msLocalization.GetString( "beakerDesc" );
		mName = GetNameByType( mType );
		mPic = ruGetTexture( "data/gui/inventory/items/beaker.png" );
		mCombinePair = Type::Unknown;
		mMass = 0.25f;
	} else if( mType == Type::FerrumOxide ) {
		mDesc = msLocalization.GetString( "fe2o3Desc" );
		mName = GetNameByType( mType );
		mPic = ruGetTexture( "data/gui/inventory/items/jar.png" );
		mCombinePair = Type::Unknown;
		mMass = 0.25f;
	} else if( mType == Type::AluminumPowder ) {
		mDesc = msLocalization.GetString( "aluminumDesc" );
		mName = GetNameByType( mType );
		mPic = ruGetTexture( "data/gui/inventory/items/jar.png" );
		mCombinePair = Type::Unknown;
		mMass = 0.25f;
	}
}

Item::~Item() {
	auto iter = find( msItemList.begin(), msItemList.end(), this );
	if( iter != msItemList.end() ) {
		msItemList.erase( iter );
	}
}

const string & Item::GetName() const {
    return mName;
}

const string & Item::GetDescription() const {
    return mDesc;
}

void Item::SetContent( float content ) {
    mContent = content;
}

float Item::GetContent() const {
    return mContent;
}

ruTextureHandle Item::GetPictogram() const {
    return mPic;
}

Item::Type Item::GetCombineType() const {
    return mCombinePair;
}

Item::Type Item::GetType() const {
    return mType;
}

bool Item::Combine( Item * pItem, Item* & pUsedItem ) {
    if( pItem->GetCombineType() == mType  ) {
        if( mCombinePair == pItem->GetType() ) {
            // player flashlight charge
			/*
            Item * pCanister = nullptr;
            Item * pFlashlight = nullptr;
            if( mType == Type::FuelCanister && pItem->GetType() == Type::Lighter ) {
                pCanister = this;
                pFlashlight = pItem;
            } else if( mType == Type::Lighter && pItem->GetType() == Type::FuelCanister ) {
                pCanister = pItem;
                pFlashlight = this;
            }
            if( pCanister && pFlashlight ) {
                if( pCanister->GetContent() > 0.0f ) {
                    pFlashlight->SetContent( pCanister->GetContent() );
                    pPlayer->ChargeFlashLight();
                }
                pUsedItem = pCanister;
                return true;
            }*/

			// pistol reloading
			Item * pPistol = nullptr;
			Item * pBullet = nullptr;
			if( mType == Type::Pistol && pItem->GetType() == Type::Bullet ) {
				pPistol = this;
				pBullet = pItem;
			} else {
				pPistol = pItem;
				pBullet = this;
			}
			if( pPistol && pBullet ) {				
				if( pPlayer->GetWeapon() ) {
					bool bulletLoaded = pPlayer->GetWeapon()->LoadBullet();
					if( bulletLoaded ) {
						pPlayer->GetInventory()->RemoveItem( pBullet );
						pBullet->mCanBeDeleted = true;
						return true;
					}					
				}
			}
        }
    }
    return false;
}

float Item::GetMass() const {
    return mMass;
}

void Item::SetMass( float mass ) {
    mMass = mass;
}

void Item::SetVolume( float val ) {
    mVolume = val;
}

float Item::GetVolume() const {
    return mVolume;
}

void Item::SetContentType( const string & contentType ) {
    mContentTypeDesc = contentType;
}

const string & Item::GetContentType() const {
    return mContentTypeDesc;
}

void Item::MarkAsFree() {
    mInInventory = false;
}

void Item::MarkAsGrabbed() {
    mInInventory = true;
}

bool Item::IsFree() {
    return mInInventory == false;
}
