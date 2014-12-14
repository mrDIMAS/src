#include "Item.h"
#include "Player.h"

vector<Item*> Item::Available;
Parser Item::msLoc;

Item * Item::GetByObject( ruNodeHandle obj )
{
    for( auto pItem : Available )
        if( pItem->mObject == obj )
            return pItem;
    return 0;
}

void Item::SetType( Type type )
{
    mType = type;
    mThrowable = true;
	mContent = 1.0f;
	mMass = 1.0f;
	mVolume = 1.0f;
    if( mType == Type::Detonator )
    {
        mDesc = msLoc.GetString( "detonatorDesc" );
        mName = msLoc.GetString( "detonatorName" );
        mPic = ruGetTexture( "data/gui/inventory/items/detonator.png" );;
        mCombinePair = Type::Unknown;
        mMorphType = Type::Unknown;
        mContentTypeDesc = msLoc.GetString( "detonatorContentType" );
    }
    if( mType == Type::FuelCanister )
    {
        mDesc = msLoc.GetString( "fuelDesc" );
        mName = msLoc.GetString( "fuelName" );
        mPic = ruGetTexture( "data/gui/inventory/items/fuel.png" );
        mCombinePair = Type::Flashlight;
        mMorphType = Type::Flashlight;
        mVolume = mContent;
        mContentTypeDesc = msLoc.GetString( "fuelContentType" );
    }
    if( mType == Type::Wires )
    {
        mDesc = msLoc.GetString( "wiresDesc" );
        mName = msLoc.GetString( "wiresName" );
        mPic = ruGetTexture( "data/gui/inventory/items/wires.png" );
        mCombinePair = Type::Unknown;
        mMorphType = Type::Unknown;
        mContentTypeDesc = msLoc.GetString( "wiresContentType" );
    }

    if( mType == Type::Explosives )
    {
        mDesc = msLoc.GetString( "explosivesDesc" );
        mName = msLoc.GetString( "explosivesName" );
        mPic = ruGetTexture( "data/gui/inventory/items/ammonit.png" );
        mCombinePair = Type::Unknown;
        mMass = 0.3f;
        mContentTypeDesc = msLoc.GetString( "explosivesContentType" );
    }
    if( mType == Type::Flashlight )
    {
        mDesc = msLoc.GetString( "flashlightDesc" );
        mName = msLoc.GetString( "flashlightName" );
        mPic = ruGetTexture( "data/gui/inventory/items/flashlight.png" );
        mCombinePair = Type::FuelCanister;
        mMorphType = Type::Flashlight;
        mThrowable = false;
        mMass = 1.3f;
        mVolume = 0.6;
        mContentTypeDesc = msLoc.GetString( "flashlightContentType" );
    }
    if( mType == Type::Fuse )
    {
        mDesc = msLoc.GetString( "fuseDesc" );
        mName = msLoc.GetString( "fuseName" );
        mPic = ruGetTexture( "data/gui/inventory/items/fuse.png" );
        mCombinePair = Type::Unknown;
        mMass = 5.0f;
        mContentTypeDesc = msLoc.GetString( "fuseContentType" );
    }
    if( mType == Type::Medkit )
    {
        mDesc = msLoc.GetString( "medkitDesc" );
        mName = msLoc.GetString( "medkitName" );
        mPic = ruGetTexture( "data/gui/inventory/items/medkit.png" );
        mCombinePair = Type::Unknown;
        mMass = 1.5f;
        mContentTypeDesc = msLoc.GetString( "medkitContentType" );
    }
}

Item::Item( ruNodeHandle obj, Type type ): InteractiveObject( obj )
{
    if( !msLoc.IsParsed() )
        msLoc.ParseFile( localizationPath + "items.loc" );
    mMorphType = Type::Unknown;
    mCombinePair = Type::Unknown;
    Available.push_back( this );
    mInInventory = false;
    SetType( type );
}

Item::~Item()
{

}

const char * Item::GetName() const
{
	return mName.c_str();
}

const char * Item::GetDescription() const
{
	return mDesc.c_str();
}

void Item::SetContent( float content )
{
	mContent = content;
}

float Item::GetContent() const
{
	return mContent;
}

ruTextureHandle Item::GetPictogram() const
{
	return mPic;
}

bool Item::IsThrowable() const
{
	return mThrowable;
}

Item::Type Item::GetCombineType() const
{
	return mCombinePair;
}

Item::Type Item::GetType() const
{
	return mType;
}

bool Item::Combine( Item * pItem, Item* & pUsedItem )
{
	if( pItem->GetCombineType() == mType  )
	{		
		if( mCombinePair == pItem->GetType() )
		{
			// player flashlight charge
			Item * pCanister = nullptr;
			Item * pFlashlight = nullptr;
			if( mType == Type::FuelCanister && pItem->GetType() == Type::Flashlight ) {
				pCanister = this;
				pFlashlight = pItem;
			} else if( mType == Type::Flashlight && pItem->GetType() == Type::FuelCanister ) {
				pCanister = pItem;
				pFlashlight = this;
			}
			if( pCanister && pFlashlight ) {
				if( pCanister->GetContent() > 0.0f )
				{
					pFlashlight->SetContent( pCanister->GetContent() );
					pPlayer->ChargeFlashLight( pCanister );
				}
				pUsedItem = pCanister;
				return true;
			}
		}
	}
	return false;
}

float Item::GetMass() const
{
	return mMass;
}

void Item::SetMass( float mass )
{
	mMass = mass;
}

void Item::SetVolume( float val )
{
	mVolume = val;
}

float Item::GetVolume() const
{
	return mVolume;
}

void Item::SetContentType( const char * contentType )
{
	mContentTypeDesc = contentType;
}

const char * Item::GetContentType() const
{
	return mContentTypeDesc.c_str();
}

void Item::MarkAsFree()
{
	mInInventory = false;
}

void Item::MarkAsGrabbed()
{
	mInInventory = true;
}

bool Item::IsFree()
{
	return mInInventory == false;
}
