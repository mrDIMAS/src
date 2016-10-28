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
		Crowbar			= 8,
		Bullet			= 10,
		Beaker			= 11,
		AluminumPowder  = 12,
		FerrumOxide		= 13,
    };
private:
	friend class Inventory;
    static Parser msLocalization;
    shared_ptr<ruTexture> mPic;
    string mDesc;
    string mName;
    Type mType;
    Type mCombinePair;
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
    shared_ptr<ruTexture> GetPictogram() const;
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
	static string GetNameByType( Type type );
};

inline bool operator < (const Item & a, const Item & b ) {
	return a.GetType() < b.GetType();
}

