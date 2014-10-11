#pragma once

#include "Game.h"

class Flashlight
{
public:
  NodeHandle light;
  NodeHandle model;

  float maxCharge;
  float charge;
  float onRange;

  float realRange;
  float rangeDest;

  SoundHandle onSound;
  SoundHandle offSound;
  SoundHandle outOfChargeSound;

  Vector3 initialPosition;

  Vector3 position;
  Vector3 destPosition;

  TimerHandle chargeTimer;

  float chargeWorkTimeSeconds;

  bool on;

  float offTime;

  Flashlight( );

  bool GotCharge();

  void Fuel();

  void Attach( NodeHandle node );

  void SwitchOff();

  void SwitchOn();

  void Switch();

  void Update();
};