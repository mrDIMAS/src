#include "Precompiled.h"
#include "Level.h"
#include "LevelArrival.h"
#include "LevelMine.h"
#include "GUIProperties.h"
#include "Player.h"
#include "Menu.h"
#include "LevelResearchFacility.h"
#include "LevelIntroduction.h"
#include "SaveLoader.h"
#include "SaveWriter.h"
#include "LevelSewers.h"
#include "LevelCutsceneIntro.h"

unique_ptr<Level> Level::msCurrent;

unique_ptr<LoadingScreen> Level::msLoadingScreen;

int g_initialLevel;
int Level::msCurLevelID = 0;

Level::Level(const unique_ptr<PlayerTransfer> & playerTransfer) {
	mInitializationComplete = false;
	mTypeNum = 0; //undefined

	 // create player
	if (Level::msCurLevelID != LevelName::L0Introduction && Level::msCurLevelID != LevelName::LCSIntro) {
		mPlayer = unique_ptr<Player>(new Player);

		// restore state
		if (playerTransfer) {
			mPlayer->SetHealth(playerTransfer->mHealth);

			for (auto uo : playerTransfer->mUsableObjects) {
				mPlayer->AddUsableObject(uo);
			}
			if (playerTransfer->mItems.size()) {
				mPlayer->GetInventory()->SetItems(playerTransfer->mItems);
			}
		}
	}
}

Level::~Level() {
	for (auto pButton : mButtonList) {
		delete pButton;
	}
}

void Level::LoadLocalization(string fn) {
	mLocalization.ParseFile(localizationPath + fn);
}

void Level::Hide() {
	mScene->Hide();
	for (auto & sWeak : mSounds) {
		shared_ptr<ruSound> & sound = sWeak.lock();
		if (sound) {
			sound->Pause();
		}
	}
	for (auto pLamp : mLampList) {
		pLamp->Hide();
	}
}

void Level::Show() {
	mScene->Show();
	for (auto & sWeak : mSounds) {
		shared_ptr<ruSound> & sound = sWeak.lock();
		if (sound) {
			if (sound->IsPaused()) {
				sound->Play();
			}
		}
	}
	for (auto pLamp : mLampList) {
		pLamp->Show();
	}
}

void Level::Change(int levelId, bool continueFromSave) {
	static int lastLevel = 0;

	Level::msCurLevelID = levelId;

	// save player state
	unique_ptr<PlayerTransfer> playerTransfer;

	if (msCurrent) {
		if (msCurrent->mPlayer) {
			playerTransfer = unique_ptr<PlayerTransfer>(new PlayerTransfer);
			if (lastLevel != levelId) {
				msCurrent->mPlayer->DumpUsableObjects(playerTransfer->mUsableObjects);
			}
			msCurrent->mPlayer->GetInventory()->GetItems(playerTransfer->mItems);
			playerTransfer->mHealth = msCurrent->mPlayer->GetHealth();
		}

		// delete player
		msCurrent->mPlayer.reset();
	}

	// delete level
	msCurrent.reset();

	msLoadingScreen->Draw();

	// load new level and restore player state
	switch (Level::msCurLevelID) {
	case LevelName::LCSIntro:
		msCurrent = unique_ptr<Level>(new LevelCutsceneIntro(playerTransfer));
		break;
	case LevelName::L0Introduction:
		msCurrent = unique_ptr<Level>(new LevelIntroduction(playerTransfer));
		break;
	case LevelName::L1Arrival:
		msCurrent = unique_ptr<Level>(new LevelArrival(playerTransfer));
		break;
	case LevelName::L2Mine:
		msCurrent = unique_ptr<Level>(new LevelMine(playerTransfer));
		break;
	case LevelName::L3ResearchFacility:
		msCurrent = unique_ptr<Level>(new LevelResearchFacility(playerTransfer));
		break;
	case LevelName::L4Sewers:
		msCurrent = unique_ptr<Level>(new LevelSewers(playerTransfer));
		break;
	default:
		throw runtime_error("Unable to load level with bad id!");
		break;
	}

	if (continueFromSave) {
		SaveLoader("lastGame.save").RestoreWorldState();
	}

	msCurrent->mPlayer->SetTip("Loaded");

	lastLevel = levelId;

	Game_UpdateClock();
}

