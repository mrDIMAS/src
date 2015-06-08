#include "Precompiled.h"
#include "RendererComponent.h"

vector<RendererComponent*> RendererComponent::msComponentList;




RendererComponent::~RendererComponent()
{
	msComponentList.erase( find( msComponentList.begin(), msComponentList.end(), this ));
}

RendererComponent::RendererComponent()
{
	mResetPriority = ResetPriority::Low;
	msComponentList.push_back( this );
}
