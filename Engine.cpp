#include "Precompiled.h"
#include "Engine.h"
#include "GUIScene.h"
#include "BitmapFont.h"

// can't actually overload with unique_ptr<Renderer> due to non-covariance, simply return raw pointers

shared_ptr<ruGUIScene> Engine::CreateGUIScene() {
	auto scene = make_shared<GUIScene>(this);
	msSceneList.push_back(scene);
	return scene;
}

Physics * const Engine::GetPhysics() const {
	return mPhysics.get();
}

const vector<weak_ptr<GUIScene>>& Engine::GetGUISceneList() {
	RemoveUnreferenced(msSceneList);
	return msSceneList;
}


// API Methods

shared_ptr<ruFont> Engine::CreateBitmapFont(int size, const string & filename) {
	return make_shared<BitmapFont>(this, filename, size);
}

Renderer * const Engine::GetRenderer() const {
	return mRenderer.get();
}

SceneFactory * const Engine::GetSceneFactory() const {
	return mSceneFactory.get();
}