shared_ptr<Lift> Level::AddLift(const string & baseNode, const string & controlPanel, const string & sourceNode, const string & destNode, const string & doorFrontLeft, const string & doorFrontRight, const string & doorBackLeft, const string & mDoorBackRight) {
	shared_ptr<Lift> lift(new Lift(GetUniqueObject(baseNode)));

	lift->SetControlPanel(GetUniqueObject(controlPanel));
	lift->SetDestinationPoint(GetUniqueObject(destNode));
	lift->SetSourcePoint(GetUniqueObject(sourceNode));
	lift->SetBackDoors(AddDoor(doorBackLeft, -90), AddDoor(mDoorBackRight, -90));
	lift->SetFrontDoors(AddDoor(doorFrontLeft, 90), AddDoor(doorFrontRight, 90));

	mLiftList.push_back(lift);

	return lift;
}

const vector<shared_ptr<Ladder>> & Level::GetLadderList() const {
	return mLadderList;
}

void Level::AddValve(const shared_ptr<Valve> & valve) {
	mValveList.push_back(valve);
}

const vector<shared_ptr<Door>> & Level::GetDoorList() const {
	return mDoorList;
}

shared_ptr<Gate> Level::AddGate(const string & nodeName, const string & buttonOpen, const string & buttonClose, const string & buttonOpen2, const string & buttonClose2) {
	shared_ptr<Gate> gate(new Gate(GetUniqueObject(nodeName), GetUniqueObject(buttonOpen), GetUniqueObject(buttonClose), GetUniqueObject(buttonOpen2), GetUniqueObject(buttonClose2)));
	mGateList.push_back(gate);
	return gate;
}

const vector<shared_ptr<Gate>> & Level::GetGateList() const {
	return mGateList;
}

shared_ptr<Ladder> Level::AddLadder(const string & hBegin, const string & hEnd, const string & hEnterZone, const string & hBeginLeavePoint, const string & hEndLeavePoint) {
	shared_ptr<Ladder> ladder(new Ladder(GetUniqueObject(hBegin), GetUniqueObject(hEnd), GetUniqueObject(hEnterZone), GetUniqueObject(hBeginLeavePoint), GetUniqueObject(hEndLeavePoint)));
	mLadderList.push_back(ladder);
	return ladder;
}

shared_ptr<Ladder> Level::FindLadder(const string & name) {
	for (auto & ladder : mLadderList) {
		if (ladder->GetEnterZone()->GetName() == name) {
			return ladder;
		}
	}
	return shared_ptr<Ladder>(nullptr);
}

shared_ptr<Door> Level::AddDoor(const string & nodeName, float fMaxAngle) {
	shared_ptr<Door> door(new Door(GetUniqueObject(nodeName), fMaxAngle));
	mDoorList.push_back(door);
	return door;
}

shared_ptr<Door> Level::FindDoor(const string & name) {
	for (auto & door : mDoorList) {
		if (door->mDoorNode->GetName() == name) {
			return door;
		}
	}
	return shared_ptr<Door>(nullptr);
}

shared_ptr<Sheet> Level::AddSheet(const string & nodeName, const string & desc, const string & text) {
	shared_ptr<Sheet> sheet(new Sheet(mPlayer->mGUIScene, GetUniqueObject(nodeName), desc, text));
	mInteractiveObjectList.push_back(sheet);
	mSheetList.push_back(sheet);
	return sheet;
}

