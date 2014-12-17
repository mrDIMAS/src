// this converter used to convert raw data from 3ds max exporter to engine format
// (C) mrDIMAS 2014 - 2015

#include "Vector.h"
#include "Writer.h"
#include "Reader.h"
#include "Mesh.h"

struct Keyframe
{
  Vector3 pos;
  Vector4 rot;

  Keyframe( Vector3 _pos, Vector4 _rot )
  {
    pos = _pos;
    rot = _rot;
  }
};





struct Object
{
  string name;
  string props;
  int animated;
  vector<Keyframe> keyframes;
  Vector3 pos;
  Vector4 rot;
  vector<Material> materials;
  vector<Mesh*> meshes;
  int skinned;
  vector<string> bones;
  vector< Vector3 > vertexCoords;
  vector< Vector2 > texCoords;
  vector< Vector2 > secondTexCoords;
};

struct Light
{
  Vector3 color;
  float inner, outer;
  float radius;
  Vector3 pos;
  Vector4 rot;
  string name;
  float brightness;
  int type;
};

struct Multimesh
{
  vector< Mesh* > meshes;
  Mesh * currentMesh;
  Material material;

  Multimesh( Material mat )
  {
    currentMesh = new Mesh;
    currentMesh->mat = mat;
    material = mat;
    meshes.push_back( currentMesh );
  }

  void AddVertex( const Vertex & v, const Weight & w )
  {
    currentMesh->AddVertex( v, w );

    if( currentMesh->indices.size() >= 65535 )
    {
      currentMesh = new Mesh;
      currentMesh->mat = material;
      meshes.push_back( currentMesh );
    }
  }

  void AddVertex( const Vertex & v )
  {
    currentMesh->AddVertex( v );

    if( currentMesh->indices.size() >= 65535 )
    {
      currentMesh = new Mesh;
      currentMesh->mat = material;
      meshes.push_back( currentMesh );
    }
  }
};


