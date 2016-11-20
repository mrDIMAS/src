#include "Precompiled.h"
#include "Level.h"
#include "GUIProperties.h"
#include "Player.h"
#include "Menu.h"
#include "SaveLoader.h"
#include "SaveWriter.h"

LevelName g_initialLevel = LevelName::Undefined;
LevelName Level::msCurLevelID = LevelName::Undefined;

Level::Level(unique_ptr<Game> & game, const unique_ptr<PlayerTransfer> & playerTransfer) :
	mGame(game),
	mChaseMusicStopInterval(0),
	mChaseMusicVolume(1.0f),
	mDestChaseMusicVolume(1.0f),
	mEnded(false) {
	mInitializationComplete = false;
	mName = LevelName::Undefined;

	// create player
	if(Level::msCurLevelID != LevelName::Intro && Level::msCurLevelID != LevelName::Ending) {
		mPlayer = unique_ptr<Player>(new Player(mGame));

		// restore state
		if(playerTransfer) {
			mPlayer->SetHealth(playerTransfer->mHealth);

			if(playerTransfer->mItems.size()) {
				mPlayer->GetInventory()->SetItems(playerTransfer->mItems);
			}
		}
	}

	mChaseMusic = ruSound::LoadMusic("data/music/chase.ogg");
}

Level::~Level() {

}

void Level::LoadLocalization(string fn) {
	mLocalization.Load(mGame->GetLocalizationPath() + fn);
}

void Level::Hide() {
	mScene->Hide();
	for(auto & sWeak : mSounds) {
		shared_ptr<ruSound> & sound = sWeak.lock();
		if(sound) {
			sound->Pause();
		}
	}
}

void Level::Show() {
	mScene->Show();
	for(auto & sWeak : mSounds) {
		shared_ptr<ruSound> & sound = sWeak.lock();
		if(sound) {
			if(sound->IsPaused()) {
				sound->Play();
			}
		}
	}
}

bool Level::IsVisible() {
	return mScene->IsVisible();
}


shared_ptr<Lift> Level::AddLift(const string & baseNode, const string & sourceNode, const string & destNode, const string & doorFrontLeft, const string & doorFrontRight, const string & doorBackLeft, const string & mDoorBackRight) {
	shared_ptr<Lift> lift(new Lift(GetUniqueObject(baseNode)));

	lift->SetDestinationPoint(GetUniqueObject(destNode));
	lift->SetSourcePoint(GetUniqueObject(sourceNode));
	lift->SetFrontDoors(AddDoor(doorBackLeft, 90), AddDoor(mDoorBackRight, 90));
	lift->SetBackDoors(AddDoor(doorFrontLeft, 90), AddDoor(doorFrontRight, 90));

	mLiftList.push_back(lift);

	return lift;
}

const vector<shared_ptr<Ladder>> & Level::GetLadderList() const {
	return mLadderList;
}