shared_ptr<Sheet> Level::FindSheet(const string & name) {
	for (auto & sheet : mSheetList) {
		if (sheet->mObject->GetName() == name) {
			return sheet;
		}
	}
	return shared_ptr<Sheet>(nullptr);
}

void Level::AddItemPlace(const shared_ptr<ItemPlace> & pItemPlace) {
	mItemPlaceList.push_back(pItemPlace);
}

shared_ptr<ItemPlace> Level::FindItemPlace(const string & name) {
	for (auto & ipl : mItemPlaceList) {
		if (ipl->mObject->GetName() == name) {
			return ipl;
		}
	}
	return shared_ptr<ItemPlace>(nullptr);
}

shared_ptr<Keypad> Level::AddKeypad(const string & keypad, const string & key0, const string & key1, const string & key2, const string & key3, const string & key4, const string & key5, const string & key6, const string & key7, const string & key8, const string & key9, const string & keyCancel, weak_ptr<Door> doorToUnlock, const string & codeToUnlock) {
	shared_ptr<Keypad> k(new Keypad(GetUniqueObject(keypad), GetUniqueObject(key0), GetUniqueObject(key1),
		GetUniqueObject(key2), GetUniqueObject(key3), GetUniqueObject(key4), GetUniqueObject(key5), GetUniqueObject(key6),
		GetUniqueObject(key7), GetUniqueObject(key8), GetUniqueObject(key9), GetUniqueObject(keyCancel), doorToUnlock, codeToUnlock));
	mKeypadList.push_back(k);
	return k;
}

void Level::Deserialize(SaveFile & in) {
	mPlayer->Deserialize(in);

	int childCount = in.ReadInteger();
	for (int i = 0; i < childCount; i++) {
		string name = in.ReadString();
		shared_ptr<ruSceneNode> node = mScene->FindChild(name);
		ruVector3 pos = in.ReadVector3();
		ruQuaternion quat = in.ReadQuaternion();
		bool visible = in.ReadBoolean();
		bool isLight = in.ReadBoolean();
		float litRange = 0.0f;
		if (isLight) {
			litRange = in.ReadFloat();
		}
		if (node) {
			node->SetLocalPosition(pos);
			node->SetLocalRotation(quat);
			if (visible) {
				node->Show();
			} else {
				node->Hide();
			}
			if (isLight) {
				std::dynamic_pointer_cast<ruLight>(node)->SetRange(litRange);
			}
		}
	}
	int countStages = in.ReadInteger();
	for (int i = 0; i < countStages; i++) {
		string stageName = in.ReadString();
		bool stageState = in.ReadBoolean();
		mStages[stageName] = stageState;
	}
	int doorCount = in.ReadInteger();
	for (int i = 0; i < doorCount; i++) {
		string name = in.ReadString();
		shared_ptr<Door> pDoor;
		for (auto door : mDoorList) {
			if (door->mDoorNode->GetName() == name) {
				pDoor = door;
				break;
			}
		}
		if (pDoor) {
			pDoor->Deserialize(in);
		}
	}

	for (auto pLift : mLiftList) {
		pLift->Deserialize(in);
	}

	int wayCount = in.ReadInteger();
	for (int i = 0; i < wayCount; i++) {
		auto ladder = FindLadder(in.ReadString());
		ladder->Deserialize(in);
	}

	// deserialize item places
	int countItemPlaces = in.ReadInteger();

	for (int i = 0; i < countItemPlaces; i++) {
		string ipName = in.ReadString();
		Item::Type placedItem = static_cast<Item::Type>(in.ReadInteger());
		Item::Type placeType = static_cast<Item::Type>(in.ReadInteger());

		auto pItemPlace = FindItemPlace(ipName);

		if (pItemPlace) {
			pItemPlace->mItemPlaced = placedItem;
			pItemPlace->mItemTypeCanBePlaced = placeType;
		}
	}

	OnDeserialize(in);
}

void Level::Purge() {
	msCurrent.reset();
}

