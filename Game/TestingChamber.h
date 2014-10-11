#pragma once

#include "Level.h"
#include "Enemy.h"
class TestingChamber : public Level
{
public:
  TestingChamber( );
  ~TestingChamber();

  Enemy * enemy;
  virtual void DoScenario();
  virtual void Show();
  virtual void Hide();
};