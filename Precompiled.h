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
#include <DxErr.h>
#include <windows.h>
#include <dinput.h>
#include <fstream>
#include FT_FREETYPE_H