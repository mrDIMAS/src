#pragma once 

#include "Common.h"
#include "Timer.h"

class Renderer
{
public:
  enum
  {
    TypeDeferredRenderer,
    TypeLightMapRenderer,
  };

  static LRESULT CALLBACK WindowProcess ( HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam );
  void UpdateMessagePump( );
  void RenderMeshesIntoGBuffer();
  void LightmapRenderMeshByGroups();
  void RenderWorld( );
  void CreatePhysics( );
  int CreateRenderWindow( int width, int height, int fullscreen );
  bool SortByTexture( Mesh * mesh1, Mesh * mesh2 );
  bool IsMeshVisible( Mesh * mesh );
  bool IsLightVisible( Light * lit );
  bool PointInBV( BoundingVolume bv, Vector3 point );

  Timer * performanceTimer;
public:
  Renderer( int width, int height, int fullscreen );
  ~Renderer();
};