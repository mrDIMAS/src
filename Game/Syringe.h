#pragma once

#include "UsableObject.h"

class Syringe : public UsableObject {
private:
	ruAnimation mUseAnim;
	ruAnimation mShowAnim;
	ruAnimation mHideAnim;
	ruAnimation mIdleAnim;
	ruAnimation mIdleDepletedAnim;
	ruAnimation mShowDepletedAnim;
	ruAnimation mHideDepletedAnim;
	void Proxy_HealPlayer();
	void Proxy_Hide();
	void Proxy_Use();
	int mDozeCount;
	virtual void OnSerialize( SaveFile & out ) final;
	virtual void OnDeserialize( SaveFile & in ) final;
public:
	explicit Syringe( );
	~Syringe( ) {

	}
	virtual void Update() final;
	virtual Item::Type GetItemType( ) final {
		return Item::Type::Syringe;
	}
	void AddDoze();
};