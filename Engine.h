#pragma once

#include "Renderer.h"
#include "SceneFactory.h"
#include "Physics.h"
#include "Input.h"
#include "SoundSystem.h"

class Engine : public IEngine {
private:
	unique_ptr<Renderer> mRenderer;
	unique_ptr<SceneFactory> mSceneFactory;
	unique_ptr<Physics> mPhysics;
	unique_ptr<Input> mInput;
	unique_ptr<SoundSystem> mSoundSystem;
	vector<weak_ptr<GUIScene>> msSceneList;

	template<typename Type>
	void RemoveUnreferenced(vector<weak_ptr<Type>>& objList);
public:
	Engine(int width, int height, bool fullscreen, bool vSync);
	virtual ~Engine();
	const vector<weak_ptr<GUIScene>> & GetGUISceneList();
	// API Methods
	virtual shared_ptr<IFont> CreateBitmapFont(int size, const string & filename) override final;
	// can't actually overload with unique_ptr<Renderer> due to non-covariance, simply return raw pointer
	virtual Renderer * const GetRenderer() const override final;
	virtual SceneFactory * const GetSceneFactory() const override final;
	virtual shared_ptr<IGUIScene> CreateGUIScene() override final;
	virtual Physics * const GetPhysics() const override final;
	virtual Input * const GetInput() const override final;
	virtual SoundSystem * const GetSoundSystem() const override final;
};

template<typename Type>
void Engine::RemoveUnreferenced(vector<weak_ptr<Type>>& objList) {
	for(auto iter = objList.begin(); iter != objList.end(); ) {
		if((*iter).use_count()) {
			++iter;
		} else {
			iter = objList.erase(iter);
		}
	}
}
