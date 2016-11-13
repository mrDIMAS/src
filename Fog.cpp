#include "Precompiled.h"
#include "Fog.h"
#include "SceneNode.h"
#include "SceneFactory.h"

Fog::Fog(const ruVector3 & min, const ruVector3 & max, const ruVector3 & color, float density) :
	mColor(color),
	mMin(min),
	mMax(max),
	mDensity(density),
	mAABB(min, max),
	mSpeed(0.002, 0, 0.002) {

}

shared_ptr<ruFog> ruFog::Create(const ruVector3 & min, const ruVector3 & max, const ruVector3 & color, float density) {
	return SceneFactory::CreateFog(min, max, color, density);
}