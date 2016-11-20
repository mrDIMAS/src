#pragma once

#include "Renderer.h"
#include "SceneFactory.h"
#include "Physics.h"
#include "Input.h"


class Engine : public ruEngine {
private:
	unique_ptr<Renderer> mRenderer;
	unique_ptr<SceneFactory> mSceneFactory;
	unique_ptr<Physics> mPhysics;
	unique_ptr<Input> mInput;

	vector<weak_ptr<GUIScene>> msSceneList;


	template<typename Type>
	void RemoveUnreferenced(vector<weak_ptr<Type>>& objList) {
		for(auto iter = objList.begin(); iter != objList.end(); ) {
			if((*iter).use_count()) {
				++iter;
			} else {
				iter = objList.erase(iter);
			}
		}
	}

public:
	Engine(int width, int height, bool fullscreen, bool vSync) {
		mRenderer = make_unique<Renderer>(this, width, height, fullscreen, vSync);
		mSceneFactory = make_unique<SceneFactory>(this);
		mPhysics = make_unique<Physics>(this);
		mInput = make_unique<Input>(this, mRenderer->GetWindow());
	}

	virtual ~Engine() {		
		mSceneFactory.reset();
		mPhysics.reset();
		mInput.reset();
		mRenderer.reset();
	}

	const vector<weak_ptr<GUIScene>> & GetGUISceneList();

	// API Methods
	virtual shared_ptr<ruFont> CreateBitmapFont(int size, const string & filename) override final;
	// can't actually overload with unique_ptr<Renderer> due to non-covariance, simply return raw pointer
	virtual Renderer * const GetRenderer() const override final;
	virtual SceneFactory * const GetSceneFactory() const override final;
	virtual shared_ptr<ruGUIScene> CreateGUIScene() override final;
	virtual Physics * const GetPhysics() const override final;
	virtual ruInput * const GetInput() const override final {
		return mInput.get();
	}
};