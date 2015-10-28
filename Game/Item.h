#pragma  once

#include "Game.h"
#include "Parser.h"
#include "InteractiveObject.h"

class Item : public InteractiveObject {
public:
    enum class Type {
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
    bool mThrowable;
    bool mInInventory;
	bool mCanBeDeleted;
	
    float mVolume;
    float mMass;
    float mContent;
	string mModelFile;
    string mContentTypeDesc;
	void Initialize( Type type );
public:
	ruEvent OnPickup;
	bool mSingleInstance;
    explicit Item( ruSceneNode obj, Type type );
	explicit Item( Type type );
    virtual ~Item();
	void Repair();
    bool Combine( Item * pItem, Item* & throwItem );
    Type GetType() const;
    Type GetCombineType() const;
    bool IsThrowable() const;
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
    void SetType( Type type );
    void MarkAsGrabbed();
    void MarkAsFree();
    bool IsFree();
	void PickUp();
    static vector<Item*> msItemList;
    static Item * GetItemPointerByNode( ruSceneNode obj );
};

