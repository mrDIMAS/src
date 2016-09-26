#include "Precompiled.h"
#include "Keypad.h"
#include "LevelSewers.h"

LevelSewers::LevelSewers(const unique_ptr<PlayerTransfer> & playerTransfer) : Level(playerTransfer), mWaterFlow(0.0f), mVerticalWaterFlow(0.0f) {
	mTypeNum = 5;

	LoadLocalization("sewers.loc");

	LoadSceneFromFile("data/maps/sewers.scene");

	mPlayer->SetPosition(GetUniqueObject("PlayerPosition")->GetPosition());

	AddLadder("Ladder3Begin", "Ladder3End", "Ladder3Enter", "Ladder3BeginLeavePoint", "Ladder3EndLeavePoint");

	mGate1 = AddGate("SmallGate1", "Button1Open", "Button1Close", "Button1Open2", "Button1Close2");
	mGate2 = AddGate("SmallGate2", "Button2Open", "Button2Close", "Button2Open2", "Button2Close2");
	mGateToLift = AddGate("SmallGate3", "Button3Open", "Button3Close", "Button3Open2", "Button3Close2");

	AutoCreateLampsByNamePattern("Lamp?([[:digit:]]+)", "data/sounds/lamp_buzz.ogg");

	mZoneKnocks = GetUniqueObject("ZoneKnocks");

	mKnocksSound = ruSound::Load3D("data/sounds/knocks.ogg");
	mKnocksSound->SetPosition(mZoneKnocks->GetPosition());
	mKnocksSound->SetRolloffFactor(0.2f);
	mKnocksSound->SetRoomRolloffFactor(0.2f);

	ruSound::SetAudioReverb(10);

	AddSheet("Note1", mLocalization.GetString("note1Desc"), mLocalization.GetString("note1"));
	AddSheet("Note2", mLocalization.GetString("note2Desc"), mLocalization.GetString("note2"));
	AddSheet("Note3", mLocalization.GetString("note3Desc"), mLocalization.GetString("note3"));

	AddDoor("Door1", 90.0f);
	AddDoor("Door2", 90.0f);
	AddDoor("Door3", 90.0f);
	AddDoor("Door4", 90.0f);
	AddDoor("Door005", 90.0f);
	AddDoor("Door006", 90.0f);
	AddDoor("Door007", 90.0f);
	AddDoor("Door008", 90.0f);
	AddDoor("Door009", 90.0f);
	AddDoor("Door010", 90.0f);

	mDoorToControl = AddDoor("DoorToControl", 90.0f);
	mKeypad1 = AddKeypad("Keypad1", "Keypad1Key0", "Keypad1Key1", "Keypad1Key2", "Keypad1Key3", "Keypad1Key4", "Keypad1Key5", "Keypad1Key6", "Keypad1Key7", "Keypad1Key8", "Keypad1Key9", "Keypad1KeyCancel", mDoorToControl, "9632");

	mDoorToCode = AddDoor("DoorToCode", 90.0f);
	mKeypad2 = AddKeypad("Keypad2", "Keypad2Key0", "Keypad2Key1", "Keypad2Key2", "Keypad2Key3", "Keypad2Key4", "Keypad2Key5", "Keypad2Key6", "Keypad2Key7", "Keypad2Key8", "Keypad2Key9", "Keypad2KeyCancel", mDoorToCode, "5486");

	mEnemySpawnPosition = GetUniqueObject("EnemySpawnPosition");

	AddZone(mZoneEnemySpawn = make_shared<Zone>(GetUniqueObject("EnemySpawnZone")));
	mZoneEnemySpawn->OnPlayerEnter.AddListener(ruDelegate::Bind(this, &LevelSewers::OnPlayerEnterSpawnEnemyZone));

	mZoneDropWaterLevel = make_shared<Zone>(GetUniqueObject("ZoneDropWaterLevel"));
	mZoneDropWaterLevel->OnPlayerEnter.AddListener(ruDelegate::Bind(this, &LevelSewers::OnPlayerEnterDropWaterLevelZone));

	mWater = GetUniqueObject("Water");

	mStages["KnocksDone"] = false;
	mStages["EnemySpawned"] = false;

	mPassLightGreen = dynamic_pointer_cast<ruLight>(GetUniqueObject("PassLightGreen"));
	mPassLightRed = dynamic_pointer_cast<ruLight>(GetUniqueObject("PassLightRed"));

	mLift1 = AddLift("Lift1", "Lift1Screen", "Lift1Source", "Lift1Dest", "Lift1FrontDoor1", "Lift1FrontDoor2", "Lift1BackDoor1", "Lift1BackDoor2");

	std::regex rx("VerticalWater?([[:digit:]]+)");
	for (int i = 0; i < mScene->GetCountChildren(); i++) {
		shared_ptr<ruSceneNode> child = mScene->GetChild(i);
		if (regex_match(child->GetName(), rx)) {
			mVerticalWaterList.push_back(child);
		}
	}

	DoneInitialization();
}

