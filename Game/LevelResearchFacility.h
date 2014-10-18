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
    SoundHandle fanSound;    
    SoundHandle leverSound;
    SoundHandle steamHissSound;
    NodeHandle scaryBarellThrowZone;
    NodeHandle steamActivateZone;
    NodeHandle steamPS;
    NodeHandle doorOpenLever;
    NodeHandle lockedDoor;
    NodeHandle spawnRipperZone;
    NodeHandle repositionRipperZone;
    NodeHandle ripperNewPosition;
    NodeHandle extremeSteamBlock;
    NodeHandle extremeSteamHurtZone;
    NodeHandle upLight[6];
    NodeHandle fuseModel[3];
    NodeHandle powerLever;
    NodeHandle powerLeverOffModel;
    NodeHandle powerLeverOnModel;
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


