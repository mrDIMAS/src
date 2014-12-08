#ifndef _COMMON_
#define _COMMON_

//#define D3D_DEBUG_INFO
#define INITGUID
#define _CRT_SECURE_NO_WARNINGS

#define DIRECTINPUT_VERSION 0x0800

#include "Engine.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <algorithm>
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "LinearMath\btVector3.h"
#include "LinearMath\btQuaternion.h"
#include "LinearMath\btTransform.h"
#include "ProjectF.h"
#include <queue>
#include "ft2build.h"
#include <DxErr.h>
#include <windows.h>
#include <dinput.h>
#include <fstream>
#include FT_FREETYPE_H
#include "vld.h"

#define DEG2RAD ( 3.14159f / 180.0f )

#pragma warning( disable:4244 ) // double to float
#pragma warning( disable:4305 ) // double to float
#pragma warning( disable:4018 ) // < signed/unsigned mismatch
#pragma warning( disable:4800 ) // bool to int

#ifdef _DEBUG
#   pragma comment(lib, "BulletCollision_debug")
#   pragma comment(lib, "BulletDynamics_debug")
#   pragma comment(lib, "LinearMath_debug")
#   pragma comment(lib, "FreeType253MT_D")
#else
#   pragma comment(lib, "BulletCollision")
#   pragma comment(lib, "BulletDynamics")
#   pragma comment(lib, "LinearMath")
#   pragma comment(lib, "FreeType253MT")
#endif

#pragma comment( lib, "ProjectF")
#pragma comment( lib, "d3d9" )
#pragma comment( lib, "d3dx9" )
#pragma comment( lib, "dxerr" )
#pragma comment( lib, "dinput8.lib" )
#pragma comment( lib, "dxguid.lib" )

using namespace std;

class Renderer;
class PostProcessing;
class DeferredRenderer;
class ParticleSystemRenderer;
class GUIRenderer;
class Light;
class ForwardRenderer;
class ParticleEmitter;
class SceneNode;
class Camera;
class Skybox;
class Texture;
class Cursor;
class Vertex;
class Vertex2D;
class FPSCounter;
class TextRenderer;

#define MEMORY_VALID_VALUE ( 0xDEADBEEF )

void ParseString( string str, map<string,string> & values);
void ParseFile( string fn, map<string,string> & values);

double GetTimeInSeconds( );
double GetTimeInMilliSeconds( );
double GetTimeInMicroSeconds( );

// globals
extern float g_width;
extern float g_height;
extern int g_dips;
extern int g_rendererType;
extern float g_dt;

extern FT_Library g_ftLibrary;
// direct3d
extern IDirect3D9 * g_d3d;
extern IDirect3DDevice9 * g_device;
extern IDirect3DVertexDeclaration9 * g_meshVertexDeclaration;

// bullet
extern btDynamicsWorld * g_dynamicsWorld;
extern btDefaultCollisionConfiguration * g_defaultCollision;
extern btCollisionDispatcher * g_collisionDispatcher;
extern btBroadphaseInterface * g_broadphase;
extern btSequentialImpulseConstraintSolver * g_solver;
// common
extern bool g_fxaaEnabled;
extern vector< SceneNode* > g_nodes;
extern vector< Light* > g_pointLights;
extern vector< Light* > g_spotLights;
extern Camera * g_camera;
extern int g_debugDraw;
extern int g_textureChanges;
extern vector<ParticleEmitter*> g_particleEmitters;
extern ruVector3 g_ambientColor;
extern DeferredRenderer * g_deferredRenderer;
extern PostProcessing * g_postProcessing;
extern ParticleSystemRenderer * g_particleSystemRenderer;
extern ForwardRenderer * g_forwardRenderer;
extern GUIRenderer * g_guiRenderer;
extern TextRenderer * g_textRenderer;
extern Cursor * g_cursor;
extern Renderer * g_renderer;
extern int g_fps;
extern FPSCounter g_fpsCounter;
extern bool g_physicsEnabled;
extern bool g_engineRunning;
extern bool g_hdrEnabled;
extern string g_texturePath;
extern ofstream g_log;

// shadow settings
extern bool g_usePointLightShadows;
extern bool g_useSpotLightShadows;

extern void CheckDXErrorFatal( HRESULT errCode );
extern void CreateLogFile();
extern void LogMessage( string message );
extern void CloseLogFile();
extern void LogError( string message );
extern string Format( const char * format, ... );

#endif