void Level::AddLightSwitch(const shared_ptr<LightSwitch>& lswitch) {
	mLightSwitchList.push_back(lswitch);
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
	for(auto & ladder : mLadderList) {
		if(ladder->mEnterZone->GetName() == name) {
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
	for(auto & door : mDoorList) {
		if(door->mDoorNode->GetName() == name) {
			return door;
		}
	}
	return shared_ptr<Door>(nullptr);
}

void Level::AddItemPlace(const shared_ptr<ItemPlace> & pItemPlace) {
	mItemPlaceList.push_back(pItemPlace);
}

shared_ptr<ItemPlace> Level::FindItemPlace(const string & name) {
	for(auto & ipl : mItemPlaceList) {
		if(ipl->mObject->GetName() == name) {
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

void Level::Serialize(SaveFile & s) {
	mPlayer->Serialize(s);

	mPlayer->GetHUD()->SetTip(mPlayer->GetLocalization()->GetString("saved"));

	// serialize all scene nodes
	int childCount = mScene->GetCountChildren();
	s & childCount;
	for(int i = 0; i < childCount; i++) {
		shared_ptr<ruSceneNode> node = mScene->GetChild(i);
		shared_ptr<ruLight> light = std::dynamic_pointer_cast<ruLight>(node);

		auto position = node->GetLocalPosition();
		auto rotation = node->GetLocalRotation();
		auto visible = node->IsVisible();
		auto name = node->GetName();
		auto isLight = light != nullptr;
		auto lightRange = light ? light->GetRange() : 0.0f;

		s & name;
		s & position;
		s & rotation;
		s & visible;
		s & isLight;
		s & lightRange;

		// apply values on load
		if(s.IsLoading()) {
			node->SetLocalPosition(position);
			node->SetLocalRotation(rotation);
			if(visible) {
				node->Show();
			} else {
				node->Hide();
			}
			if(isLight) {
				std::dynamic_pointer_cast<ruLight>(node)->SetRange(lightRange);
			}
		}
	}
	// serialize doors
	for(auto door : mDoorList) {
		door->Serialize(s);
	}
	// serialize lifts
	for(auto lift : mLiftList) {
		lift->Serialize(s);
	}
	// serialize ladders
	for(auto ladder : mLadderList) {
		ladder->Serialize(s);
	}
	// serialize item places
	for(auto itemPlace : mItemPlaceList) {
		itemPlace->Serialize(s);
	}
	// serialize light switches
	for(auto lswitch : mLightSwitchList) {
		lswitch->Serialize(s);
	}
	// serialize stages
	s & mStages;
	OnSerialize(s);
}

void Level::AddSound(shared_ptr<ruSound> sound) {
	if(!sound) {
		throw std::runtime_error("Unable to add ambient sound! Invalid source!");
	}
	mSounds.push_back(sound);
}

void Level::PlayAmbientSounds() {
	mAmbSoundSet.DoRandomPlaying();
}

void Level::AddAmbientSound(shared_ptr<ruSound> sound) {
	if(!sound) {
		throw std::runtime_error("Unable to add ambient sound! Invalid source!");
	}
	mSounds.push_back(sound);
	mAmbSoundSet.AddSound(sound);
}

shared_ptr<ruSceneNode> Level::GetUniqueObject(const string & name) {
	// the point of this behaviour is to reduce number of possible errors during runtime, if some object doesn't exist in the scene( but it must )
	// game notify user on level loading stage, but not in the game. So this feature is very useful for debugging purposes
	// also this feature can help to improve some performance by reducing FindXXX calls, which take a lot of time
	if(mInitializationComplete) {
		throw std::runtime_error(StringBuilder("You must get object in game level initialization! Get object in game logic loop is strictly forbidden! Object name: ") << name);
	}
	if(!mScene) {
		throw std::runtime_error(StringBuilder("Object ") << name << " can't be found in the empty scene. Load scene first!");
	}
	shared_ptr<ruSceneNode> object = mScene->FindChild(name);
	// each unique object must be presented in the scene, otherwise error will be generated
	if(!object) {
		throw std::runtime_error(StringBuilder("Object ") << name << " can't be found in the scene! Game will be closed.");
	}
	return object;
}

void Level::LoadSceneFromFile(const string & file) {
	mScene = mGame->GetEngine()->GetSceneFactory()->LoadScene(file);
	if(!mScene) {
		throw std::runtime_error(StringBuilder("Unable to load scene from ") << file << "! Game will be closed.");
	}
}

void Level::CreateBlankScene() {
	mScene = mGame->GetEngine()->GetSceneFactory()->CreateSceneNode();
}

void Level::DoneInitialization() {
	mInitializationComplete = true;
}

void Level::GenericUpdate() {
	if(mPlayer) {
		mPlayer->Update();
	}

	for(auto io : mInteractiveObjectList) {
		io->Update();
	}
	for(auto kp : mKeypadList) {
		kp->Update();
	}
	for(auto pZone : mZoneList) {
		pZone->Update();
	}
	for(auto pButton : mButtonList) {
		pButton->Update();
	}
	for(auto lswitch : mLightSwitchList) {
		lswitch->Update();
	}


	if(mMusic) {
		mMusic->SetVolume(mGame->GetMenu()->GetMusicVolume());
	}

	--mChaseMusicStopInterval;
	if(mChaseMusicStopInterval < 0) {
		mDestChaseMusicVolume = 0.0f;
	}

	if(mChaseMusicVolume < 0.0f) {
		mChaseMusicVolume = 0.0f;
	}

	mChaseMusicVolume += (mDestChaseMusicVolume - mChaseMusicVolume) * 0.02f;
	mChaseMusic->SetVolume(mChaseMusicVolume);
}

void Level::AutoCreateDoorsByNamePattern(const string & namePattern) {
	std::regex rx(namePattern);
	for(int i = 0; i < mScene->GetCountChildren(); i++) {
		shared_ptr<ruSceneNode> child = mScene->GetChild(i);
		bool ignore = false;
		for(auto pDoor : mDoorList) {
			if(pDoor->mDoorNode == child) {
				ignore = true;
				break;
			}
		}
		if(!ignore) {
			if(regex_match(child->GetName(), rx)) {
				AddDoor(child->GetName(), 90);
			}
		}
	}
}

void Level::AddZone(const shared_ptr<Zone> & zone) {
	mZoneList.push_back(zone);
}

void Level::AddButton(const shared_ptr<Button> & button) {
	mButtonList.push_back(button);
}


// when enemy detects player, enemy should call this method

void Level::PlayChaseMusic() {
	if(!mChaseMusic->IsPlaying()) {
		mChaseMusic->Play();
	}
	mDestChaseMusicVolume = 1.0f;
	mChaseMusicStopInterval = 60 * 10; // 10 seconds (60 fps)
}

const unique_ptr<Enemy> & Level::GetEnemy() const {
	return mEnemy;
}

const unique_ptr<Player> & Level::GetPlayer() const {
	return mPlayer;
}

void Level::DestroyPlayer() {
	mPlayer.reset();
}

void Level::AddInteractiveObject(const string & desc, const shared_ptr<InteractiveObject> & io, const ruDelegate & interactAction) {
	io->OnInteract += interactAction;
	io->SetPickDescription(desc);
	mInteractiveObjectList.push_back(io);
}

shared_ptr<InteractiveObject> Level::FindInteractiveObject(const string & name) {
	for(auto & io : mInteractiveObjectList) {
		if(io->mObject->GetName() == name) {
			return io;
		}
	}
	return shared_ptr<InteractiveObject>(nullptr);
}
