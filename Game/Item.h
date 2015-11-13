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
    float mVolume;
    float mMass;
    float mContent;
    string mContentTypeDesc;
public:
	bool mSingleInstance;
	explicit Item( Type type );
    virtual ~Item();
    bool Combine( Item::Type combinerType );
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
	//void PickUp();
	static string GetNameByType( Type type );
    static vector<Item*> msItemList;
};

inline bool operator < (const Item & a, const Item & b ) {
	return a.GetType() < b.GetType();
}

