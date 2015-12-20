/*******************************************************************************
*                               Ruthenium Engine                               *
*            Copyright (c) 2013-2016 Stepanov Dmitriy aka mrDIMAS              *
*                                                                              *
* This file is part of Ruthenium Engine.                                      *
*                                                                              *
* Ruthenium Engine is free software: you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as published by  *
* the Free Software Foundation, either version 3 of the License, or            *
* (at your option) any later version.                                          *
*                                                                              *
* Ruthenium Engine is distributed in the hope that it will be useful,         *
* but WITHOUT ANY WARRANTY; without even the implied warranty of               *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                *
* GNU Lesser General Public License for more details.                          *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with Ruthenium Engine.  If not, see <http://www.gnu.org/licenses/>.   *
*                                                                              *
*******************************************************************************/

//#define D3D_DEBUG_INFO
#define INITGUID
#define _CRT_SECURE_NO_WARNINGS

#define DIRECTINPUT_VERSION 0x0800
#include <vld.h>
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

template<typename Interface>
class COMPtr {
private:
	Interface * mInterface;
public:
	COMPtr() {
		mInterface = nullptr;
	}

	// for Direct3DCreate9
	void Set( Interface * iface ) {
		mInterface = iface;
	}

	void Reset() {
		if( mInterface ) {
			mInterface->Release();
			mInterface = nullptr;	
		}
	}

	~COMPtr() {
		if( mInterface ) {
			mInterface->Release();
		}
	}

	inline Interface ** operator & () {
		return &mInterface;
	}

	inline Interface * operator -> () {
		return mInterface;
	}

	operator Interface* () {
		return mInterface;
	}

	Interface * operator = ( Interface * iface ) {
		iface->AddRef();
		mInterface = iface;
		return mInterface;
	}
};

using namespace std;