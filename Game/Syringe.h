#pragma once

#include "UsableObject.h"

class Syringe : public UsableObject {
private:
	ruAnimation mUseAnim;
	ruAnimation mIdleAnim;
	void Proxy_HealPlayer();
	bool mDepleted;
	virtual void OnSerialize( SaveFile & out ) final;
	virtual void OnDeserialize( SaveFile & in ) final;
public:
	explicit Syringe( );
	virtual void Update() final;
	virtual Item* CreateItem( );
};