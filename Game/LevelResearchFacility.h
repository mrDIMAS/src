#pragma once

#include "Level.h"
#include "Lift.h"
#include "Valve.h"
#include "Enemy.h"
#include "ItemPlace.h"
#include "Fan.h"
#include "Sparks.h"
#include "SteamStream.h"



class LevelResearchFacility : public Level
{
private:
  NodeHandle steamActivateZone;
  NodeHandle steamPS;
  SoundHandle steamHissSound;
  Lift * lift1;
  SoundHandle fanSound;
  NodeHandle scaryBarellThrowZone;
  SoundHandle leverSound;
  Sparks * powerSparks;
  Fan * fan1;
  Fan * fan2;
  Valve * steamValve;
  SteamStream * extemeSteam;

  Enemy * ripper;

  // Door open
  NodeHandle doorOpenLever;
  NodeHandle lockedDoor;
  NodeHandle spawnRipperZone;
  NodeHandle repositionRipperZone;
  NodeHandle ripperNewPosition;

  NodeHandle extremeSteamBlock;
  NodeHandle extremeSteamHurtZone;
public:
  LevelResearchFacility();
  ~LevelResearchFacility();
  virtual void DoScenario();
  virtual void Hide();
  virtual void Show();
  void CreateSteam();
  int CreateSparks( int at );

  Item * fuse[3];
  ItemPlace * fusePlace[3]; 
  NodeHandle fuseModel[3];
  NodeHandle powerLever;
  NodeHandle powerLeverOffModel;
  NodeHandle powerLeverOnModel;
  int fuseInsertedCount;
  bool powerOn;

  NodeHandle upLight[6];
  float upLightOnRange[6];

  void CreatePowerUpSequence();
  void UpdatePowerupSequence();

  virtual void OnSerialize( TextFileStream & out ) final
  {

  }

  virtual void OnDeserialize( TextFileStream & in ) final
  {

  }
};


