#pragma once

#include "Level.h"
#include "Lift.h"
#include "Valve.h"
#include "Enemy.h"
#include "ItemPlace.h"
#include "Fan.h"
#include "Sparks.h"
#include "SteamStream.h"
#include "Keypad.h"

class LevelResearchFacility : public Level {
private:
    Lift * mLift1;
	Lift * mLift2;
    Sparks * mpPowerSparks;
    Valve * mpSteamValve;
    SteamStream * mpExtemeSteam;
    Enemy * mpRipper;
    Fan * mpFan1;
    Fan * mpFan2;
    ruSoundHandle mFanSound;
    ruSoundHandle mLeverSound;
    ruSoundHandle mSteamHissSound;
    ruNodeHandle mZoneScaryBarellThrow;
    ruNodeHandle mZoneSteamActivate;
    ruNodeHandle mSteamPS;
    ruNodeHandle mDoorOpenLever;
    ruNodeHandle mLockedDoor;
    ruNodeHandle mExtremeSteamBlock;
    ruNodeHandle mZoneExtremeSteamHurt;
    ruNodeHandle fuseModel[3];
    ruNodeHandle powerLever;
    ruNodeHandle mPowerLeverOffModel;
    ruNodeHandle mPowerLeverOnModel;
    ruNodeHandle mScaryBarell;
    ruNodeHandle mScaryBarellPositionNode;
    ruNodeHandle mPowerLamp;
    ruNodeHandle mPowerLeverSnd;
    ruNodeHandle mSmallSteamPosition;
	ruNodeHandle mZoneObjectiveRestorePower;
	ruNodeHandle mZoneExaminePlace;
	ruNodeHandle mZoneNeedCrowbar;
	ruNodeHandle mDoorUnderFloor;
	ruNodeHandle mZoneNewLevelLoad;
	ruNodeHandle mZoneObjectiveNeedPassThroughMesh;
	ruNodeHandle mZoneRemovePathBlockingMesh;
	ruNodeHandle mPathBlockingMesh;

	Keypad * mKeypad1;
	Door * mKeypad1DoorToUnlock;

	Keypad * mKeypad2;
	Door * mKeypad2DoorToUnlock;

	Keypad * mKeypad3;
	Door * mKeypad3DoorToUnlock;

    Item * fuse[3];
	Item * mCrowbarItem;
    ItemPlace * mFusePlaceList[3];
    int fuseInsertedCount;
    bool mPowerOn;

    void CreatePowerUpSequence();
    void UpdatePowerupSequence();
public:
    explicit LevelResearchFacility();
    virtual ~LevelResearchFacility();
    virtual void DoScenario() final;
    virtual void Hide() final;
    virtual void Show() final;
    virtual void OnSerialize( TextFileStream & out ) final;
    virtual void OnDeserialize( TextFileStream & in ) final;
};


