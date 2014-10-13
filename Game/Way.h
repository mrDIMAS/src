#pragma once

#include "Game.h"
#include "TextFileStream.h"

class Way
{
protected:
  NodeHandle begin;
  NodeHandle end;
  NodeHandle enterZone;
  NodeHandle target;
  bool inside;
  bool entering;
  bool freeLook;

public:
  static vector<Way*> all;

  enum class Direction
  {
    Forward  = 1,
    Backward = 2,
  };

  Way( NodeHandle hBegin, NodeHandle hEnd, NodeHandle hEnterZone );
  virtual ~Way();
  bool IsEntering();
  bool IsPlayerInside();
  bool IsEnterPicked();
  NodeHandle GetEnterZone();
  NodeHandle GetTarget();
  bool IsFreeLook();
  void DoEntering();
  virtual void Enter();
  virtual void DoPlayerCrawling() = 0;  
  virtual void LookAtTarget() = 0;
  virtual void SetDirection( Direction direction ) = 0;
  static Way * GetByObject( NodeHandle obj );
  void SerializeWith( TextFileStream & out );
  void DeserializeWith( TextFileStream & in );
};