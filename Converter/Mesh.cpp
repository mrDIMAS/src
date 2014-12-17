#include "Mesh.h"
#include "Vector.h"

Mesh::Mesh()
{

}

void Mesh::AddVertex( const Vertex & v, const Weight & w )
{
  int vertexIndex   = 0;
  bool foundIdentic = false;

  auto begin = vertices.rbegin();
  auto end = vertices.rend();
  int  i = vertices.size() - 1;

  for( auto iter = begin; iter != end; ++iter, i-- )
  {
    Vertex & vertex = *iter;

    if( v.pos == vertex.pos && v.tex == vertex.tex )
    {
      vertexIndex = i;

      foundIdentic = true;

      break;
    }
  };

  if ( foundIdentic == false )
  {
    vertexIndex = vertices.size();

    vertices.push_back( v );   
    weights.push_back( w );
  }

  indices.push_back ( vertexIndex );
}

void Mesh::AddVertex( const Vertex & v )
{
  int vertexIndex   = 0;
  bool foundIdentic = false;

  auto begin = vertices.rbegin();
  auto end = vertices.rend();
  int  i = vertices.size() - 1;

  for( auto iter = begin; iter != end; ++iter, i-- )
  {
    Vertex & vertex = *iter;

    bool equalNormals = v.nor.x == vertex.nor.x && v.nor.y == vertex.nor.y && v.nor.z == vertex.nor.z;

    if( v.pos == vertex.pos && v.tex == vertex.tex && equalNormals )
    {
      vertexIndex = i;

      foundIdentic = true;

      break;
    }
  };

  if ( foundIdentic == false )
  {
    vertexIndex = vertices.size();

    vertices.push_back( v );   
  }

  indices.push_back ( vertexIndex );
}

void Mesh::CalculateAABB( )
{
  max = Vector3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
  min = Vector3 ( FLT_MAX,  FLT_MAX,  FLT_MAX );

  for( size_t i = 0; i < vertices.size(); i++ )
  {
    Vertex & v = vertices[i];

    float x = (*v.pos).x;
    float y = (*v.pos).y;
    float z = (*v.pos).z;

    if ( x > max.x )
      max.x = x;
    if ( y > max.y )
      max.y = y;
    if ( z > max.z )
      max.z = z;

    if ( x < min.x )
      min.x = x;
    if ( y < min.y )
      min.y = y;
    if ( z < min.z )
      min.z = z;
  }

  center.x = ( max.x + min.x ) / 2;
  center.y = ( max.y + min.y ) / 2;
  center.z = ( max.z + min.z ) / 2;

  Vector3 delta;
  delta.x = max.x - min.x;
  delta.y = max.y - min.y;
  delta.z = max.z - min.z;

  radius = sqrtf( delta.x * delta.x + delta.y * delta.y + delta.z * delta.z ) / 2;
};

void Mesh::CalculateNormals( )
{
  Vector3 * tempNormals = new Vector3[ indices.size() ];

  int faceNum = 0;

  for ( size_t i = 0; i < indices.size(); i += 3 )
  {    
    Vector3 & vA = *vertices[ indices[ i + 0 ] ].pos;
    Vector3 & vB = *vertices[ indices[ i + 1 ] ].pos;
    Vector3 & vC = *vertices[ indices[ i + 2 ] ].pos;

    tempNormals[ faceNum ] = ( vB - vA ).cross( vC - vA );

    ++faceNum;
  };

  Vector3 sum ( 0.0f, 0.0f, 0.0f );
  Vector3 zero ( 0.0f, 0.0f, 0.0f );

  int vLinksCount = vertices.size();
  int indicesCount = indices.size();

  for ( size_t i = 0; i < vLinksCount; i++ )
  {
    faceNum = 0;

    for ( size_t j = 0; j < indicesCount; j += 3 )
    {
      if ( indices[ j ] == i || indices[ j + 1 ] == i || indices[ j + 2 ] == i )
        sum = sum + tempNormals [ faceNum ];

      faceNum++;
    }

    vertices[ i ].nor = sum.normalized();

    sum = zero;
  }

  delete tempNormals;
};

struct Triangle
{
  unsigned short  index[3];
};

void Mesh::CalculateTangent(  )
{  
  Vector3 *tan1 = new Vector3[ vertices.size() * 2];
  Vector3 *tan2 = tan1 + vertices.size();

  memset( tan1, 0, vertices.size() * sizeof( Vector3 ) * 2 );

  for (long a = 0; a < indices.size(); a += 3)
  {
    long i1 = indices[ a + 0 ];
    long i2 = indices[ a + 1 ];
    long i3 = indices[ a + 2 ];

    const Vector3 & v1 = *vertices[ i1 ].pos;
    const Vector3 & v2 = *vertices[ i2 ].pos;
    const Vector3 & v3 = *vertices[ i3 ].pos;

    const Vector2 & w1 = *vertices[ i1 ].tex;
    const Vector2 & w2 = *vertices[ i2 ].tex;
    const Vector2 & w3 = *vertices[ i3 ].tex;

    float x1 = v2.x - v1.x;
    float x2 = v3.x - v1.x;
    float y1 = v2.y - v1.y;
    float y2 = v3.y - v1.y;
    float z1 = v2.z - v1.z;
    float z2 = v3.z - v1.z;

    float s1 = w2.x - w1.x;
    float s2 = w3.x - w1.x;
    float t1 = w2.y - w1.y;
    float t2 = w3.y - w1.y;

    float r = 1.0F / (s1 * t2 - s2 * t1);

    Vector3 sdir( (t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r );
    Vector3 tdir( (s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r );

    tan1[i1].x += sdir.x;
    tan1[i1].y += sdir.y;
    tan1[i1].z += sdir.z;

    tan1[i2].x += sdir.x;
    tan1[i2].y += sdir.y;
    tan1[i2].z += sdir.z;

    tan1[i3].x += sdir.x;
    tan1[i3].y += sdir.y;
    tan1[i3].z += sdir.z;

    tan2[i1].x += tdir.x;
    tan2[i1].y += tdir.y;
    tan2[i1].z += tdir.z;

    tan2[i2].x += tdir.x;
    tan2[i2].y += tdir.y;
    tan2[i2].z += tdir.z;

    tan2[i3].x += tdir.x;
    tan2[i3].y += tdir.y;
    tan2[i3].z += tdir.z;
  }

  for (long a = 0; a < vertices.size(); a++)
    vertices[ a ].tan = (tan1[ a ] - vertices[ a ].nor * ( vertices[ a ].nor.dot( tan1[ a ] ))).normalized(); 

  delete tan1;
}