#pragma once

#include "SaveFile.h"
#include "Item.h"

class UsableObject {
protected:
	shared_ptr<ruSceneNode> mModel;
	UsableObject * mNext;
	UsableObject * mPrev;
	bool mToNext;
	bool mToPrev;
	bool mAppear;
	void SwitchIfAble();
	virtual void OnSerialize( SaveFile & out ) = 0;
	virtual void OnDeserialize( SaveFile & in ) = 0;
public:
	explicit UsableObject();
	virtual ~UsableObject();
	shared_ptr<ruSceneNode> GetModel();
	void Link( UsableObject * other );
	void Next( );
	void Prev( );
	void Appear( );
	// interface
	virtual Item::Type GetItemType( ) = 0;
	virtual void Update() = 0;
	void Serialize( SaveFile & out );
	static UsableObject * Deserialize( SaveFile & in );
};