#pragma once

#include "Level.h"
#include "Lift.h"
#include "Valve.h"
#include "Enemy.h"
#include "ItemPlace.h"
#include "Fan.h"
#include "Sparks.h"
#include "SteamStream.h"

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
    ruNodeHandle mScaryBarellThrowZone;
    ruNodeHandle mSteamActivateZone;
    ruNodeHandle mSteamPS;
    ruNodeHandle mDoorOpenLever;
    ruNodeHandle mLockedDoor;
    ruNodeHandle mExtremeSteamBlock;
    ruNodeHandle mExtremeSteamHurtZone;
    ruNodeHandle fuseModel[3];
    ruNodeHandle powerLever;
    ruNodeHandle mPowerLeverOffModel;
    ruNodeHandle mPowerLeverOnModel;
    ruNodeHandle mScaryBarell;
    ruNodeHandle mScaryBarellPositionNode;
    ruNodeHandle mPowerLamp;
    ruNodeHandle mPowerLeverSnd;
    ruNodeHandle mSmallSteamPosition;

    Item * fuse[3];
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


