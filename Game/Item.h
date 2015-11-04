#pragma  once

#include "Game.h"
#include "Parser.h"
#include "InteractiveObject.h"

class Item {
public:
    enum class Type : int {
        Unknown			= 0,
        Detonator		= 1,
        FuelCanister	= 2,
        Wires			= 3,
        Explosives		= 4,
        Lighter			= 5,
        Fuse			= 6,
        Syringe			= 7,
		Crowbar			= 8,
		Pistol			= 9,
		Bullet			= 10,
		Beaker			= 11,
		AluminumPowder  = 12,
		FerrumOxide		= 13,
    };
private:
	friend class Inventory;
    static Parser msLocalization;
    ruTextureHandle mPic;
    string mDesc;
    string mName;
    Type mType;
    Type mCombinePair;
    Type mMorphType;
    bool mInInventory;
	bool mCanBeDeleted;	
    float mVolume;
    float mMass;
    float mContent;
    string mContentTypeDesc;
public:
	ruEvent OnPickup;
	bool mSingleInstance;
	explicit Item( Type type );
    virtual ~Item();
    bool Combine( Item * pItem, Item* & throwItem );
    Type GetType() const;
    Type GetCombineType() const;
    ruTextureHandle GetPictogram() const;
    float GetContent() const;
    void SetContent( float content );
    const string & GetContentType() const;
    void SetContentType( const string & contentType );
    const string & GetDescription() const;
    const string & GetName() const;
    void SetMass( float mass );
    float GetMass() const;
    float GetVolume() const;
    void SetVolume(float val);
    void MarkAsGrabbed();
    void MarkAsFree();
    bool IsFree();
	//void PickUp();
	static string GetNameByType( Type type ) {
		if( !msLocalization.IsParsed() ) {
			msLocalization.ParseFile( localizationPath + "items.loc" );
		}
		if( type == Type::Detonator ) {
			return msLocalization.GetString( "detonatorName" );
		} else if( type == Type::FuelCanister ) {
			return msLocalization.GetString( "fuelName" );
		} else if( type == Type::Wires ) {
			return msLocalization.GetString( "wiresName" );
		} else if( type == Type::Explosives ) {
			return msLocalization.GetString( "explosivesName" );
		} else if( type == Type::Crowbar ) {
			return msLocalization.GetString( "crowbarName" );
		} else if( type == Type::Pistol ) {
			return msLocalization.GetString( "pistolName" );
		} else if( type == Type::Bullet ) {
			return msLocalization.GetString( "bulletName" );
		} else if( type == Type::Lighter ) {
			return msLocalization.GetString( "flashlightName" );
		} else if( type == Type::Fuse ) {
			return msLocalization.GetString( "fuseName" );
		} else if( type == Type::Syringe ) {
			return msLocalization.GetString( "syringeName" );
		} else if( type == Type::Beaker ) {
			return msLocalization.GetString( "beakerName" );
		} else if( type == Type::FerrumOxide ) {
			return msLocalization.GetString( "fe2o3Name" );
		} else if( type == Type::AluminumPowder ) {
			return msLocalization.GetString( "aluminumName" );
		}
		return "(name is not set)";
	}
    static vector<Item*> msItemList;
};

