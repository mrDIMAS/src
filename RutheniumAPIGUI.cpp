#include "Precompiled.h"
#include "RutheniumAPI.h"
#include "GUIScene.h"

shared_ptr<ruGUIScene> ruGUIScene::Create() {
	return GUIScene::Create();
}