void Level::Serialize(SaveFile & out) {
	mPlayer->Serialize(out);

	mPlayer->SetTip("Saved");

	int childCount = mScene->GetCountChildren();
	out.WriteInteger(childCount);
	for (int i = 0; i < childCount; i++) {
		shared_ptr<ruSceneNode> node = mScene->GetChild(i);
		out.WriteString(node->GetName());
		out.WriteVector3(node->GetLocalPosition());
		out.WriteQuaternion(node->GetLocalRotation());
		out.WriteBoolean(node->IsVisible());
		shared_ptr<ruLight>light = std::dynamic_pointer_cast<ruLight>(node);
		out.WriteBoolean(light != nullptr);
		if (light) {
			out.WriteFloat(light->GetRange());
		}
	}
	out.WriteInteger(mStages.size());
	for (auto stage : mStages) {
		out.WriteString(stage.first);
		out.WriteBoolean(stage.second);
	}

	out.WriteInteger(mDoorList.size());
	for (auto pDoor : mDoorList) {
		out.WriteString(pDoor->mDoorNode->GetName());
		pDoor->Serialize(out);
	}

	for (auto pLift : mLiftList) {
		pLift->Serialize(out);
	}

	out.WriteInteger(mLadderList.size());
	for (auto pWay : mLadderList) {
		pWay->Serialize(out);
	}

	out.WriteInteger(mItemPlaceList.size());
	for (auto pItemPlace : mItemPlaceList) {
		out.WriteString(pItemPlace->mObject->GetName());
		out.WriteInteger(static_cast<int>(pItemPlace->mItemPlaced));
		out.WriteInteger(static_cast<int>(pItemPlace->GetPlaceType()));
	}

	OnSerialize(out);
}

void Level::AddSound(shared_ptr<ruSound> sound) {
	if (!sound) {
		throw std::runtime_error("Unable to add ambient sound! Invalid source!");
	}
	mSounds.push_back(sound);
}

void Level::PlayAmbientSounds() {
	mAmbSoundSet.DoRandomPlaying();
}

void Level::AddAmbientSound(shared_ptr<ruSound> sound) {
	if (!sound) {
		throw std::runtime_error("Unable to add ambient sound! Invalid source!");
	}
	mSounds.push_back(sound);
	mAmbSoundSet.AddSound(sound);
}

shared_ptr<ruSceneNode> Level::GetUniqueObject(const string & name) {
	// the point of this behaviour is to reduce number of possible errors during runtime, if some object doesn't exist in the scene( but it must )
	// game notify user on level loading stage, but not in the game. So this feature is very useful for debugging purposes
	// also this feature can help to improve some performance by reducing FindXXX calls, which take a lot of time
	if (mInitializationComplete) {
		throw std::runtime_error(StringBuilder("You must get object in game level initialization! Get object in game logic loop is strictly forbidden! Object name: ") << name);
	}
	if (!mScene) {
		throw std::runtime_error(StringBuilder("Object ") << name << " can't be found in the empty scene. Load scene first!");
	}
	shared_ptr<ruSceneNode> object = mScene->FindChild(name);
	// each unique object must be presented in the scene, otherwise error will be generated
	if (!object) {
		throw std::runtime_error(StringBuilder("Object ") << name << " can't be found in the scene! Game will be closed.");
	}
	return object;
}

void Level::LoadSceneFromFile(const string & file) {
	mScene = ruSceneNode::LoadFromFile(file);
	if (!mScene) {
		throw std::runtime_error(StringBuilder("Unable to load scene from ") << file << "! Game will be closed.");
	}
}

void Level::CreateBlankScene() {
	mScene = ruSceneNode::Create();
}

void Level::BuildPath(Path & path, const string & nodeBaseName) {
	path.BuildPath(mScene, nodeBaseName);
}

void Level::DoneInitialization() {
	mInitializationComplete = true;
}

