#include "Precompiled.h"
#include "Fog.h"
#include "SceneNode.h"
#include "SceneFactory.h"

Fog::Fog(SceneFactory * factory, const Vector3 & min, const Vector3 & max, const Vector3 & color, float density) :
	SceneNode(factory),
	mColor(color),
	mMin(min),
	mMax(max),
	mDensity(density),
	mAABB(min, max),
	mSpeed(0.002, 0, 0.002) {

}
