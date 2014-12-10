#include "Item.h"

vector<Item*> Item::Available;
Parser Item::msLoc;

Item * Item::GetByObject( ruNodeHandle obj )
{
    for( auto pItem : Available )
        if( pItem->mObject == obj )
            return pItem;
    return 0;
}

void Item::SetType( int typ )
{
    mType = typ;

    mThrowable = true;

    if( mType == Detonator )
    {
        mDesc = msLoc.GetString( "detonatorDesc" );
        mName = msLoc.GetString( "detonatorName" );
        mPic = ruGetTexture( "data/gui/inventory/items/detonator.png" );;
        mCombineType = 0;
        mOnCombineBecomes = 0;

        mContent = msLoc.GetString( "detonatorContent" );
        mContentType = msLoc.GetString( "detonatorContentType" );
    }

    if( mType == FuelCanister )
    {
        mDesc = msLoc.GetString( "fuelDesc" );
        mName = msLoc.GetString( "fuelName" );
        mPic = ruGetTexture( "data/gui/inventory/items/fuel.png" );
        mCombineType = Flashlight;
        mOnCombineBecomes = Flashlight;

        mContent = msLoc.GetString( "fuelContent" );
        mVolume = mContent;

        mContentType = msLoc.GetString( "fuelContentType" );
    }


    if( mType == Wires )
    {
        mDesc = msLoc.GetString( "wiresDesc" );
        mName = msLoc.GetString( "wiresName" );
        mPic = ruGetTexture( "data/gui/inventory/items/wires.png" );
        mCombineType = 0;
        mOnCombineBecomes = 0;

        mContent = msLoc.GetString( "wiresContent" );
        mContentType = msLoc.GetString( "wiresContentType" );
    }

    if( mType == Explosives )
    {
        mDesc = msLoc.GetString( "explosivesDesc" );
        mName = msLoc.GetString( "explosivesName" );
        mPic = ruGetTexture( "data/gui/inventory/items/ammonit.png" );
        mCombineType = 0;

        mMass = "0.3";
        mContentType = msLoc.GetString( "explosivesContentType" );
    }

    if( mType == Flashlight )
    {
        mDesc = msLoc.GetString( "flashlightDesc" );
        mName = msLoc.GetString( "flashlightName" );
        mPic = ruGetTexture( "data/gui/inventory/items/flashlight.png" );
        mCombineType = FuelCanister;
        mOnCombineBecomes = Flashlight;

        mThrowable = false;
        mMass = "1.3";
        mVolume = "0.6";
        mContentType = msLoc.GetString( "flashlightContentType" );
    }

    if( mType == Fuse )
    {
        mDesc = msLoc.GetString( "fuseDesc" );
        mName = msLoc.GetString( "fuseName" );
        mPic = ruGetTexture( "data/gui/inventory/items/fuse.png" );
        mCombineType = 0;

        mMass = "6.3";
        mContentType = msLoc.GetString( "fuseContentType" );
    }

    if( mType == Medkit )
    {
        mDesc = msLoc.GetString( "medkitDesc" );
        mName = msLoc.GetString( "medkitName" );
        mPic = ruGetTexture( "data/gui/inventory/items/medkit.png" );
        mCombineType = 0;

        mMass = "1.3";
        mContentType = msLoc.GetString( "medkitContentType" );
    }
}

Item::Item( ruNodeHandle obj, int typ ): InteractiveObject( obj )
{
    if( !msLoc.IsParsed() )
        msLoc.ParseFile( localizationPath + "items.loc" );
    mOnCombineBecomes = 0;
    mCombineType = 0;
    Available.push_back( this );
    mVolume = Property( string( msLoc.GetString( "volume" )) + "\n", "-" );
    mMass = Property( string( msLoc.GetString( "mass" )) + "\n", "-" );
    mContent = Property( string( msLoc.GetString( "content" )) + "\n", "-" );
    mContentType = Property( string( msLoc.GetString( "contentType" ))+ "\n", "-" );
    mInInventory = false;
    SetType( typ );
}


Item::Property::Property( string description, float theValue )
{
    mDesc = description;
    SetFloatValue( theValue );
}

Item::Property::Property( const Property & p )
{
    mFloatValue = p.mFloatValue;
    mStringValue = p.mStringValue;
    mDesc = p.mDesc;
    mFormatted = p.mFormatted;
}

Item::Property::Property()
{
    mFloatValue = 0;

    Format();
}

Item::Property::Property( string description, string theValue )
{
    mDesc = description;
    SetstringValue( theValue );
}

void Item::Property::SetstringValue( string s )
{
    mStringValue = s;
    mFloatValue = atof( s.c_str() );

    Format();
}

void Item::Property::SetFloatValue( float f )
{
    char buffer[ 64 ];
    sprintf( buffer, "%.1f", f );
    mStringValue = buffer;
    mFloatValue = f;

    Format();
}

void Item::Property::Format()
{
    mFormatted = mDesc + mStringValue;
}
