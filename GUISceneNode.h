#pragma once

#include "Common.h"

class GUISceneNode
{
public:
  float x, y, w, h;
  bool visible;

  GUISceneNode * parent;

  static vector< GUISceneNode* > all;

  explicit GUISceneNode() : x( 0.0f ), y( 0.0f ), w( 0.0f ), h( 0.0f ), visible( true ), parent( nullptr )
  {
    all.push_back( this );
  }

  virtual ~GUISceneNode()
  {

  }

  virtual void Update()
  {

  }

  static void UpdateAll()
  {
    for( auto node : all )
      node->Update();
  }
};