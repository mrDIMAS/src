#pragma once

#include "LightAnimator.h"
#include "Level.h"
#include "Enemy.h"
#include "ItemPlace.h"
#include "AmbientSoundSet.h"

class LevelMine : public Level
{
public:
  LevelMine( );
  ~LevelMine();

  virtual void DoScenario();
  virtual void Show();
  virtual void Hide();

  void UpdateExplodeSequence();
  void CleanUpExplodeArea();

  AmbientSoundSet ambSoundSet;
  void CreateItems();

  NodeHandle screamerZone;
  NodeHandle screamerZone2;

  NodeHandle concreteWall;
  NodeHandle deathZone;
  NodeHandle detonator;

  NodeHandle findItemsZone;

  NodeHandle newLevelZone;

  NodeHandle stoneFallZone;

  bool detonatorActivated;

  SoundHandle alertSound;
  SoundHandle explosionSound;

  // Item places
  ItemPlace * detonatorPlace[4];

  // Create items
  Item * explosives[4];
  Item * detonators[4];
  Item * wires[4];

  Item * fuel[2];

  NodeHandle wireModels[4];
  NodeHandle detonatorModels[4];
  NodeHandle explosivesModels[4];

  TimerHandle explosionTimer;
  TimerHandle beepSoundTimer;

  float beepSoundTiming;

  int readyExplosivesCount;

  LightAnimator * explosionFlashAnimator;

  virtual void SerializeWith( TextFileStream & out ) final
  {
    out.WriteBoolean( detonatorActivated );
    out.WriteFloat( beepSoundTiming );
  }

  virtual void DeserializeWith( TextFileStream & in ) final
  {
    in.ReadBoolean( detonatorActivated );
    in.ReadFloat( beepSoundTiming );    
  }
};