#include "Precompiled.h"
#include "BareHands.h"

void BareHands::Update()
{
	SwitchIfAble();
}

Item* BareHands::CreateItem()
{
	return nullptr;
}

BareHands::BareHands()
{
	mModel = ruCreateSceneNode();
}

void BareHands::OnDeserialize( SaveFile & in )
{

}

void BareHands::OnSerialize( SaveFile & out )
{

}
