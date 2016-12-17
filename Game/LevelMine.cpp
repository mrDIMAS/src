#include "Precompiled.h"

#include "LevelMine.h"
#include "Player.h"
#include "Pathfinder.h"
#include "Utils.h"

LevelMine::LevelMine(unique_ptr<Game> & game, const unique_ptr<PlayerTransfer> & playerTransfer) : Level(game, playerTransfer) {
	mName = LevelName::Mine;

	LoadLocalization("mine.loc");

	mPlayer->mYaw.SetTarget(180.0f);

	LoadSceneFromFile("data/maps/mine.scene");

	mPlayer->SetPosition(GetUniqueObject("PlayerPosition")->GetPosition());

	Vector3 placePos = GetUniqueObject("PlayerPosition")->GetPosition();
	Vector3 playerPos = mPlayer->GetCurrentPosition();

	mPlayer->GetHUD()->SetObjective(mLocalization.GetString("objective1"));

	AddInteractiveObject("Note", make_shared<InteractiveObject>(GetUniqueObject("Note1")), [this] { mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note1Desc"), mLocalization.GetString("note1")); });
	AddInteractiveObject("Note", make_shared<InteractiveObject>(GetUniqueObject("Note2")), [this] { mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note2Desc"), mLocalization.GetString("note2")); });
	AddInteractiveObject("Note", make_shared<InteractiveObject>(GetUniqueObject("Note3")), [this] { mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note3Desc"), mLocalization.GetString("note3")); });
	AddInteractiveObject("Note", make_shared<InteractiveObject>(GetUniqueObject("Note4")), [this] { mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note4Desc"), mLocalization.GetString("note4")); });
	AddInteractiveObject("Note", make_shared<InteractiveObject>(GetUniqueObject("Note5")), [this] { mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note5Desc"), mLocalization.GetString("note5")); });
	AddInteractiveObject("Note", make_shared<InteractiveObject>(GetUniqueObject("Note6")), [this] { mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note6Desc"), mLocalization.GetString("note6")); });
	AddInteractiveObject("Note", make_shared<InteractiveObject>(GetUniqueObject("Note7")), [this] { mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note7Desc"), mLocalization.GetString("note7")); });
	AddInteractiveObject("Note", make_shared<InteractiveObject>(GetUniqueObject("Note8")), [this] { mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note8Desc"), mLocalization.GetString("note8")); });

	mStoneFallZone = GetUniqueObject("StoneFallZone");

	mNewLevelZone = GetUniqueObject("NewLevel");

	auto soundSystem = mGame->GetEngine()->GetSoundSystem();

	soundSystem->SetReverbPreset(ReverbPreset::Cave);

	AddSound(mMusic = soundSystem->LoadMusic("data/music/chapter2.ogg"));

	mConcreteWall = GetUniqueObject("ConcreteWall");
	mDeathZone = GetUniqueObject("DeadZone");
	mDetonator = GetUniqueObject("Detonator");

	AddSound(mAlertSound = soundSystem->LoadSound3D("data/sounds/alert.ogg"));
	mAlertSound->Attach(mDetonator);

	AddSound(mExplosionSound = soundSystem->LoadSound3D("data/sounds/blast.ogg"));
	mExplosionSound->SetReferenceDistance(10);

	mDetonatorActivated = 0;

	mExplosionFlashAnimator = 0;

	// Create detonator places
	AddItemPlace(mDetonatorPlace[0] = make_shared<ItemPlace>(GetUniqueObject("DetonatorPlace1"), Item::Type::Explosives));
	AddItemPlace(mDetonatorPlace[1] = make_shared<ItemPlace>(GetUniqueObject("DetonatorPlace2"), Item::Type::Explosives));
	AddItemPlace(mDetonatorPlace[2] = make_shared<ItemPlace>(GetUniqueObject("DetonatorPlace3"), Item::Type::Explosives));
	AddItemPlace(mDetonatorPlace[3] = make_shared<ItemPlace>(GetUniqueObject("DetonatorPlace4"), Item::Type::Explosives));

	mWireModels[0] = GetUniqueObject("WireModel1");
	mWireModels[1] = GetUniqueObject("WireModel2");
	mWireModels[2] = GetUniqueObject("WireModel3");
	mWireModels[3] = GetUniqueObject("WireModel4");

	mDetonatorModels[0] = GetUniqueObject("DetonatorModel1");
	mDetonatorModels[1] = GetUniqueObject("DetonatorModel2");
	mDetonatorModels[2] = GetUniqueObject("DetonatorModel3");
	mDetonatorModels[3] = GetUniqueObject("DetonatorModel4");

	mExplosivesModels[0] = GetUniqueObject("ExplosivesModel1");
	mExplosivesModels[1] = GetUniqueObject("ExplosivesModel2");
	mExplosivesModels[2] = GetUniqueObject("ExplosivesModel3");
	mExplosivesModels[3] = GetUniqueObject("ExplosivesModel4");

	mFindItemsZone = GetUniqueObject("FindItemsZone");

	AddAmbientSound(soundSystem->LoadSound3D("data/sounds/ambient/mine/ambientmine1.ogg"));
	AddAmbientSound(soundSystem->LoadSound3D("data/sounds/ambient/mine/ambientmine2.ogg"));
	AddAmbientSound(soundSystem->LoadSound3D("data/sounds/ambient/mine/ambientmine3.ogg"));
	AddAmbientSound(soundSystem->LoadSound3D("data/sounds/ambient/mine/ambientmine4.ogg"));
	AddAmbientSound(soundSystem->LoadSound3D("data/sounds/ambient/mine/ambientmine5.ogg"));

	mExplosionTimer = ITimer::Create();
	mBeepSoundTimer = ITimer::Create();
	mBeepSoundTiming = 1.0f;

	CreateItems();

	mReadyExplosivesCount = 0;

	MakeLadder("LadderBegin", "LadderEnd", "LadderEnter", "LadderBeginLeavePoint", "LadderEndLeavePoint");
	MakeDoor("Door1", 90);
	MakeDoor("Door3", 90);
	MakeDoor("DoorToAdministration", 90);
	MakeDoor("Door6", 90);
	MakeDoor("DoorToDirectorsOffice", 90);
	MakeDoor("DoorToResearchFacility", 90);

	MakeKeypad("Keypad1", "Keypad1Key0", "Keypad1Key1", "Keypad1Key2", "Keypad1Key3", "Keypad1Key4", "Keypad1Key5", "Keypad1Key6", "Keypad1Key7", "Keypad1Key8", "Keypad1Key9", "Keypad1KeyCancel", MakeDoor("StorageDoor", 90), "7854");
	MakeKeypad("Keypad2", "Keypad2Key0", "Keypad2Key1", "Keypad2Key2", "Keypad2Key3", "Keypad2Key4", "Keypad2Key5", "Keypad2Key6", "Keypad2Key7", "Keypad2Key8", "Keypad2Key9", "Keypad2KeyCancel", MakeDoor("DoorToResearchFacility", 90), "1689");
	MakeKeypad("Keypad3", "Keypad3Key0", "Keypad3Key1", "Keypad3Key2", "Keypad3Key3", "Keypad3Key4", "Keypad3Key5", "Keypad3Key6", "Keypad3Key7", "Keypad3Key8", "Keypad3Key9", "Keypad3KeyCancel", MakeDoor("DoorMedical", 90), "9632");

	mMusic->Play();

	mStages["ConcreteWallExp"] = false;
	mStages["FindObjectObjectiveSet"] = false;
	mStages["FoundObjectsForExplosion"] = false;

	// create paths
	const char * ways[] = {
		"WayA", "WayB", "WayC", "WayD", "WayE", "WayF", "WayG",
		"WayH", "WayI", "WayJ", "WayK"
	};
	Path p;
	for(auto w : ways) {
		p += Path(mScene, w);
	}

	p.Get("WayB1")->AddEdge(p.Get("WayA004"));
	p.Get("WayC1")->AddEdge(p.Get("WayA019"));
	p.Get("WayD1")->AddEdge(p.Get("WayA019"));
	p.Get("WayE1")->AddEdge(p.Get("WayA040"));
	p.Get("WayF1")->AddEdge(p.Get("WayA042"));
	p.Get("WayF009")->AddEdge(p.Get("WayG1"));
	p.Get("WayH1")->AddEdge(p.Get("WayA059"));
	p.Get("WayI1")->AddEdge(p.Get("WayA097"));
	p.Get("WayJ1")->AddEdge(p.Get("WayA073"));
	p.Get("WayK1")->AddEdge(p.Get("WayA027"));

	vector<shared_ptr<GraphVertex>> patrolPoints = {
		p.Get("WayA1"), p.Get("WayC024"), p.Get("WayB012"),
		p.Get("WayB012"), p.Get("WayD003"), p.Get("WayK005"),
		p.Get("WayE006"), p.Get("WayF019"), p.Get("WayG007"),
		p.Get("WayH010"), p.Get("WayA110"), p.Get("WayI009")
	};

	mEnemy = make_unique<Enemy>(mGame, p.mVertexList, patrolPoints);
	mEnemy->SetPosition(GetUniqueObject("EnemyPosition")->GetPosition());

	mExplosivesDummy[0] = GetUniqueObject("ExplosivesModel5");
	mExplosivesDummy[1] = GetUniqueObject("ExplosivesModel6");
	mExplosivesDummy[2] = GetUniqueObject("ExplosivesModel7");
	mExplosivesDummy[3] = GetUniqueObject("ExplosivesModel8");

	mExplodedWall = GetUniqueObject("ConcreteWallExploded");
	mExplodedWall->Hide();

	mExplosionFlashPosition = GetUniqueObject("ExplosionFlash");

	mPlayer->GetInventory()->RemoveItem(Item::Type::Crowbar, 1);

	DoneInitialization();
}

LevelMine::~LevelMine() {

}

void LevelMine::Show() {
	Level::Show();
}

void LevelMine::Hide() {
	Level::Hide();

	mMusic->Pause();
}

void LevelMine::DoScenario() {
	mMusic->Play();
	mEnemy->Think();
	mGame->GetEngine()->GetRenderer()->SetAmbientColor(Vector3(0.05, 0.05, 0.05));
	PlayAmbientSounds();


	if(!mStages["FindObjectObjectiveSet"]) {
		if(!mStages["FoundObjectsForExplosion"]) {
			if(mPlayer->IsInsideZone(mFindItemsZone)) {
				mPlayer->GetHUD()->SetObjective(mLocalization.GetString("objective2"));

				mStages["FindObjectObjectiveSet"] = true;
			}
		} else {
			mStages["FindObjectObjectiveSet"] = true;
		}
	}

	if(mExplosionFlashAnimator) {
		mExplosionFlashAnimator->Update();
	}

	if(!mStages["ConcreteWallExp"]) {
		if(mPlayer->mNearestPickedNode == mDetonator) {
			mPlayer->GetHUD()->SetAction(mPlayer->mKeyUse, mLocalization.GetString("detonator"));

			if(mGame->GetEngine()->GetInput()->IsKeyHit(mPlayer->mKeyUse) && mReadyExplosivesCount >= 4 && !mDetonatorActivated) {
				mDetonatorActivated = true;

				mExplosionTimer->Restart();
			}
		}

		if(mDetonatorActivated) {
			if(mExplosionTimer->GetElapsedTimeInSeconds() >= 10.0f) {
				mDetonatorActivated = false;

				mExplosionSound->Play();

				mExplosionSound->SetPosition(mConcreteWall->GetPosition());

				mStages["ConcreteWallExp"] = true;

				mConcreteWall->SetPosition(Vector3(10000, 10000, 10000));

				CleanUpExplodeArea();

				Vector3 vec = (mConcreteWall->GetPosition() - mPlayer->GetCurrentPosition()).Normalize() * 20;

				mExplodedWall->Show();

				mExplosionFlashLight = mGame->GetEngine()->GetSceneFactory()->CreatePointLight();
				mExplosionFlashLight->Attach(mExplosionFlashPosition);
				mExplosionFlashLight->SetColor(Vector3(255, 200, 160));
				mExplosionFlashAnimator = make_unique<LightAnimator>(mExplosionFlashLight, 0.25, 30, 1.1);
				mExplosionFlashAnimator->SetAnimationType(LightAnimator::AnimationType::Off);

				// dust
				mExplosionDustParticleSystem = mGame->GetEngine()->GetSceneFactory()->CreateParticleSystem(400);
				mExplosionDustParticleSystem->SetPosition(mExplosivesDummy[0]->GetPosition() - Vector3(0, 2.5, 0));
				mExplosionDustParticleSystem->SetTexture(mGame->GetEngine()->GetRenderer()->GetTexture("data/textures/particles/p1.png"));
				mExplosionDustParticleSystem->SetType(IParticleSystem::Type::Box);
				mExplosionDustParticleSystem->SetLightingEnabled(false);
				mExplosionDustParticleSystem->SetAutoResurrection(false);
				mExplosionDustParticleSystem->SetPointSize(0.8f);
				mExplosionDustParticleSystem->SetSpeedDeviation(Vector3(-0.0008, 0.0, -0.0008), Vector3(0.0008, 0.005, 0.0008));
				mExplosionDustParticleSystem->SetBoundingBox(Vector3(-1, 0, -3), Vector3(1, 3, 3));
				mExplosionDustParticleSystem->SetColorRange(Vector3(130, 130, 130), Vector3(150, 150, 150));

				if(mPlayer->IsInsideZone(mDeathZone)) {
					mPlayer->Damage(1000);
				}
			}

			if(mBeepSoundTimer->GetElapsedTimeInSeconds() > mBeepSoundTiming) { // every 1 sec
				mBeepSoundTiming -= 0.05f;
				mBeepSoundTimer->Restart();
				mAlertSound->Play(false);
			}
		}
	}

	UpdateExplodeSequence();

	if(mPlayer->IsInsideZone(mNewLevelZone)) {
		mGame->LoadLevel(LevelName::ResearchFacility);
	}
}


void LevelMine::UpdateExplodeSequence() {
	if(mReadyExplosivesCount < 4) {
		mReadyExplosivesCount = 0;

		for(int i = 0; i < 4; i++) {
			shared_ptr<ItemPlace> dp = mDetonatorPlace[i];

			if(dp->GetPlaceType() == Item::Type::Unknown) {
				mReadyExplosivesCount++;
			}

			if(mReadyExplosivesCount >= 4) {
				mPlayer->GetHUD()->SetObjective(mLocalization.GetString("objective4"));
			}
		}
	}

	static int totalNeededObjects = 0;
	if(totalNeededObjects < 12) {
		totalNeededObjects = 0;
		totalNeededObjects += mPlayer->GetInventory()->GetItemCount(Item::Type::Wires);
		totalNeededObjects += mPlayer->GetInventory()->GetItemCount(Item::Type::Explosives);
		totalNeededObjects += mPlayer->GetInventory()->GetItemCount(Item::Type::Detonator);
		if(totalNeededObjects >= 12) {
			mStages["FindObjectObjectiveSet"] = true;
			mPlayer->GetHUD()->SetObjective(mLocalization.GetString("objective3"));
		}
	}

	if(mPlayer->GetInventory()->GetItemSelectedForUse()) {
		for(int i = 0; i < 4; i++) {
			shared_ptr<ItemPlace> dp = mDetonatorPlace[i];

			if(dp->IsPickedByPlayer()) {
				mPlayer->GetHUD()->SetAction(mPlayer->mKeyUse, mPlayer->mLocalization.GetString("putItem"));
			}
		}

		if(mGame->GetEngine()->GetInput()->IsKeyHit(mPlayer->mKeyUse)) {
			for(int i = 0; i < 4; i++) {
				shared_ptr<ItemPlace> dp = mDetonatorPlace[i];

				if(dp->IsPickedByPlayer()) {
					bool placed = dp->PlaceItem(mPlayer->GetInventory()->GetItemSelectedForUse()->GetType());

					if(placed) {
						// 1st: Explosives
						// 2nd: Detonator
						// 3rd: Wires
						// 4th: Ready to explode
						if(dp->GetPlaceType() == Item::Type::Explosives) {
							mExplosivesModels[i]->Show();
							dp->SetPlaceType(Item::Type::Detonator);
						} else if(dp->GetPlaceType() == Item::Type::Detonator) {
							mDetonatorModels[i]->Show();
							dp->SetPlaceType(Item::Type::Wires);
						} else if(dp->GetPlaceType() == Item::Type::Wires) {
							mWireModels[i]->Show();
							dp->SetPlaceType(Item::Type::Unknown);
						}
					}
				}
			}
		}
	}
}

void LevelMine::CleanUpExplodeArea() {
	for(int i = 0; i < 4; i++) {
		mDetonatorPlace[i]->mObject->SetPosition(Vector3(1000, 1000, 1000));
		mWireModels[i]->Hide();
		mExplosivesModels[i]->Hide();
		mDetonatorModels[i]->Hide();
		mExplosivesDummy[i]->Hide();
	}
}

void LevelMine::CreateItems() {
	// Create explosives
	AddInteractiveObject(Item::GetNameByType(Item::Type::Explosives), make_shared<InteractiveObject>(GetUniqueObject("Explosives1")), [this] { mPlayer->AddItem(Item::Type::Explosives); });
	AddInteractiveObject(Item::GetNameByType(Item::Type::Explosives), make_shared<InteractiveObject>(GetUniqueObject("Explosives2")), [this] { mPlayer->AddItem(Item::Type::Explosives); });
	AddInteractiveObject(Item::GetNameByType(Item::Type::Explosives), make_shared<InteractiveObject>(GetUniqueObject("Explosives3")), [this] { mPlayer->AddItem(Item::Type::Explosives); });
	AddInteractiveObject(Item::GetNameByType(Item::Type::Explosives), make_shared<InteractiveObject>(GetUniqueObject("Explosives4")), [this] { mPlayer->AddItem(Item::Type::Explosives); });

	// Create detonators
	AddInteractiveObject(Item::GetNameByType(Item::Type::Detonator), make_shared<InteractiveObject>(GetUniqueObject("Detonator1")), [this] { mPlayer->AddItem(Item::Type::Detonator); });
	AddInteractiveObject(Item::GetNameByType(Item::Type::Detonator), make_shared<InteractiveObject>(GetUniqueObject("Detonator2")), [this] { mPlayer->AddItem(Item::Type::Detonator); });
	AddInteractiveObject(Item::GetNameByType(Item::Type::Detonator), make_shared<InteractiveObject>(GetUniqueObject("Detonator3")), [this] { mPlayer->AddItem(Item::Type::Detonator); });
	AddInteractiveObject(Item::GetNameByType(Item::Type::Detonator), make_shared<InteractiveObject>(GetUniqueObject("Detonator4")), [this] { mPlayer->AddItem(Item::Type::Detonator); });

	// Create wires
	AddInteractiveObject(Item::GetNameByType(Item::Type::Wires), make_shared<InteractiveObject>(GetUniqueObject("Wire1")), [this] { mPlayer->AddItem(Item::Type::Wires); });
	AddInteractiveObject(Item::GetNameByType(Item::Type::Wires), make_shared<InteractiveObject>(GetUniqueObject("Wire2")), [this] { mPlayer->AddItem(Item::Type::Wires); });
	AddInteractiveObject(Item::GetNameByType(Item::Type::Wires), make_shared<InteractiveObject>(GetUniqueObject("Wire3")), [this] { mPlayer->AddItem(Item::Type::Wires); });
	AddInteractiveObject(Item::GetNameByType(Item::Type::Wires), make_shared<InteractiveObject>(GetUniqueObject("Wire4")), [this] { mPlayer->AddItem(Item::Type::Wires); });
}

void LevelMine::OnSerialize(SaveFile & s) {
	s & mDetonatorActivated;
	s & mBeepSoundTiming;
	mEnemy->Serialize(s);
}
