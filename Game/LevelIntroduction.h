#pragma once

#include "Level.h"
#include "GUI.h"

class LevelIntroduction : public Level
{
public:
  string text;
  float textAlpha;
  float textAlphaTo;
  bool intro;
  TimerHandle textTimer;

  LevelIntroduction();
  ~LevelIntroduction();
  virtual void DoScenario();
  virtual void Show();
  virtual void Hide();

  virtual void OnSerialize( TextFileStream & out ) final
  {

  }

  virtual void OnDeserialize( TextFileStream & in ) final
  {

  }
};