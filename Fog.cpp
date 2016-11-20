#include "Precompiled.h"
#include "Fog.h"
#include "SceneNode.h"
#include "SceneFactory.h"

Fog::Fog(SceneFactory * factory, const ruVector3 & min, const ruVector3 & max, const ruVector3 & color, float density) :
	SceneNode(factory),
	mColor(color),
	mMin(min),
	mMax(max),
	mDensity(density),
	mAABB(min, max),
	mSpeed(0.002, 0, 0.002) {

}
