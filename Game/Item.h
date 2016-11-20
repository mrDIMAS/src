#pragma  once

#include "Game.h"
#include "Parser.h"
#include "InteractiveObject.h"

class Item {
public:
	enum class Type : int {
		Unknown,
		Detonator,
		Wires,
		Explosives,
		Lighter,
		Fuse,
		Crowbar,
		AluminumPowder,
		FerrumOxide,
	};
private:
	friend class Inventory;
	static ruConfig msLocalization;
	shared_ptr<ruTexture> mPic;
	string mDesc;
	string mName;
	Type mType;
	float mVolume;
	float mMass;
	float mContent;
	string mContentTypeDesc;
public:
	bool mSingleInstance;
	explicit Item(Type type);
	virtual ~Item();
	bool Combine(Item::Type combinerType);
	Type GetType() const;
	shared_ptr<ruTexture> GetPictogram() const;
	float GetContent() const;
	void SetContent(float content);
	const string & GetContentType() const;
	void SetContentType(const string & contentType);
	const string & GetDescription() const;
	const string & GetName() const;
	void SetMass(float mass);
	float GetMass() const;
	float GetVolume() const;
	void SetVolume(float val);
	static string GetNameByType(Type type);
};

inline bool operator < (const Item & a, const Item & b)
{
	return a.GetType() < b.GetType();
}