void Level::CreateLoadingScreen() {
	msLoadingScreen = unique_ptr<LoadingScreen>(new LoadingScreen());
}

unique_ptr<Level> & Level::Current() {
	return msCurrent;
}

void Level::AddLamp(const shared_ptr<Lamp> & lamp) {
	mLampList.push_back(lamp);
}

void Level::UpdateGenericObjectsIdle() {
	if (mPlayer) {
		mPlayer->Update();
	}

	if (mMusic) {
		mMusic->SetVolume(pMainMenu->GetMusicVolume());
	}

	for (auto io : mInteractiveObjectList) {
		io->Update();
		io->UpdateFlashing();
	}

	for (auto kp : mKeypadList) {
		kp->Update();
	}

	for (auto pLamp : mLampList) {
		pLamp->Update();
	}
	for (auto pZone : mZoneList) {
		pZone->Update();
	}
	for (auto pButton : mButtonList) {
		pButton->Update();
	}
}

void Level::AutoCreateLampsByNamePattern(const string & namePattern, string buzzSound) {
	std::regex rx(namePattern);
	for (int i = 0; i < mScene->GetCountChildren(); i++) {
		shared_ptr<ruSceneNode> child = mScene->GetChild(i);
		if (regex_match(child->GetName(), rx)) {
			AddLamp(make_shared<Lamp>(child, ruSound::Load3D(buzzSound)));
		}
	}
}

void Level::AutoCreateBulletsByNamePattern(const string & namePattern) {
	std::regex rx(namePattern);
	for (int i = 0; i < mScene->GetCountChildren(); i++) {
		shared_ptr<ruSceneNode> child = mScene->GetChild(i);
		if (regex_match(child->GetName(), rx)) {
			AddInteractiveObject(Item::GetNameByType(Item::Type::Bullet), make_shared<InteractiveObject>(child), ruDelegate::Bind(this, &Level::Proxy_GiveBullet));
		}
	}
}

void Level::AutoCreateDoorsByNamePattern(const string & namePattern) {
	std::regex rx(namePattern);
	for (int i = 0; i < mScene->GetCountChildren(); i++) {
		shared_ptr<ruSceneNode> child = mScene->GetChild(i);
		bool ignore = false;
		for (auto pDoor : mDoorList) {
			if (pDoor->mDoorNode == child) {
				ignore = true;
				break;
			}
		}
		if (!ignore) {
			if (regex_match(child->GetName(), rx)) {
				AddDoor(child->GetName(), 90);
			}
		}
	}
}

void Level::AddZone(const shared_ptr<Zone> & zone) {
	mZoneList.push_back(zone);
}

void Level::AddButton(Button * button) {
	mButtonList.push_back(button);
}

unique_ptr<Enemy> & Level::GetEnemy() {
	return mEnemy;
}

void Level::AddInteractiveObject(const string & desc, const shared_ptr<InteractiveObject> & io, const ruDelegate & interactAction) {
	io->OnInteract.AddListener(interactAction);
	io->SetPickDescription(desc);
	mInteractiveObjectList.push_back(io);
}

shared_ptr<InteractiveObject> Level::FindInteractiveObject(const string & name) {
	for (auto & io : mInteractiveObjectList) {
		if (io->mObject->GetName() == name) {
			return io;
		}
	}
	return shared_ptr<InteractiveObject>(nullptr);
}

void Level::DeserializeAnimation(SaveFile & in, ruAnimation & anim) {
	anim.SetCurrentFrame(in.ReadInteger());
	anim.SetEnabled(in.ReadBoolean());
}

void Level::SerializeAnimation(SaveFile & out, ruAnimation & anim) {
	out.WriteInteger(anim.GetCurrentFrame());
	out.WriteBoolean(anim.IsEnabled());
}

void Level::Proxy_GiveBullet() {
	mPlayer->AddItem(Item::Type::Bullet);
}
