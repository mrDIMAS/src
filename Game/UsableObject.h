#pragma once

#include "SaveFile.h"

class UsableObject {
protected:
	ruSceneNode mModel;
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
	ruSceneNode GetModel();
	void Link( UsableObject * other );
	void Next( );
	void Prev( );
	void Appear( );
	// interface
	virtual Item* CreateItem( ) = 0;
	virtual void Update() = 0;
	void Serialize( SaveFile & out );
	static UsableObject * Deserialize( SaveFile & in );
};