//#define D3D_DEBUG_INFO
#define INITGUID
#define _CRT_SECURE_NO_WARNINGS

#define DIRECTINPUT_VERSION 0x0800

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
#include <windows.h>
#include <dinput.h>
#include <fstream>
#include <memory>

#include FT_FREETYPE_H

#include "RutheniumAPI.h"
#include "Log.h"
#include "RendererComponent.h"

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
#	pragma comment( lib, "../external/ProjectF/ProjectF_debug")
#else
#   pragma comment(lib, "BulletCollision")
#   pragma comment(lib, "BulletDynamics")
#   pragma comment(lib, "LinearMath")
#   pragma comment(lib, "FreeType253MT")
#	pragma comment( lib, "../external/ProjectF/ProjectF")
#endif


#pragma comment( lib, "d3d9" )
#pragma comment( lib, "d3dx9" )
#pragma comment( lib, "dinput8.lib" )
#pragma comment( lib, "dxguid.lib" )

static void RaiseError( const string & text ) {
	MessageBoxA( 0, text.c_str(), "CriticalError", MB_OK | MB_ICONERROR );
	exit( -1 );
}


using namespace std;