void LevelSewers::CreateEnemy()
{
	// create paths
	Path wayNorth; BuildPath(wayNorth, "WayNorth");
	Path wayWest; BuildPath(wayWest, "WayWest");
	Path waySouthWest; BuildPath(waySouthWest, "WaySouthWest");
	Path wayBasement; BuildPath(wayBasement, "WayBasement");

	// add edges
	wayNorth.mVertexList[0]->AddEdge(wayWest.mVertexList[0]);
	wayNorth.mVertexList[0]->AddEdge(waySouthWest.mVertexList[0]);
	wayBasement.mVertexList[0]->AddEdge(waySouthWest.mVertexList.back());

	// concatenate paths
	vector<GraphVertex*> allPaths;
	allPaths.insert(allPaths.end(), wayNorth.mVertexList.begin(), wayNorth.mVertexList.end());
	allPaths.insert(allPaths.end(), wayWest.mVertexList.begin(), wayWest.mVertexList.end());
	allPaths.insert(allPaths.end(), waySouthWest.mVertexList.begin(), waySouthWest.mVertexList.end());
	allPaths.insert(allPaths.end(), wayBasement.mVertexList.begin(), wayBasement.mVertexList.end());

	// create patrol paths
	vector< GraphVertex* > patrolPoints;

	patrolPoints.push_back(wayNorth.mVertexList.front());
	patrolPoints.push_back(wayNorth.mVertexList.back());

	patrolPoints.push_back(wayWest.mVertexList.front());
	patrolPoints.push_back(wayWest.mVertexList.back());

	patrolPoints.push_back(waySouthWest.mVertexList.front());
	patrolPoints.push_back(waySouthWest.mVertexList.back());

	patrolPoints.push_back(wayBasement.mVertexList.front());
	patrolPoints.push_back(wayBasement.mVertexList.back());

	mEnemy = unique_ptr<Enemy>(new Enemy(allPaths, patrolPoints));
	mEnemy->SetPosition(mEnemySpawnPosition->GetPosition());
}


LevelSewers::~LevelSewers() {

}

void LevelSewers::DoScenario() {
	mWater->SetTexCoordFlow(ruVector2(0.0, -mWaterFlow));
	mWaterFlow += 0.00025f;

	mVerticalWaterFlow += 0.001;
	for (auto & pVW : mVerticalWaterList) {
		pVW->SetTexCoordFlow(ruVector2(0.0f, mVerticalWaterFlow));
	}

	if (mEnemy) {
		mEnemy->Think();
	}

	mLift1->Update();

	ruEngine::SetAmbientColor(ruVector3(9.5f / 255.0f, 9.5f / 255.0f, 9.5f / 255.0f));
	mGate1->Update();
	mGate2->Update();
	mGateToLift->Update();

	if (!mStages["KnocksDone"]) {
		if (mPlayer->IsInsideZone(mZoneKnocks)) {
			mKnocksSound->Play();
			mStages["KnocksDone"] = true;

			mPlayer->GetHUD()->SetObjective(mLocalization.GetString("objectiveFindCodeToPump"));
		}
	}
}

void LevelSewers::Show() {
	Level::Show();
}

void LevelSewers::Hide() {
	Level::Hide();
}

void LevelSewers::OnSerialize(SaveFile & s) {
	auto enemyPresented = mEnemy != nullptr;
	s & enemyPresented;
	
	if (enemyPresented) {
		if (s.IsLoading()) {
			CreateEnemy();
		}
		auto epos = mEnemy->GetBody()->GetPosition();
		s & epos;
		mEnemy->GetBody()->SetPosition(epos);
	}
}

