#include "Precompiled.h"
#include "Level.h"
#include "Item.h"
#include "Player.h"

Parser Item::msLocalization;

Item::Item( Type type ) : 
	mCombinePair(Type::Unknown),
	mType(type),
	mSingleInstance(false),
	mContent(1.0f),
	mMass(1.0f),
	mVolume(1.0f)
{
	if( !msLocalization.IsParsed() ) {
		msLocalization.ParseFile( gLocalizationPath + "items.loc" );
	}

	if( mType == Type::Detonator ) {
		mDesc = msLocalization.GetString( "detonatorDesc" );
		mName = GetNameByType( mType );
		mPic = ruTexture::Request( "data/gui/inventory/items/detonator.png" );
		mCombinePair = Type::Unknown;
		mContentTypeDesc = msLocalization.GetString( "detonatorContentType" );
	} else if( mType == Type::FuelCanister ) {
		mDesc = msLocalization.GetString( "fuelDesc" );
		mName = GetNameByType( mType );
		mPic = ruTexture::Request( "data/gui/inventory/items/fuel.png" );
		mCombinePair = Type::Lighter;
		mVolume = mContent;
		mContentTypeDesc = msLocalization.GetString( "fuelContentType" );
	} else if( mType == Type::Wires ) {
		mDesc = msLocalization.GetString( "wiresDesc" );
		mName = GetNameByType( mType );
		mPic = ruTexture::Request( "data/gui/inventory/items/wires.png" );
		mCombinePair = Type::Unknown;
		mContentTypeDesc = msLocalization.GetString( "wiresContentType" );
	} else if( mType == Type::Explosives ) {
		mDesc = msLocalization.GetString( "explosivesDesc" );
		mName = GetNameByType( mType );
		mPic = ruTexture::Request( "data/gui/inventory/items/ammonit.png" );
		mCombinePair = Type::Unknown;
		mMass = 0.6f;
		mContentTypeDesc = msLocalization.GetString( "explosivesContentType" );
	} else if( mType == Type::Crowbar ) {
		mDesc = msLocalization.GetString( "crowbarDesc" );
		mName = GetNameByType( mType );
		mPic = ruTexture::Request( "data/gui/inventory/items/crowbarpic.png" );
		mCombinePair = Type::Unknown;
		mMass = 6.0f;
		mContentTypeDesc = msLocalization.GetString( "crowbarContentType" );
	} else if( mType == Type::Lighter ) {
		mDesc = msLocalization.GetString( "flashlightDesc" );
		mName = GetNameByType( mType );
		mPic = ruTexture::Request( "data/gui/inventory/items/lighter.png" );
		mCombinePair = Type::FuelCanister;
		mSingleInstance = true;
		mMass = 0.4f;
		mVolume = 0.6;
		mContentTypeDesc = msLocalization.GetString( "flashlightContentType" );
	} else if( mType == Type::Fuse ) {
		mDesc = msLocalization.GetString( "fuseDesc" );
		mName = GetNameByType( mType );
		mPic = ruTexture::Request( "data/gui/inventory/items/fuse.png" );
		mCombinePair = Type::Unknown;
		mMass = 5.0f;
		mContentTypeDesc = msLocalization.GetString( "fuseContentType" );
	} else if( mType == Type::Beaker ) {
		mDesc = msLocalization.GetString( "beakerDesc" );
		mName = GetNameByType( mType );
		mPic = ruTexture::Request( "data/gui/inventory/items/beaker.png" );
		mCombinePair = Type::Unknown;
		mMass = 0.25f;
	} else if( mType == Type::FerrumOxide ) {
		mDesc = msLocalization.GetString( "fe2o3Desc" );
		mName = GetNameByType( mType );
		mPic = ruTexture::Request( "data/gui/inventory/items/jar.png" );
		mCombinePair = Type::Unknown;
		mMass = 0.25f;
	} else if( mType == Type::AluminumPowder ) {
		mDesc = msLocalization.GetString( "aluminumDesc" );
		mName = GetNameByType( mType );
		mPic = ruTexture::Request( "data/gui/inventory/items/jar.png" );
		mCombinePair = Type::Unknown;
		mMass = 0.25f;
	}
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

shared_ptr<ruTexture> Item::GetPictogram() const {
    return mPic;
}

Item::Type Item::GetCombineType() const {
    return mCombinePair;
}

Item::Type Item::GetType() const {
    return mType;
}

bool Item::Combine( Item::Type combinerType ) {
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

std::string Item::GetNameByType( Type type ) {
	if( !msLocalization.IsParsed() ) {
		msLocalization.ParseFile( gLocalizationPath + "items.loc" );
	}
	switch (type) {
	case Type::Detonator:
		return msLocalization.GetString("detonatorName");
	case Type::FuelCanister:
		return msLocalization.GetString("fuelName");
	case Type::Wires:
		return msLocalization.GetString("wiresName");
	case Type::Explosives:
		return msLocalization.GetString("explosivesName");
	case Type::Crowbar:
		return msLocalization.GetString("crowbarName");
	case Type::Bullet:
		return msLocalization.GetString("bulletName");
	case Type::Lighter:
		return msLocalization.GetString("flashlightName");
	case Type::Fuse:
		return msLocalization.GetString("fuseName");
	case Type::Beaker:
		return msLocalization.GetString("beakerName");
	case Type::FerrumOxide:
		return msLocalization.GetString("fe2o3Name");
	case Type::AluminumPowder:
		return msLocalization.GetString("aluminumName");
	default:
		return "(name is not set)";
	}	
}
