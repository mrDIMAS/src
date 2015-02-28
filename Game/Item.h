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
        Flashlight		= 5,
        Fuse			= 6,
        Medkit			= 7,
		Crowbar			= 8,
    };
private:
    static Parser msLocalization;
    ruTextureHandle mPic;
    string mDesc;
    string mName;
    Type mType;
    Type mCombinePair;
    Type mMorphType;
    bool mThrowable;
    bool mInInventory;
    float mVolume;
    float mMass;
    float mContent;
    string mContentTypeDesc;
public:
    explicit Item( ruNodeHandle obj, Type type );
    virtual ~Item();
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
    static vector<Item*> msItemList;
    static Item * GetItemPointerByNode( ruNodeHandle obj );
};

