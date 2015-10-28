#pragma once

#include "UsableObject.h"

class BareHands : public UsableObject {
protected:
	virtual void OnSerialize( SaveFile & out ) final;
	virtual void OnDeserialize( SaveFile & in ) final;
public:
	explicit BareHands();
	virtual Item* CreateItem( ) final;
	virtual void Update() final;
};