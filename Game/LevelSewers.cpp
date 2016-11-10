#include "Precompiled.h"
#include "Keypad.h"
#include "LevelSewers.h"

LevelSewers::LevelSewers(const unique_ptr<PlayerTransfer> & playerTransfer) : 
	Level(playerTransfer),
	mWaterFlow(0.0f), 
	mVerticalWaterFlow(0.0f),
	mDrainTimer(60 * 60)
{
	mName = LevelName::Sewers;

	LoadLocalization("sewers.loc");

	LoadSceneFromFile("data/maps/sewers.scene");

	mPlayer->SetPosition(GetUniqueObject("PlayerPosition")->GetPosition());

	AddLadder("Ladder3Begin", "Ladder3End", "Ladder3Enter", "Ladder3BeginLeavePoint", "Ladder3EndLeavePoint");

	mGate1 = AddGate("SmallGate1", "Button1Open", "Button1Close", "Button1Open2", "Button1Close2");
	mGate2 = AddGate("SmallGate2", "Button2Open", "Button2Close", "Button2Open2", "Button2Close2");
	mGateToLift = AddGate("SmallGate3", "Button3Open", "Button3Close", "Button3Open2", "Button3Close2");
	mGateToLift->mLocked = true;

	mZoneKnocks = GetUniqueObject("ZoneKnocks");

	mKnocksSound = ruSound::Load3D("data/sounds/knocks.ogg");
	mKnocksSound->SetPosition(mZoneKnocks->GetPosition());
	mKnocksSound->SetRolloffFactor(0.2f);
	mKnocksSound->SetRoomRolloffFactor(0.2f);

	ruSound::SetAudioReverb(10);

	AddInteractiveObject("Note", make_shared<InteractiveObject>(GetUniqueObject("Note1")), [this] { mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note1Desc"), mLocalization.GetString("note1")); });
	AddInteractiveObject("Note", make_shared<InteractiveObject>(GetUniqueObject("Note2")), [this] { mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note2Desc"), mLocalization.GetString("note2")); });
	AddInteractiveObject("Note", make_shared<InteractiveObject>(GetUniqueObject("Note3")), [this] { mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note3Desc"), mLocalization.GetString("note3")); });

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
	AddDoor("Door012", 90.0f);
	AddDoor("Door013", 90.0f);

	mDoorToControl = AddDoor("DoorToControl", 90.0f);
	mKeypad1 = AddKeypad("Keypad1", "Keypad1Key0", "Keypad1Key1", "Keypad1Key2", "Keypad1Key3", "Keypad1Key4", "Keypad1Key5", "Keypad1Key6", "Keypad1Key7", "Keypad1Key8", "Keypad1Key9", "Keypad1KeyCancel", mDoorToControl, "9632");

	mDoorToCode = AddDoor("DoorToCode", 90.0f);
	mKeypad2 = AddKeypad("Keypad2", "Keypad2Key0", "Keypad2Key1", "Keypad2Key2", "Keypad2Key3", "Keypad2Key4", "Keypad2Key5", "Keypad2Key6", "Keypad2Key7", "Keypad2Key8", "Keypad2Key9", "Keypad2KeyCancel", mDoorToCode, "5486");

	mEnemySpawnPosition = GetUniqueObject("EnemySpawnPosition");

	AddZone(make_shared<Zone>(GetUniqueObject("ZoneDropWaterLevel"), [this] { OnPlayerEnterDrainWaterLevelZone(); }));

	mWater = GetUniqueObject("Water");

	mStages["KnocksDone"] = false;
	mStages["EnemySpawned"] = false;
	mStages["WaterDrained"] = false;
	mStages["PumpsActivated"] = false;

	mPassLightGreen = dynamic_pointer_cast<ruLight>(GetUniqueObject("PassLightGreen"));
	mPassLightRed = dynamic_pointer_cast<ruLight>(GetUniqueObject("PassLightRed"));

	mPumpLight[0] = dynamic_pointer_cast<ruLight>(GetUniqueObject("PumpLight1"));
	mPumpLight[1] = dynamic_pointer_cast<ruLight>(GetUniqueObject("PumpLight2"));
	mPumpLight[2] = dynamic_pointer_cast<ruLight>(GetUniqueObject("PumpLight3"));

	{
		mLift1 = AddLift("Lift1", "Lift1Source", "Lift1Dest", "Lift1FrontDoor1", "Lift1FrontDoor2", "Lift1BackDoor1", "Lift1BackDoor2");
		// add go up button
		AddButton(make_shared<Button>(GetUniqueObject("Lift1GoUp"), "Go up", [this] { mLift1->GoUp(); }));
		// add go down button
		AddButton(make_shared<Button>(GetUniqueObject("Lift1GoDown"), "Go down", [this] { mLift1->GoDown(); }));
		// add call up button
		AddButton(make_shared<Button>(GetUniqueObject("Lift1CallUp"), "Call up", [this] { mLift1->GoUp(); }));
		// add call down button
		AddButton(make_shared<Button>(GetUniqueObject("Lift1CallDown"), "Call down", [this] { mLift1->GoDown(); }));
	}
	std::regex rx("VerticalWater?([[:digit:]]+)");
	for (int i = 0; i < mScene->GetCountChildren(); i++) {
		shared_ptr<ruSceneNode> child = mScene->GetChild(i);
		if (regex_match(child->GetName(), rx)) {
			mVerticalWaterList.push_back(child);
		}
	}

	for (int i = 0; i < 3; ++i) {
		AddSound(mWaterPumpSound[i] = ruSound::Load3D("data/sounds/waterpump.ogg"));
		mWaterPumpSound[i]->Attach(GetUniqueObject(StringBuilder("WaterPumpSound") << i + 1));
		mWaterPumpSound[i]->SetLoop(true);
	}

	mZoneNextLevel = GetUniqueObject("ZoneNextLevel");

	mPumpSwitch = GetUniqueObject("PumpSwitch");
	mPumpSwitchAnimation = ruAnimation(0, 100, 0.6, false);
	mPumpSwitch->SetAnimation(&mPumpSwitchAnimation);

	OnPlayerEnterSpawnEnemyZone();

	DoneInitialization();
}

void LevelSewers::CreateEnemy() {
	// create paths
	const char * ways[] = {
		"WayA", "WayB", "WayC", "WayD", "WayE", "WayF", "WayG", 
		"WayH", "WayI", "WayJ", "WayK", "WayL", "WayM"
	};
	Path p;
	for (auto w : ways) {
		p += Path(mScene, w);
	}

	// add egdes
	p.Get("WayA001")->AddEdge(p.Get("WayF1"));
	p.Get("WayB1")->AddEdge(p.Get("WayE1"));
	p.Get("WayF002")->AddEdge(p.Get("WayE002")); // bridge
	p.Get("WayA020")->AddEdge(p.Get("WayB026")); // bridge
	p.Get("WayA029")->AddEdge(p.Get("WayC025")); // bridge
	p.Get("WayC041")->AddEdge(p.Get("WayB041")); // bridge
	p.Get("WayA046")->AddEdge(p.Get("WayC008")); // bridge
	p.Get("WayD1")->AddEdge(p.Get("WayC068")); // bridge
	p.Get("WayA001")->AddEdge(p.Get("WayG1")); 
	p.Get("WayH012")->AddEdge(p.Get("WayG028"));
	p.Get("WayH1")->AddEdge(p.Get("WayG022"));
	p.Get("WayI1")->AddEdge(p.Get("WayG033"));
	p.Get("WayJ1")->AddEdge(p.Get("WayI011"));
	p.Get("WayK1")->AddEdge(p.Get("WayG040"));
	p.Get("WayA017")->AddEdge(p.Get("WayB021"));
	p.Get("WayA011")->AddEdge(p.Get("WayB012"));
	p.Get("WayA004")->AddEdge(p.Get("WayB005"));
	p.Get("WayF005")->AddEdge(p.Get("WayE005"));
	p.Get("WayC028")->AddEdge(p.Get("WayB029"));
	p.Get("WayC028")->AddEdge(p.Get("WayA024"));
	p.Get("WayC018")->AddEdge(p.Get("WayA035"));
	p.Get("WayC013")->AddEdge(p.Get("WayA041"));
	p.Get("WayC1")->AddEdge(p.Get("WayA050"));
	p.Get("WayC036")->AddEdge(p.Get("WayB037"));
	p.Get("WayC052")->AddEdge(p.Get("WayB046"));
	p.Get("WayC061")->AddEdge(p.Get("WayB055"));
	p.Get("WayC069")->AddEdge(p.Get("WayB063"));
	p.Get("WayH006")->AddEdge(p.Get("WayM1"));
	p.Get("WayB1")->AddEdge(p.Get("WayL1"));

	vector<shared_ptr<GraphVertex>> patrolPoints = {
		p.Get("WayF007"), p.Get("WayA050"), p.Get("WayF007"),
		p.Get("WayB064"), p.Get("WayD008"), p.Get("WayG046"),
		p.Get("WayH008"), p.Get("WayK006"), p.Get("WayI033"),
		p.Get("WayL012"), p.Get("WayM006")
	};

	mEnemy = unique_ptr<Enemy>(new Enemy(p.mVertexList, patrolPoints));
	mEnemy->SetPosition(mEnemySpawnPosition->GetPosition());
}

LevelSewers::~LevelSewers() {

}

void LevelSewers::DoScenario() {
	// animate water flow
	mWater->SetTexCoordFlow(ruVector2(0.0, -mWaterFlow));
	mWaterFlow += 0.00025f;

	mVerticalWaterFlow += 0.001;
	for (auto & pVW : mVerticalWaterList) {
		pVW->SetTexCoordFlow(ruVector2(0.0f, mVerticalWaterFlow));
	}

	// update enemy, if present
	if (mEnemy) {
		mEnemy->Think();
	}

	// indicate, that water is drained or not
	if (mStages["WaterDrained"]) {
		mPassLightGreen->Show();
		mPassLightRed->Hide();

		mGateToLift->mLocked = false;
	} else {
		mPassLightGreen->Hide();
		mPassLightRed->Show();

		if (!mStages["PumpsActivated"]) {
			if (mPlayer->mNearestPickedNode == mPumpSwitch) {
				mPlayer->GetHUD()->SetAction(mPlayer->mKeyUse, mLocalization.GetString("activatePumps"));

				if (ruInput::IsKeyHit(mPlayer->mKeyUse)) {
					mStages["PumpsActivated"] = true;
					mPumpSwitchAnimation.SetEnabled(true);

					if (mEnemy) {
						mEnemy->ForceCheckSound(mWaterPumpSound[0]->GetPosition());
					}
				}
			}
		} else {
			--mDrainTimer;
			if (mDrainTimer < 0) {
				mDrainTimer = 0;
				mStages["WaterDrained"] = true;
				for (int i = 0; i < 3; ++i) {
					mWaterPumpSound[i]->Stop();
					mPumpLight[i]->SetColor(ruVector3(255, 0, 0));
				}				
			} else {
				for (int i = 0; i < 3; ++i) {
					mWaterPumpSound[i]->Play();
					mPumpLight[i]->SetColor(ruVector3(0, 255, 0));
				}
			}
			mPlayer->GetHUD()->SetObjective(StringBuilder() << mLocalization.GetString("objectiveWaitDrain") << mDrainTimer / 60);
		}
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

	

	mPumpSwitchAnimation.Update();

	if (mPlayer->IsInsideZone(mZoneNextLevel)) {
		Level::Change(LevelName::Forest);
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

	s & mDrainTimer;

	if (enemyPresented) {
		if (s.IsLoading()) {
			CreateEnemy();
		}
		auto epos = mEnemy->GetBody()->GetPosition();
		s & epos;
		mEnemy->GetBody()->SetPosition(epos);
	}
}

