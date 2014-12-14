#pragma  once

#include "Game.h"
#include "Parser.h"
#include "InteractiveObject.h"

class Item : public InteractiveObject
{
public:
	enum class Type
	{
		Unknown,
		Detonator,
		FuelCanister,
		Wires,
		Explosives,
		Flashlight,
		Fuse,
		Medkit,
	};
private:
	static Parser msLoc;
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
	const char * GetContentType() const;
	void SetContentType( const char * contentType );
	const char * GetDescription() const;
	const char * GetName() const;
	void SetMass( float mass );
	float GetMass() const;
	float GetVolume() const;
	void SetVolume(float val);
    void SetType( Type type );
	void MarkAsGrabbed();
	void MarkAsFree();
	bool IsFree();
    static vector<Item*> Available;
    static Item * GetByObject( ruNodeHandle obj );
};

