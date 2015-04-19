#include "Item.h"
#include "Player.h"

vector<Item*> Item::msItemList;
Parser Item::msLocalization;

Item * Item::GetItemPointerByNode( ruNodeHandle obj ) {
    for( auto pItem : msItemList )
        if( pItem->mObject == obj ) {
            return pItem;
        }
    return nullptr;
}

void Item::SetType( Type type ) {
    mType = type;
    mThrowable = true;
    mContent = 1.0f;
    mMass = 1.0f;
    mVolume = 1.0f;
    if( mType == Type::Detonator ) {
        mDesc = msLocalization.GetString( "detonatorDesc" );
        mName = msLocalization.GetString( "detonatorName" );
        mPic = ruGetTexture( "data/gui/inventory/items/detonator.png" );
		mModelFile = "data/models/detonator/detonator.scene";
        mCombinePair = Type::Unknown;
        mMorphType = Type::Unknown;
        mContentTypeDesc = msLocalization.GetString( "detonatorContentType" );
    }
    if( mType == Type::FuelCanister ) {
        mDesc = msLocalization.GetString( "fuelDesc" );
        mName = msLocalization.GetString( "fuelName" );
        mPic = ruGetTexture( "data/gui/inventory/items/fuel.png" );
        mCombinePair = Type::Lighter;
        mMorphType = Type::Lighter;
		mModelFile = "data/models/fuel/fuel.scene";
        mVolume = mContent;
        mContentTypeDesc = msLocalization.GetString( "fuelContentType" );
    }
    if( mType == Type::Wires ) {
        mDesc = msLocalization.GetString( "wiresDesc" );
        mName = msLocalization.GetString( "wiresName" );
        mPic = ruGetTexture( "data/gui/inventory/items/wires.png" );
		mModelFile = "data/models/wires/wires.scene";
        mCombinePair = Type::Unknown;
        mMorphType = Type::Unknown;
        mContentTypeDesc = msLocalization.GetString( "wiresContentType" );
    }

    if( mType == Type::Explosives ) {
        mDesc = msLocalization.GetString( "explosivesDesc" );
        mName = msLocalization.GetString( "explosivesName" );
        mPic = ruGetTexture( "data/gui/inventory/items/ammonit.png" );
		mModelFile = "data/models/explosives/explosives.scene";
        mCombinePair = Type::Unknown;
        mMass = 0.3f;
        mContentTypeDesc = msLocalization.GetString( "explosivesContentType" );
    }

	if( mType == Type::Crowbar ) {
		mDesc = msLocalization.GetString( "crowbarDesc" );
		mName = msLocalization.GetString( "crowbarName" );
		mPic = ruGetTexture( "data/gui/inventory/items/crowbarpic.png" );
		mModelFile = "data/models/crowbar/crowbar.scene";
		mCombinePair = Type::Unknown;
		mMass = 4.0f;
		mContentTypeDesc = msLocalization.GetString( "crowbarContentType" );
	}

	if( mType == Type::Pistol ) {
		mDesc = msLocalization.GetString( "pistolDesc" );
		mName = msLocalization.GetString( "pistolName" );
		mPic = ruGetTexture( "data/gui/inventory/items/pistol.png" );
		mModelFile = "data/models/pistol/pistol.scene";
		mCombinePair = Type::Bullet;
		mThrowable = false;
		mMass = 4.0f;
	}

	if( mType == Type::Bullet ) {
		mDesc = msLocalization.GetString( "bulletDesc" );
		mName = msLocalization.GetString( "bulletName" );
		mPic = ruGetTexture( "data/gui/inventory/items/bullet.png" );
		mModelFile = "data/models/bullet/bullet.scene";
		mCombinePair = Type::Pistol;
		mThrowable = false;
		mMass = 0.12f;
	}

    if( mType == Type::Lighter ) {
        mDesc = msLocalization.GetString( "flashlightDesc" );
        mName = msLocalization.GetString( "flashlightName" );
        mPic = ruGetTexture( "data/gui/inventory/items/lighter.png" );
		mModelFile = "data/models/lighter/lighter.scene";
        mCombinePair = Type::FuelCanister;
        mMorphType = Type::Lighter;
        mThrowable = false;
        mMass = 1.3f;
        mVolume = 0.6;
        mContentTypeDesc = msLocalization.GetString( "flashlightContentType" );
    }
    if( mType == Type::Fuse ) {
        mDesc = msLocalization.GetString( "fuseDesc" );
        mName = msLocalization.GetString( "fuseName" );
        mPic = ruGetTexture( "data/gui/inventory/items/fuse.png" );
		mModelFile = "data/models/fuse/fuse.scene";
        mCombinePair = Type::Unknown;
        mMass = 5.0f;
        mContentTypeDesc = msLocalization.GetString( "fuseContentType" );
    }
    if( mType == Type::Medkit ) {
        mDesc = msLocalization.GetString( "medkitDesc" );
        mName = msLocalization.GetString( "medkitName" );
        mPic = ruGetTexture( "data/gui/inventory/items/medkit.png" );
        mCombinePair = Type::Unknown;
        mMass = 1.5f;
        mContentTypeDesc = msLocalization.GetString( "medkitContentType" );
    }

}

void Item::PickUp() {
	switch ( mType ) {
	case Type::Pistol:
		pPlayer->AddWeapon( Weapon::Type::Pistol );
		break;
	default:
		break;
	}
}

Item::Item( ruNodeHandle obj, Type type ): InteractiveObject( obj ) {
    if( !msLocalization.IsParsed() ) {
        msLocalization.ParseFile( localizationPath + "items.loc" );
    }
    mMorphType = Type::Unknown;
    mCombinePair = Type::Unknown;
    msItemList.push_back( this );
    mInInventory = false;
    SetType( type );
}

Item::~Item() {

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

bool Item::IsThrowable() const {
    return mThrowable;
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
            }

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
				if( pPlayer->mCurrentWeapon ) {
					if( pPlayer->mCurrentWeapon->GetType() == Weapon::Type::Pistol ) {
						bool bulletLoaded = pPlayer->mCurrentWeapon->LoadBullet();
						if( bulletLoaded ) {
							pPlayer->GetInventory()->RemoveItem( pBullet );
							delete pBullet;
							return true;
						}
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

void Item::Repair() {
	// when level is changed, handle to object becomes invalid
	// so we need to reload item model from it source file
	if( !ruIsNodeHandleValid( mObject )) {
		if( mModelFile.empty() ) {
			RaiseError( "Item::mModelFile is empty! Model source must be defined!" );
		}
		mObject = ruLoadScene( mModelFile );
		ruFreeze( mObject );
	}
}
