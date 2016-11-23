#include "Precompiled.h"
#include "Engine.h"
#include "GUIScene.h"
#include "BitmapFont.h"

// can't actually overload with unique_ptr<Renderer> due to non-covariance, simply return raw pointers

shared_ptr<IGUIScene> Engine::CreateGUIScene() {
	auto scene = make_shared<GUIScene>(this);
	msSceneList.push_back(scene);
	return scene;
}

Physics * const Engine::GetPhysics() const {
	return mPhysics.get();
}

Input * const Engine::GetInput() const {
	return mInput.get();
}

SoundSystem * const Engine::GetSoundSystem() const {
	return mSoundSystem.get();
}

Engine::Engine(int width, int height, bool fullscreen, bool vSync) {
	mRenderer = make_unique<Renderer>(this, width, height, fullscreen, vSync);
	mSceneFactory = make_unique<SceneFactory>(this);
	mPhysics = make_unique<Physics>(this);
	mInput = make_unique<Input>(this, mRenderer->GetWindow());
	mSoundSystem = make_unique<SoundSystem>();
}

Engine::~Engine() {
	mSceneFactory.reset();
	mPhysics.reset();
	mInput.reset();
	mRenderer.reset();
}

const vector<weak_ptr<GUIScene>>& Engine::GetGUISceneList() {
	RemoveUnreferenced(msSceneList);
	return msSceneList;
}


// API Methods

shared_ptr<IFont> Engine::CreateBitmapFont(int size, const string & filename) {
	return make_shared<BitmapFont>(this, filename, size);
}

Renderer * const Engine::GetRenderer() const {
	return mRenderer.get();
}

SceneFactory * const Engine::GetSceneFactory() const {
	return mSceneFactory.get();
}