int main( int argc, char * argv[] )
{
  cout << "Ruthenium Raw Scene Converter. (C) mrDIMAS 2014-2015" << endl;
#ifndef _DEBUG
  if( argc < 2 )
  {
    cout << "You must pass at least filename to convert!" << endl;

    getch();

    return 0;
  }

  string filename = argv[ 1 ];
#else
  string filename = "C:/Mine/release/data/maps/release/arrival/arrival.scene";
#endif
  cout << "Reading " << filename << endl;

  Reader reader;

  vector<Object*> objects;
  vector<Light> lights;
  map< string, string > hierarchy;

  int framesCount;

  if( reader.ReadFile( filename ))
  {
    cout << "Reading header" << endl;

    framesCount = reader.GetInteger();
    int numObjects = reader.GetInteger();
    int numMeshes = reader.GetInteger();
    int numLights = reader.GetInteger();

    cout << "   Frames count: " << framesCount << endl;
    cout << "   Object count: " << numObjects << endl;
    cout << "   Mesh count: " << numMeshes << endl;
    cout << "   Light count: " << numLights << endl << endl;

    cout << "Conversion started..." << endl;

    for ( int objNum = 0; objNum < numMeshes; objNum++ )
    {
      Object * object = new Object;

      object->props = reader.GetString();

      int vertexCount = reader.GetInteger();
      int textureCoordCount = reader.GetInteger();
      int faceCount = reader.GetInteger();
      int isSkinned = reader.GetInteger();
      int hasAnimation = reader.GetInteger();
      
      object->name = reader.GetString();
      object->animated = hasAnimation;

#ifdef _DEBUG
      cout << "Reading object " << object->name << endl;
      cout << "   Vertex count: " << vertexCount << endl;
      cout << "   Texture coords count: " << textureCoordCount << endl;
      cout << "   Face count: " << faceCount << endl;
      cout << "   Is skinned: " << isSkinned << endl;
      cout << "   Animated: " << hasAnimation << endl ;
#endif

      if( hasAnimation )
      {
        // read keyframes
        for ( int frameNum = 0; frameNum < framesCount; frameNum++ )
        {
          Vector3 pos = reader.GetVector();
          Vector4 rot = reader.GetQuaternion();

          object->keyframes.push_back( Keyframe( pos, rot ) );
        };
      }
      else
      {
        object->pos = reader.GetVector();
        object->rot = reader.GetQuaternion();
      }

      vector<Multimesh*> meshes;

      // create materials
      int numMaterials = reader.GetInteger();

      for ( int matID = 0; matID < numMaterials; matID++ )
      {
        Material mat;

        string diffuse = reader.GetString();
        float opacity = reader.GetFloat();

        mat.diffuse = diffuse;
        mat.opacity = opacity;

        if( diffuse.size() )
        {
          diffuse.insert( diffuse.find_last_of( '.' ), "_normal" );

          mat.normal = diffuse;
        }

        object->materials.push_back( mat );
        meshes.push_back( new Multimesh( mat ) );
      }

      // read vertices
      for ( int i = 0; i < vertexCount; ++i )
        object->vertexCoords.push_back( reader.GetVector() );

      vector< Weight > weights;

      // read bones
      if( isSkinned )
      {
        // read weights for each vertex
        for( int i = 0; i < vertexCount; i++ )
        {
          Weight weigth;

          weigth.boneCount = reader.GetInteger();

          for( int k = 0; k < weigth.boneCount; k++ )
          { 
            weigth.bones[ k ].id = reader.GetInteger();
            weigth.bones[ k ].weight = reader.GetFloat();
          }

          weights.push_back( weigth );
        }
      }

      int numMaps = reader.GetInteger();
      int numMapChannels = reader.GetInteger();

      if( numMapChannels == 1 )
      {
        // read texture coords
        object->texCoords.reserve( textureCoordCount );
        for ( int i = 0; i < textureCoordCount; ++i )
          object->texCoords.push_back ( reader.GetBareVector2() );

        // read texture coords
        object->secondTexCoords.reserve( textureCoordCount );
        for ( int i = 0; i < textureCoordCount; ++i )
          object->secondTexCoords.push_back ( object->texCoords[ i ] );
      }

      if( numMapChannels == 2 )
      {
        // read texture coords
        object->texCoords.reserve( textureCoordCount );
        for ( int i = 0; i < textureCoordCount; ++i )
          object->texCoords.push_back ( reader.GetBareVector2() );

        // read texture coords
        object->secondTexCoords.reserve( textureCoordCount );
        for ( int i = 0; i < textureCoordCount; ++i )
          object->secondTexCoords.push_back ( reader.GetBareVector2() );
      }

      if( object->texCoords.size() == 0 )
        object->texCoords.push_back( Vector2( 0, 0 ));
      
      // read faces and construct meshes
      for ( int i = 0; i < faceCount; ++i )
      {
        int fa = reader.GetInteger();
        int fb = reader.GetInteger();
        int fc = reader.GetInteger();

        int ta = reader.GetInteger();
        int tb = reader.GetInteger();
        int tc = reader.GetInteger();

        size_t faceMaterialID = reader.GetInteger();

        Vector3 aNormal = reader.GetVector();
        Vector3 bNormal = reader.GetVector();
        Vector3 cNormal = reader.GetVector();

        if( faceMaterialID >= numMaterials )
          continue;

        Multimesh * currentMultimesh = meshes[ faceMaterialID ];

        if( isSkinned )
        {
          if( textureCoordCount )
          {
            currentMultimesh->AddVertex( Vertex( &object->vertexCoords[ fa ], &object->texCoords[ ta ], &object->secondTexCoords[ ta ], aNormal ), weights[ fa ] );
            currentMultimesh->AddVertex( Vertex( &object->vertexCoords[ fb ], &object->texCoords[ tb ], &object->secondTexCoords[ tb ], bNormal), weights[ fb ] );
            currentMultimesh->AddVertex( Vertex( &object->vertexCoords[ fc ], &object->texCoords[ tc ], &object->secondTexCoords[ tc ], cNormal), weights[ fc ] );
          }
          else
          {
            currentMultimesh->AddVertex( Vertex( &object->vertexCoords[ fa ], &object->texCoords[ 0 ], &object->secondTexCoords[ ta ], aNormal ), weights[ fa ] );
            currentMultimesh->AddVertex( Vertex( &object->vertexCoords[ fb ], &object->texCoords[ 0 ], &object->secondTexCoords[ tb ], bNormal ), weights[ fb ] );
            currentMultimesh->AddVertex( Vertex( &object->vertexCoords[ fc ], &object->texCoords[ 0 ], &object->secondTexCoords[ tc ], cNormal ), weights[ fc ] );
          }
        }
        else
        {
          if( textureCoordCount )
          {
            currentMultimesh->AddVertex( Vertex( &object->vertexCoords[ fa ], &object->texCoords[ ta ], &object->secondTexCoords[ ta ], aNormal ) );
            currentMultimesh->AddVertex( Vertex( &object->vertexCoords[ fb ], &object->texCoords[ tb ], &object->secondTexCoords[ tb ], bNormal ) );
            currentMultimesh->AddVertex( Vertex( &object->vertexCoords[ fc ], &object->texCoords[ tc ], &object->secondTexCoords[ tc ], cNormal ) );
          }
          else
          {
            currentMultimesh->AddVertex( Vertex( &object->vertexCoords[ fa ], &object->texCoords[ 0 ], &object->texCoords[ 0 ], aNormal ) );
            currentMultimesh->AddVertex( Vertex( &object->vertexCoords[ fb ], &object->texCoords[ 0 ], &object->texCoords[ 0 ], bNormal ) );
            currentMultimesh->AddVertex( Vertex( &object->vertexCoords[ fc ], &object->texCoords[ 0 ], &object->texCoords[ 0 ], cNormal ) );
          }
        }

      }

      for( auto it = meshes.begin(); it != meshes.end(); ++it )
        for( int k = 0; k < (*it)->meshes.size(); k++ )
          object->meshes.push_back( (*it)->meshes[ k ]);

      for( size_t i = 0; i < object->meshes.size(); i++ )
      {
        Mesh * mesh = object->meshes[i];   
        //mesh->CalculateNormals();
        mesh->CalculateTangent();
        mesh->CalculateAABB();
      }

      object->skinned = isSkinned;

      objects.push_back( object );
    }
    
    // read lights
    for ( int objNum = 0; objNum < numLights; objNum++ )
    {
      Light light;
      light.name = reader.GetString();
      light.type = reader.GetInteger();
  
      // this helps avoid undefined behaviour
      light.color.x = reader.GetInteger();
      light.color.y = reader.GetInteger();
      light.color.z = reader.GetInteger();
  
      light.radius = reader.GetFloat();

	  light.brightness = reader.GetFloat();

      light.pos = reader.GetVector();

      if( light.type == 1 ) // free spot
      {
        light.inner = reader.GetFloat();
        light.outer = reader.GetFloat();

        light.rot = reader.GetQuaternion();
      }
      lights.push_back( light );
    }   

    // read hierarchy
    for ( int objNum = 0; objNum < numObjects; objNum++ )
    {
      string objectName = reader.GetString();
      string parentName = reader.GetString();

      hierarchy[ objectName ] = parentName;
    }
  }
  


  // now write computed data
  Writer writer( filename );

  writer.WriteInteger( objects.size() + lights.size() );
  writer.WriteInteger( objects.size() );
  writer.WriteInteger( lights.size() );
  writer.WriteInteger( framesCount );

  cout << endl << "Convertion done!" << endl << endl;
  cout << "Writing computed data..." << endl;
  cout << "   Total objects: " << objects.size() + lights.size() << endl;
  cout << "   Total mesh-objects: " << objects.size()  << endl;
  cout << "   Total lights: " << lights.size() << endl;
  cout << "   Total frames: " << framesCount << endl;
 
  for( int i = 0; i < objects.size(); i++ )
  {
    Object * obj = objects[ i ];

#ifdef _DEBUG
    cout << "Write object: " << obj->name << endl;
    cout << "   Mesh count: " << obj->meshes.size() << endl;
#endif

    writer.WriteVector( obj->pos );
    writer.WriteQuaternion( obj->rot );
    writer.WriteInteger( obj->animated );
    writer.WriteInteger( obj->skinned );    
    writer.WriteInteger( obj->meshes.size() );
    writer.WriteInteger( obj->keyframes.size() );

    writer.WriteString( obj->props );
    writer.WriteString( obj->name );

    for( int keyFrameNum = 0; keyFrameNum < obj->keyframes.size(); keyFrameNum++ )
    {
      Keyframe & kf = obj->keyframes[ keyFrameNum ];
      writer.WriteVector( kf.pos );
      writer.WriteQuaternion( kf.rot );
    }

    for( int meshNum = 0; meshNum < obj->meshes.size(); meshNum++ )
    {
      Mesh * mesh = obj->meshes[ meshNum ];
      
      writer.WriteInteger( mesh->vertices.size() );
      writer.WriteInteger( mesh->indices.size() );
      writer.WriteVector( mesh->min );
      writer.WriteVector( mesh->max );
      writer.WriteVector( mesh->center );
      writer.WriteFloat( mesh->radius );
      writer.WriteString( mesh->mat.diffuse );
      writer.WriteString( mesh->mat.normal );
      writer.WriteFloat( mesh->mat.opacity );

#ifdef _DEBUG
      cout << "   Write mesh: " << meshNum << endl;
      cout << "       Vertex count: " << mesh->vertices.size() << endl;
      cout << "       Index count: " << mesh->indices.size() << endl;
      cout << "       Face count: " << mesh->indices.size() / 3 << endl;
      cout << "       Diffuse: " << mesh->mat.diffuse << endl;
      cout << "       Normal: " << mesh->mat.normal << endl;
#endif

      for( int vertexNum = 0; vertexNum < mesh->vertices.size(); vertexNum++ )
      {
        Vertex & v = mesh->vertices[ vertexNum ];

        writer.WriteVector( *v.pos );
        writer.WriteVector( v.nor );
        writer.WriteVector2( *v.tex );
        writer.WriteVector2( *v.secondTex );
        writer.WriteVector( v.tan );
      }

      for( int indexNum = 0; indexNum < mesh->indices.size(); indexNum++ )
        writer.WriteShort( mesh->indices[ indexNum ]);

      if( obj->skinned )
      {
        for( int weightNum = 0; weightNum < mesh->weights.size(); weightNum++ )
        {
          Weight & w = mesh->weights[ weightNum ];

          writer.WriteInteger( w.boneCount );

          for( int k = 0; k < w.boneCount; k++ )
          {
            writer.WriteInteger( w.bones[ k ].id );
            writer.WriteFloat( w.bones[ k ].weight );
          }
        }
      }
    }
  }

  for( int i = 0; i < lights.size(); i++ )
  {
    Light & light = lights[ i ];

    writer.WriteString( light.name  );
    writer.WriteInteger( light.type );
    writer.WriteVector( light.color ); 
    writer.WriteFloat( light.radius );
	writer.WriteFloat( light.brightness );
    writer.WriteVector( light.pos );

    if( light.type == 1 ) // free spot
    {
      writer.WriteFloat( light.inner );
      writer.WriteFloat( light.outer );
      writer.WriteQuaternion( light.rot );
    }
  }

  for( auto it = hierarchy.begin(); it != hierarchy.end(); ++it )
  {
    writer.WriteString( it->first );
    writer.WriteString( it->second );
  }

  for( int i = 0; i < objects.size(); i++ )
  {
    Object * object = objects[ i ];
    for( int k = 0; k < object->meshes.size(); k++ )
      delete object->meshes[ k ];
  }

  cout << "Complete!" << endl;

  return 0;
}