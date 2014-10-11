#include "AABB.h"




void AABB::Visualize()
{
  Vector3 red = Vector3( 255, 0, 0 );
  Draw3DLine( LinePoint( Vector3( min.x,min.y,min.z ), red ), LinePoint( Vector3( min.x,min.y,max.z ), red ));
  Draw3DLine( LinePoint( Vector3( min.x,min.y,max.z ), red ), LinePoint( Vector3( max.x,min.y,max.z ), red ));
  Draw3DLine( LinePoint( Vector3( max.x,min.y,max.z ), red ), LinePoint( Vector3( max.x,min.y,min.z ), red ));
  Draw3DLine( LinePoint( Vector3( max.x,min.y,min.z ), red ), LinePoint( Vector3( min.x,min.y,min.z ), red ));
  Draw3DLine( LinePoint( Vector3( min.x,max.y,min.z ), red ), LinePoint( Vector3( min.x,max.y,max.z ), red ));
  Draw3DLine( LinePoint( Vector3( min.x,max.y,max.z ), red ), LinePoint( Vector3( max.x,max.y,max.z ), red ));
  Draw3DLine( LinePoint( Vector3( max.x,max.y,max.z ), red ), LinePoint( Vector3( max.x,max.y,min.z ), red ));
  Draw3DLine( LinePoint( Vector3( max.x,max.y,min.z ), red ), LinePoint( Vector3( min.x,max.y,min.z ), red ));
  Draw3DLine( LinePoint( Vector3( min.x,min.y,min.z ), red ), LinePoint( Vector3( min.x,max.y,min.z ), red ));
  Draw3DLine( LinePoint( Vector3( min.x,min.y,max.z ), red ), LinePoint( Vector3( min.x,max.y,max.z ), red ));
  Draw3DLine( LinePoint( Vector3( max.x,min.y,max.z ), red ), LinePoint( Vector3( max.x,max.y,max.z ), red ));
  Draw3DLine( LinePoint( Vector3( max.x,min.y,min.z ), red ), LinePoint( Vector3( max.x,max.y,min.z ), red ));
}

bool AABB::IsTriangleInside( const Vector3 & v1, const Vector3 & v2, const Vector3 & v3 )
{
  return IsPointInside( v1 ) || IsPointInside( v2 ) || IsPointInside( v3 );
}

bool AABB::IsPointInside( const Vector3 & v )
{
  return  v.x >= min.x && v.x <= max.x &&
    v.y >= min.y && v.y <= max.y &&
    v.z >= min.z && v.z <= max.z ;
}

AABB::AABB( const Vector3 & min, const Vector3 & max )
{
  this->min = min;
  this->max = max;

  vertices[0] = min;
  vertices[1] = Vector3( min.x, min.y, max.z );
  vertices[2] = Vector3( max.x, min.y, max.z );
  vertices[3] = Vector3( max.x, min.y, min.z );

  vertices[4] = Vector3( min.x, max.y, min.z );
  vertices[5] = Vector3( min.x, max.y, max.z );
  vertices[6] = max;
  vertices[7] = Vector3( max.x, max.y, min.z );
}

AABB::AABB()
{

}
