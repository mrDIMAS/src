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
    Lift * lift1;
    Sparks * powerSparks;  
    Valve * steamValve;
    SteamStream * extemeSteam;
    Enemy * ripper;
    Fan * fan1;    
    Fan * fan2;
    ruSoundHandle fanSound;    
    ruSoundHandle leverSound;
    ruSoundHandle steamHissSound;
    ruNodeHandle scaryBarellThrowZone;
    ruNodeHandle steamActivateZone;
    ruNodeHandle steamPS;
    ruNodeHandle doorOpenLever;
    ruNodeHandle lockedDoor;
    ruNodeHandle spawnRipperZone;
    ruNodeHandle repositionRipperZone;
    ruNodeHandle ripperNewPosition;
    ruNodeHandle extremeSteamBlock;
    ruNodeHandle extremeSteamHurtZone;
    ruNodeHandle upLight[6];
    ruNodeHandle fuseModel[3];
    ruNodeHandle powerLever;
    ruNodeHandle powerLeverOffModel;
    ruNodeHandle powerLeverOnModel;
    Item * fuse[3];
    ItemPlace * fusePlace[3];
    float upLightOnRange[6];
    int fuseInsertedCount;    
    bool powerOn;

    void CreateSteam();
    int CreateSparks( int at );   
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


