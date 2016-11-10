/*******************************************************************************
*                               Ruthenium Engine                               *
*            Copyright (c) 2013-2017 Stepanov Dmitriy aka mrDIMAS              *
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

#include "Precompiled.h"
#include "Light.h"
#include "Mesh.h"
#include "Camera.h"
#include "Texture.h"
#include "Renderer.h"
#include "Physics.h"
#include "BitmapFont.h"
#include "SceneFactory.h"
#include "Cursor.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "ParticleSystem.h"
#include "SceneNode.h"
#include "Mesh.h"
#include "Camera.h"
#include "Skybox.h"
#include "Utility.h"
#include "Renderer.h"
#include "SceneFactory.h"
#include "BitmapFont.h"
#include "GUIRect.h"
#include "GUIText.h"
#include "GUIButton.h"
#include "GUIScene.h"
#include <random>

IDirect3DDevice9Ex * pD3D;
unique_ptr<Renderer> pEngine;
Mouse mouse;
Keyboard keyboard;



struct XYZNormalVertex {
	D3DXVECTOR3 p;
	D3DXVECTOR3 n;
};

struct CameraDirection {
	ruVector3 target;
	ruVector3 up;
};

struct GPUFloatRegister {
	float a, b, c, d;

	void Set(float v1) {
		a = v1;
		b = 0.0f;
		c = 0.0f;
		d = 0.0f;
	}

	void Set(float v1, float v2) {
		a = v1;
		b = v2;
		c = 0.0f;
		d = 0.0f;
	}

	void Set(float v1, float v2, float v3) {
		a = v1;
		b = v2;
		c = v3;
		d = 0.0f;
	}

	void Set(float v1, float v2, float v3, float v4) {
		a = v1;
		b = v2;
		c = v3;
		d = v4;
	}
};

class GPUBoolRegisterStack {
public:
	BOOL mBooleans[8];
	int mBooleanCount;

	GPUBoolRegisterStack() : mBooleanCount(0) {

	}

	void Clear() {
		mBooleanCount = 0;
	}

	void Push(BOOL boolean) {
		mBooleans[mBooleanCount++] = boolean;
	}

	const BOOL * GetPointer() const {
		return &mBooleans[0];
	}
};

class GPUFloatRegisterStack {
public:
	GPUFloatRegister mRegisters[256];
	int mRegisterCount;

	GPUFloatRegisterStack() : mRegisterCount(0) {

	}

	void Clear() {
		mRegisterCount = 0;
	}

	void PushMatrix(const D3DXMATRIX & m) {
		mRegisters[mRegisterCount++].Set(m._11, m._12, m._13, m._14);
		mRegisters[mRegisterCount++].Set(m._21, m._22, m._23, m._24);
		mRegisters[mRegisterCount++].Set(m._31, m._32, m._33, m._34);
		mRegisters[mRegisterCount++].Set(m._41, m._42, m._43, m._44);
	}

	void PushIdentityMatrix() {
		mRegisters[mRegisterCount++].Set(1, 0, 0, 0);
		mRegisters[mRegisterCount++].Set(0, 1, 0, 0);
		mRegisters[mRegisterCount++].Set(0, 0, 1, 0);
		mRegisters[mRegisterCount++].Set(0, 0, 0, 1);
	}

	void PushVector(const ruVector3 & v) {
		mRegisters[mRegisterCount++].Set(v.x, v.y, v.z);
	}

	void PushVector(const ruVector2 & v) {
		mRegisters[mRegisterCount++].Set(v.x, v.y);
	}

	void PushFloat(float v1) {
		mRegisters[mRegisterCount++].Set(v1);
	}

	void PushFloat(float v1, float v2) {
		mRegisters[mRegisterCount++].Set(v1, v2);
	}

	void PushFloat(float v1, float v2, float v3) {
		mRegisters[mRegisterCount++].Set(v1, v2, v3);
	}

	void PushFloat(float v1, float v2, float v3, float v4) {
		mRegisters[mRegisterCount++].Set(v1, v2, v3, v4);
	}

	const float * GetPointer() const {
		return &mRegisters[0].a;
	}
};

/*
=========
SetUniformScaleTranslationMatrix
=========
*/
D3DXMATRIX SetUniformScaleTranslationMatrix(float s, const ruVector3 & p) {
	return D3DXMATRIX(
		s, 0.0f, 0.0f, 0.0f,
		0.0f, s, 0.0f, 0.0f,
		0.0f, 0.0f, s, 0.0f,
		p.x, p.y, p.z, 1.0f
	);
}

/*
=========
WindowProcess
=========
*/
LRESULT CALLBACK WindowProcess(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_DESTROY:
		ruEngine::Free();
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		keyboard.KeyDown((lParam & 0x01FF0000) >> 16);
		break;
	case WM_KEYUP:
		keyboard.KeyUp((lParam & 0x01FF0000) >> 16);
		break;
	case WM_MOUSEMOVE:
		mouse.Move(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_LBUTTONDOWN:
		mouse.ButtonDown(ruInput::MouseButton::Left);
		break;
	case WM_RBUTTONDOWN:
		mouse.ButtonDown(ruInput::MouseButton::Right);
		break;
	case WM_MBUTTONDOWN:
		mouse.ButtonDown(ruInput::MouseButton::Middle);
		break;
	case WM_LBUTTONUP:
		mouse.ButtonUp(ruInput::MouseButton::Left);
		break;
	case WM_RBUTTONUP:
		mouse.ButtonUp(ruInput::MouseButton::Right);
		break;
	case WM_MBUTTONUP:
		mouse.ButtonUp(ruInput::MouseButton::Middle);
		break;
	case WM_MOUSEWHEEL:
		mouse.Wheel(((short)HIWORD(wParam)) / WHEEL_DELTA);
		break;
	}
	return DefWindowProc(wnd, msg, wParam, lParam);
}

/*
=========
Renderer
=========
*/
Renderer::Renderer(int width, int height, int fullscreen, char vSync) :
	mAmbientColor(0.05, 0.05, 0.05),
	mUsePointLightShadows(false),
	mUseSpotLightShadows(false),
	mHDREnabled(false),
	mRunning(true),
	mFXAAEnabled(false),
	mTextureStoragePath("data/textures/generic/"),
	mParallaxEnabled(true),
	mPaused(false),
	mChangeVideomode(false),
	mShadersChangeCount(0),
	mRenderedTriangleCount(0),
	mTextureChangeCount(0),
	mSSAOEnabled(false) {
	if (width == 0 || height == 0) {
		width = GetSystemMetrics(SM_CXSCREEN);
		height = GetSystemMetrics(SM_CYSCREEN);
		Log::Write("Zero passed as a resolution, using desktop native resolution.");
	}

	// Create Render window
	const wchar_t * className = L"The Mine";
	WNDCLASSEXW wcx = { 0 };
	wcx.cbSize = sizeof(wcx);
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcx.hInstance = GetModuleHandle(0);
	wcx.lpfnWndProc = WindowProcess;
	wcx.lpszClassName = className;
	wcx.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClassExW(&wcx);

	DWORD style = fullscreen ? (style = WS_POPUP) : (WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

	RECT wRect = { 0, 0, width, height };

	AdjustWindowRect(&wRect, style, 0);

	mWindowHandle = CreateWindowW(className, className, style, 0, 0, wRect.right - wRect.left, wRect.bottom - wRect.top, 0, 0, GetModuleHandle(0), 0);

	// Setup window
	ShowWindow(mWindowHandle, SW_SHOW);
	UpdateWindow(mWindowHandle);
	SetActiveWindow(mWindowHandle);
	SetForegroundWindow(mWindowHandle);

	ruInput::Init(mWindowHandle);

	// Create Direct3D9
	Direct3DCreate9Ex(D3D_SDK_VERSION, &mpDirect3D);

	if (!mpDirect3D) {
		Log::Error("Failed to Direct3DCreate9! Ensure, that you have latest video drivers! Engine initialization failed!");
	} else {
		Log::Write("Direct3D 9 Created successfully!");
	}

	// Check if Pixel Shader 3.0+ is supported
	D3DCAPS9 dCaps;
	mpDirect3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &dCaps);

	if (D3DSHADER_VERSION_MAJOR(dCaps.PixelShaderVersion) < 3) {
		mpDirect3D->Release();
		Log::Error("Your graphics card doesn't support Pixel Shader 3.0. Engine initialization failed! Buy a modern video card!");
	}

	// Check passed resolution
	bool passedResolutionValid = false;
	D3DDISPLAYMODE mode;
	for (int i = 0; i < mpDirect3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8); i++) {
		mpDirect3D->EnumAdapterModes(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, i, &mode);
		Log::Write(StringBuilder("Videomode: ") << mode.Width << " x " << mode.Height << " x 32 @ " << mode.RefreshRate);
		mVideomodeList.push_back(Videomode(mode.Width, mode.Height, mode.RefreshRate));
		if (mode.Width == width && mode.Height == height) {
			passedResolutionValid = true;
			break;
		}
	}

	if (passedResolutionValid) {
		Log::Write(StringBuilder("Setting resolution ") << width << " x " << height << "...");
	} else {
		Log::Write(StringBuilder("WARNING! Resolution ") << width << " x " << height << " is invalid! Using native desktop resolution instead");
		width = GetSystemMetrics(SM_CXSCREEN);
		height = GetSystemMetrics(SM_CYSCREEN);
	}

	mResWidth = width;
	mResHeight = height;

	mNativeResolutionWidth = GetSystemMetrics(SM_CXSCREEN);
	mNativeResolutionHeight = GetSystemMetrics(SM_CYSCREEN);

	D3DDISPLAYMODEEX displayMode;
	displayMode.Size = sizeof(D3DDISPLAYMODEEX);
	D3DCALL(mpDirect3D->GetAdapterDisplayModeEx(D3DADAPTER_DEFAULT, &displayMode, nullptr));

	// present parameters
	memset(&mPresentParameters, 0, sizeof(mPresentParameters));

	mPresentParameters.BackBufferCount = 2;
	mPresentParameters.EnableAutoDepthStencil = TRUE;
	if (vSync) {
		mPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	} else {
		mPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}
	mPresentParameters.AutoDepthStencilFormat = D3DFMT_D24S8;
	mPresentParameters.BackBufferWidth = width;
	mPresentParameters.BackBufferHeight = height;
	mPresentParameters.hDeviceWindow = mWindowHandle;
	if (fullscreen) {
		mPresentParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
		mPresentParameters.SwapEffect = D3DSWAPEFFECT_FLIPEX;
		mPresentParameters.Windowed = FALSE;
		mPresentParameters.FullScreen_RefreshRateInHz = mode.RefreshRate;

		displayMode.Height = mPresentParameters.BackBufferHeight;
		displayMode.Width = mPresentParameters.BackBufferWidth;
		displayMode.Format = mPresentParameters.BackBufferFormat;
		displayMode.RefreshRate = mode.RefreshRate;
	} else {
		mPresentParameters.BackBufferFormat = displayMode.Format;
		mPresentParameters.SwapEffect = D3DSWAPEFFECT_FLIPEX;
		mPresentParameters.Windowed = TRUE;
	}

	// No multisampling, because of deferred shading
	mPresentParameters.MultiSampleQuality = 0;
	mPresentParameters.MultiSampleType = D3DMULTISAMPLE_NONE;

	// Create device
	D3DCALL(mpDirect3D->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, mWindowHandle, D3DCREATE_HARDWARE_VERTEXPROCESSING, &mPresentParameters, fullscreen ? &displayMode : nullptr, &mpDevice));

	pD3D = mpDevice;

	D3DCALL(pD3D->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &mpBackBuffer));

	// Create common vertex declaration
	D3DVERTEXELEMENT9 commonVertexDeclaration[] = {
		{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
		{ 0, 44, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 }, // bone indices
		{ 0, 60, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 }, // bone weights
		D3DDECL_END()
	};

	D3DCALL(pD3D->CreateVertexDeclaration(commonVertexDeclaration, &msVertexDeclaration));

	SetDefaults();

	// Initialize physics
	Physics::CreateWorld();

	// Initialize sound sub-system
	pfSystemCreateLogFile("ProjectF.log");

	//pfSystemEnableMessagesOutputToConsole();
	pfSystemEnableMessagesOutputToLogFile();

	pfSystemInit();
	pfSetListenerDopplerFactor(0);


	//***************************************************************
	// Init Deferred Rendering Stuff

	// Create render targets 
	D3DCALL(pD3D->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &mHDRFrame, nullptr));
	D3DCALL(mHDRFrame->GetSurfaceLevel(0, &mHDRFrameSurface));

	// Create additional render targets for posteffects
	for (int i = 0; i < 2; i++) {
		D3DCALL(pD3D->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &mFrame[i], nullptr));
		D3DCALL(mFrame[i]->GetSurfaceLevel(0, &mFrameSurface[i]));
	}

	// FXAA Pixel Shader
	LoadPixelShader(mFXAAPixelShader, "data/shaders/fxaa.pso");

	// Create G-Buffer ( Depth: R32F, Diffuse: ARGB8, Normal: ARGB8 )
	{
		D3DCALL(pD3D->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &mDepthMap, nullptr));
		D3DCALL(mDepthMap->GetSurfaceLevel(0, &mDepthSurface));

		D3DCALL(pD3D->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &mNormalMap, nullptr));
		D3DCALL(mNormalMap->GetSurfaceLevel(0, &mNormalSurface));

		D3DCALL(pD3D->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &mDiffuseMap, nullptr));
		D3DCALL(mDiffuseMap->GetSurfaceLevel(0, &mDiffuseSurface));
	}

	// Ambient light pixel shader
	LoadPixelShader(mAmbientPixelShader, "data/shaders/deferredAmbientLight.pso");

	// GBuffer shaders
	LoadVertexShader(mGBufferVertexShader, "data/shaders/deferredGBuffer.vso");
	LoadPixelShader(mGBufferPixelShader, "data/shaders/deferredGBuffer.pso");
	LoadPixelShader(mDeferredBlending, "data/shaders/deferredBlending.pso");
	LoadPixelShader(mDeferredLightShader, "data/shaders/deferredLighting.pso");

	// Init HDR Stuff
	{
		int scaledSize = IntegerPow(2, mHDRDownSampleCount + 1);
		D3DCALL(pD3D->CreateTexture(scaledSize, scaledSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &mScaledScene, nullptr));
		D3DCALL(mScaledScene->GetSurfaceLevel(0, &mScaledSceneSurf));

		for (int i = 0; i < mHDRDownSampleCount; i++) {
			int size = IntegerPow(2, mHDRDownSampleCount - i);
			D3DCALL(pD3D->CreateTexture(size, size, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &mDownSampTex[i], nullptr));
		}

		D3DCALL(pD3D->CreateTexture(1, 1, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &mAdaptedLuminanceLast, nullptr));
		D3DCALL(pD3D->CreateTexture(1, 1, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &mAdaptedLuminanceCurrent, nullptr));

		for (int i = 0; i < mHDRDownSampleCount; i++) {
			D3DCALL(mDownSampTex[i]->GetSurfaceLevel(0, &mDownSampSurf[i]));
		}

		LoadPixelShader(mToneMapShader, "data/shaders/hdrTonemap.pso");
		LoadPixelShader(mScaleScenePixelShader, "data/shaders/hdrScale.pso");
		LoadPixelShader(mAdaptationPixelShader, "data/shaders/hdrAdaptation.pso");
		LoadPixelShader(mDownScalePixelShader, "data/shaders/hdrDownscale.pso");
	}

	// Spot light shadow map
	{
		int shadowMapWidth = width * 0.5f;
		int shadowMapHeight = height * 0.5f;

		LoadVertexShader(mShadowMapVertexShader, "data/shaders/shadowMap.vso");
		LoadPixelShader(mShadowMapPixelShader, "data/shaders/shadowMap.pso");

		D3DCALL(pD3D->CreateTexture(shadowMapWidth, shadowMapHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &mShadowMap, nullptr));
		D3DCALL(mShadowMap->GetSurfaceLevel(0, &mShadowMapSurface));

		D3DCALL(pD3D->GetDepthStencilSurface(&mDefaultDepthStencil));
		D3DCALL(pD3D->CreateDepthStencilSurface(shadowMapWidth, shadowMapHeight, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, TRUE, &mDepthStencilSurface, 0));
	}

	// Particle shaders
	LoadVertexShader(mParticleSystemVertexShader, "data/shaders/particle.vso");
	LoadPixelShader(mParticleSystemPixelShader, "data/shaders/particle.pso");

	// Create Bounding volumes
	{
		int quality = 6;

		D3DVERTEXELEMENT9 vd[] = {
			{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
			D3DDECL_END()
		};

		XYZNormalVertex * data;

		D3DCALL(D3DXCreateSphere(pD3D, 1.0, quality, quality, &mBoundingSphere, 0));
		D3DCALL(mBoundingSphere->UpdateSemantics(vd));

		D3DCALL(D3DXCreateCylinder(pD3D, 0.0f, 1.0f, 1.0f, quality, quality, &mBoundingCone, 0));

		D3DCALL(mBoundingCone->LockVertexBuffer(0, reinterpret_cast<void**>(&data)));
		D3DXMATRIX tran, rot90, transform;
		D3DXMatrixTranslation(&tran, 0, -0.5, 0);
		D3DXMatrixRotationAxis(&rot90, &D3DXVECTOR3(1, 0, 0), SIMD_HALF_PI);
		transform = rot90 * tran;
		for (int i = 0; i < mBoundingCone->GetNumVertices(); i++) {
			XYZNormalVertex * v = &data[i];
			D3DXVec3TransformCoord(&v->p, &v->p, &transform);
		}
		D3DCALL(mBoundingCone->UnlockVertexBuffer());
		D3DCALL(mBoundingCone->UpdateSemantics(vd));

		D3DCALL(D3DXCreateSphere(pD3D, 1.0, quality, quality, &mBoundingStar, 0));

		D3DCALL(mBoundingStar->LockVertexBuffer(0, reinterpret_cast<void**>(&data)));
		int n = 0;
		for (int i = 0; i < mBoundingStar->GetNumVertices(); i++) {
			XYZNormalVertex * v = &data[i];
			n++;
			if (n == 5) {
				v->p.x = 0;
				v->p.y = 0;
				v->p.z = 0;
				n = 0;
			}
		}
		D3DCALL(mBoundingStar->UnlockVertexBuffer());
		D3DCALL(mBoundingStar->UpdateSemantics(vd));
	}

	// Create fullscreen quad
	void * lockedData;
	{
		LoadVertexShader(mQuadVertexShader, "data/shaders/quad.vso");

		Vertex vertices[6];
		vertices[0] = Vertex(ruVector3(-0.5, -0.5, 0.0f), ruVector2(0, 0));
		vertices[1] = Vertex(ruVector3(GetResolutionWidth() - 0.5, -0.5, 0.0f), ruVector2(1, 0));
		vertices[2] = Vertex(ruVector3(-0.5, GetResolutionHeight() - 0.5, 0), ruVector2(0, 1));
		vertices[3] = Vertex(ruVector3(GetResolutionWidth() - 0.5, -0.5, 0.0f), ruVector2(1, 0));
		vertices[4] = Vertex(ruVector3(GetResolutionWidth() - 0.5, GetResolutionHeight() - 0.5, 0.0f), ruVector2(1, 1));
		vertices[5] = Vertex(ruVector3(-0.5, GetResolutionHeight() - 0.5, 0.0f), ruVector2(0, 1));

		D3DCALL(pD3D->CreateVertexBuffer(6 * sizeof(Vertex), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_TEX1, D3DPOOL_DEFAULT, &mQuadVertexBuffer, 0));
		D3DCALL(mQuadVertexBuffer->Lock(0, 0, &lockedData, 0));
		memcpy(lockedData, vertices, sizeof(Vertex) * 6);
		D3DCALL(mQuadVertexBuffer->Unlock());

		D3DXMatrixOrthoOffCenterLH(&mOrthoProjectionMatrix, 0, GetResolutionWidth(), GetResolutionHeight(), 0, 0, 1024);
	}

	// Skybox
	{
		LoadVertexShader(mSkyboxVertexShader, "data/shaders/skybox.vso");
		LoadPixelShader(mSkyboxPixelShader, "data/shaders/skybox.pso");

		float size = 1024.0f;
		Vertex fv[] = {
			Vertex(ruVector3(-size,  size, -size), ruVector2(0.0f, 0.0f)),
			Vertex(ruVector3(size,  size, -size), ruVector2(1.0f, 0.0f)),
			Vertex(ruVector3(size, -size, -size), ruVector2(1.0f, 1.0f)),
			Vertex(ruVector3(-size, -size, -size), ruVector2(0.0f, 1.0f)),
			Vertex(ruVector3(size,  size,  size), ruVector2(0.0f, 0.0f)),
			Vertex(ruVector3(-size,  size,  size), ruVector2(1.0f, 0.0f)),
			Vertex(ruVector3(-size, -size,  size), ruVector2(1.0f, 1.0f)),
			Vertex(ruVector3(size, -size,  size), ruVector2(0.0f, 1.0f)),
			Vertex(ruVector3(size,  size, -size), ruVector2(0.0f, 0.0f)),
			Vertex(ruVector3(size,  size,  size), ruVector2(1.0f, 0.0f)),
			Vertex(ruVector3(size, -size,  size), ruVector2(1.0f, 1.0f)),
			Vertex(ruVector3(size, -size, -size), ruVector2(0.0f, 1.0f)),
			Vertex(ruVector3(-size,  size,  size), ruVector2(0.0f, 0.0f)),
			Vertex(ruVector3(-size,  size, -size), ruVector2(1.0f, 0.0f)),
			Vertex(ruVector3(-size, -size, -size), ruVector2(1.0f, 1.0f)),
			Vertex(ruVector3(-size, -size,  size), ruVector2(0.0f, 1.0f)),
			Vertex(ruVector3(-size,  size,  size), ruVector2(0.0f, 0.0f)),
			Vertex(ruVector3(size,  size,  size), ruVector2(1.0f, 0.0f)),
			Vertex(ruVector3(size,  size, -size), ruVector2(1.0f, 1.0f)),
			Vertex(ruVector3(-size,  size, -size), ruVector2(0.0f, 1.0f))
		};
		D3DCALL(pD3D->CreateVertexBuffer(sizeof(fv), D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_TEX1, D3DPOOL_DEFAULT, &mSkyboxVertexBuffer, 0));
		D3DCALL(mSkyboxVertexBuffer->Lock(0, 0, &lockedData, 0));
		memcpy(lockedData, fv, sizeof(fv));
		D3DCALL(mSkyboxVertexBuffer->Unlock());

		unsigned short indices[] = { 0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19 };
		D3DCALL(pD3D->CreateIndexBuffer(sizeof(indices), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &mSkyboxIndexBuffer, 0));
		D3DCALL(mSkyboxIndexBuffer->Lock(0, 0, &lockedData, 0));
		memcpy(lockedData, indices, sizeof(indices));
		D3DCALL(mSkyboxIndexBuffer->Unlock());
	}

	// GUI Stuff
	{
		D3DCALL(pD3D->CreateVertexBuffer(6 * sizeof(Vertex), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_XYZ, D3DPOOL_DEFAULT, &mRectVertexBuffer, 0));

		LoadVertexShader(mGUIVertexShader, "data/shaders/gui.vso");
		LoadPixelShader(mGUIPixelShader, "data/shaders/gui.pso");

		mTextMaxChars = 8192;
		int vBufLen = mTextMaxChars * 4 * sizeof(Vertex);
		D3DCALL(pD3D->CreateVertexBuffer(vBufLen, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_TEX1 | D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &mTextVertexBuffer, nullptr));
		int iBufLen = mTextMaxChars * 2 * sizeof(Triangle);
		D3DCALL(pD3D->CreateIndexBuffer(iBufLen, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &mTextIndexBuffer, nullptr));
	}

	// Bloom stuff
	{
		float bloomWidth = width * 0.5f;
		float bloomHeight = height * 0.5f;
		mBloomDX = ruVector2(1.0f / bloomWidth, 0.0f);
		mBloomDY = ruVector2(0.0f, 1.0f / bloomHeight);
		D3DCALL(pD3D->CreateTexture(bloomWidth, bloomHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &mBloomTexture, nullptr));
		D3DCALL(mBloomTexture->GetSurfaceLevel(0, &mBloomTextureSurface));

		D3DCALL(pD3D->CreateTexture(bloomWidth, bloomHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &mBloomBlurredTexture, nullptr));
		D3DCALL(mBloomBlurredTexture->GetSurfaceLevel(0, &mBloomBlurredSurface));

		LoadPixelShader(mBloomPixelShader, "data/shaders/bloom.pso");
		LoadPixelShader(mGaussianBlurShader, "data/shaders/gaussianblur.pso");
	}

	// Create default textures
	D3DLOCKED_RECT lockedRect;
	A8R8G8B8Pixel * pixels;

	IDirect3DSurface9 * surface;
	const int defTexSize = 4;

	// Create default point light projection cube map
	{
		D3DCALL(pD3D->CreateCubeTexture(defTexSize, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &mWhiteCubeMap, nullptr));
		for (int face = 0; face < 6; ++face) {
			D3DCALL(mWhiteCubeMap->LockRect((D3DCUBEMAP_FACES)face, 0, &lockedRect, nullptr, 0));
			pixels = (A8R8G8B8Pixel*)lockedRect.pBits;
			for (int i = 0; i < defTexSize * defTexSize; ++i) {
				(*pixels++) = A8R8G8B8Pixel(255, 255, 255, 255);
			}
			D3DCALL(mWhiteCubeMap->UnlockRect((D3DCUBEMAP_FACES)face, 0));
		}
	}

	// Create default normal map
	{
		D3DCALL(pD3D->CreateTexture(defTexSize, defTexSize, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &mDefaultNormalMap, nullptr));
		D3DCALL(mDefaultNormalMap->GetSurfaceLevel(0, &surface));
		D3DCALL(surface->LockRect(&lockedRect, nullptr, 0));
		pixels = (A8R8G8B8Pixel*)lockedRect.pBits;
		for (int i = 0; i < defTexSize * defTexSize; ++i) {
			(*pixels++) = A8R8G8B8Pixel(255, 128, 128, 255);
		}
		D3DCALL(surface->UnlockRect());
		D3DCALL(surface->Release());
	}

	// Create default white texture
	{
		D3DCALL(pD3D->CreateTexture(defTexSize, defTexSize, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &mWhiteMap, nullptr));
		D3DCALL(mWhiteMap->GetSurfaceLevel(0, &surface));
		D3DCALL(surface->LockRect(&lockedRect, nullptr, 0));
		pixels = (A8R8G8B8Pixel*)lockedRect.pBits;
		for (int i = 0; i < defTexSize * defTexSize; ++i) {
			(*pixels++) = A8R8G8B8Pixel(255, 255, 255, 255);
		}
		D3DCALL(surface->UnlockRect());
		D3DCALL(surface->Release());
	}

	// SSAO Stuff
	LoadPixelShader(mSSAOPixelShader, "data/shaders/ssao.pso");

	// Shadow map cache
	{
		D3DCALL(pD3D->CreateDepthStencilSurface(1024, 1024, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, TRUE, &mCubeDepthStencilSurface, 0));
		constexpr static int cubeMapCacheSizes[mShadowMapCacheSize] = {
			1024, 1024, 1024, 1024,
			512, 512, 512, 512,
			256, 256, 256, 256,
			128, 128, 128, 128,
			64, 64, 64, 64, 64, 64, 64, 64,
			32, 32, 32, 32, 32, 32, 32, 32
		};
		for (int i = 0; i < mShadowMapCacheSize; ++i) {
			D3DCALL(pD3D->CreateCubeTexture(cubeMapCacheSizes[i], 1, D3DUSAGE_RENDERTARGET, D3DFMT_R16F, D3DPOOL_DEFAULT, &mCubeShadowMapCache[i], nullptr));
		}
	}

	// Light flare stuff
	{
		LoadVertexShader(mFlareVertexShader, "data/shaders/flare.vso");
		LoadPixelShader(mFlarePixelShader, "data/shaders/flare.pso");

		Vertex flareVertices[4];
		flareVertices[0].mTexCoord.x = 0.0f;
		flareVertices[0].mTexCoord.y = 0.0f;
		flareVertices[0].mBoneWeights.x = 1.0f;
		flareVertices[0].mBoneWeights.y = 1.0f;

		flareVertices[1].mTexCoord.x = 1.0f;
		flareVertices[1].mTexCoord.y = 0.0f;
		flareVertices[1].mBoneWeights.x = -1.0f;
		flareVertices[1].mBoneWeights.y = 1.0f;

		flareVertices[2].mTexCoord.x = 1.0f;
		flareVertices[2].mTexCoord.y = 1.0f;
		flareVertices[2].mBoneWeights.x = -1.0f;
		flareVertices[2].mBoneWeights.y = -1.0f;

		flareVertices[3].mTexCoord.x = 0.0f;
		flareVertices[3].mTexCoord.y = 1.0f;
		flareVertices[3].mBoneWeights.x = 1.0f;
		flareVertices[3].mBoneWeights.y = -1.0f;

		D3DCALL(pD3D->CreateVertexBuffer(4 * sizeof(Vertex), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_TEX1 | D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &mFlareVertexBuffer, nullptr));
		D3DCALL(mFlareVertexBuffer->Lock(0, 0, &lockedData, 0));
		memcpy(lockedData, flareVertices, sizeof(flareVertices));
		D3DCALL(mFlareVertexBuffer->Unlock());

		unsigned short flareIndices[] = { 0, 1, 2, 0, 2, 3 };
		D3DCALL(pD3D->CreateIndexBuffer(2 * sizeof(Triangle), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &mFlareIndexBuffer, nullptr));
		D3DCALL(mFlareIndexBuffer->Lock(0, 0, &lockedData, 0));
		memcpy(lockedData, flareIndices, sizeof(flareIndices));
		D3DCALL(mFlareIndexBuffer->Unlock());

		D3DCALL(D3DXCreateTextureFromFileA(pD3D, "data/textures/effects/flare.png", &mFlareTexture));
	}

	// Color grading 
	{
		LoadColorGradingMap("data/textures/colormaps/default.png");
	}
}

/*
=========
~Renderer
=========
*/
Renderer::~Renderer() {
	while (Timer::msTimerList.size()) {
		delete Timer::msTimerList.front();
	}
	for (auto & kv : CubeTexture::all) {
		delete kv.second;
	}
	Physics::DestructWorld();
}



/*
=========
RenderWorld
	- All rendering is done here
	- Firstly done deferred rendering (also does interlace rendering for transparent meshes)
	- Then goes rendering of transparent stuff, like particles and etc.
	- And finally GUI is rendered
	- Actually this method is pretty straigthforward and clear
=========
*/
void Renderer::RenderWorld() {
	// Dispatch all window messages
	MSG message;
	while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
		DispatchMessage(&message);
		if (message.message == WM_QUIT) {
			Shutdown();
		}
	}

	// Rendering could not be done when there is no camera
	shared_ptr<Camera> & camera = Camera::msCurrentCamera.lock();
	if (!camera || !mRunning) {
		return;
	}

	// Remove unreferenced nodes from mesh list and remove list if it is empty
	// This is necessary to be sure that weak_ptr<>.lock() returns valid pointer
	for (auto texGroupPairIter = mDeferredMeshMap.begin(); texGroupPairIter != mDeferredMeshMap.end(); ) {
		auto & texGroupPair = *texGroupPairIter;

		bool textureFound = texGroupPair.first == mWhiteMap;
		if (!textureFound) {
			for (auto textureGroup : Texture::msTextureList) {
				auto texture = textureGroup.second.lock();
				if (texture) {
					if (texture->GetInterface() == texGroupPair.first) {
						textureFound = true;
						break;
					}
				}
			}
		}

		if (textureFound) {
			if (texGroupPair.second.size()) {
				for (auto & meshIter = texGroupPair.second.begin(); meshIter != texGroupPair.second.end(); ) {
					if ((*meshIter).use_count()) {
						++meshIter;
					} else {
						meshIter = texGroupPair.second.erase(meshIter);
					}
				}
				++texGroupPairIter;
			} else {
				texGroupPairIter = mDeferredMeshMap.erase(texGroupPairIter);
				Log::Write("Texture-MeshGroup pair erased due to MeshGroup is empty.");
			}
		} else {
			texGroupPairIter = mDeferredMeshMap.erase(texGroupPairIter);
			Log::Write("Texture-MeshGroup pair erased due to texture does not exist anymore.");
		}
	}

	// Clear statistics
	mDIPCount = 0;
	mTextureChangeCount = 0;
	mShadersChangeCount = 0;
	mRenderedTriangleCount = 0;

	// Begin rendering
	pD3D->BeginScene();

	//********************************
	// Geometry pass
	//
	// Samplers layout: 
	//	0 - diffuse 
	//	1 - normal
	//	2 - height
	//
	// Render Target layout: 
	//	0 - depth
	//	1 - normal
	//	2 - diffuse
	GPUFloatRegisterStack gpuFloatRegisterStack;
	GPUBoolRegisterStack gpuBoolRegisterStack;

	D3DCALL(pD3D->SetRenderState(D3DRS_ZENABLE, TRUE));
	D3DCALL(pD3D->SetRenderState(D3DRS_ZWRITEENABLE, TRUE));
	D3DCALL(pD3D->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW));
	D3DCALL(pD3D->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE));

	D3DCALL(pD3D->SetTexture(0, nullptr));
	D3DCALL(pD3D->SetTexture(1, nullptr));
	D3DCALL(pD3D->SetTexture(2, nullptr));
	mTextureChangeCount += 3;

	D3DCALL(pD3D->SetRenderTarget(0, mDepthSurface));
	D3DCALL(pD3D->SetRenderTarget(1, mNormalSurface));
	D3DCALL(pD3D->SetRenderTarget(2, mDiffuseSurface));

	D3DCALL(pD3D->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0, 0, 0), 1.0, 0));

	if (mHDREnabled) {
		D3DCALL(pD3D->SetSamplerState(0, D3DSAMP_SRGBTEXTURE, TRUE));
	}

	D3DCALL(pD3D->SetVertexDeclaration(msVertexDeclaration));

	D3DCALL(pD3D->SetVertexShader(mGBufferVertexShader));
	D3DCALL(pD3D->SetPixelShader(mGBufferPixelShader));

	mShadersChangeCount += 2;

	for (auto & texGroupPair : mDeferredMeshMap) {
		IDirect3DTexture9 * pDiffuseTexture = texGroupPair.first;
		IDirect3DTexture9 * pNormalTexture = nullptr;

		// skip group, if it has no meshes
		if (texGroupPair.second.size() == 0) {
			continue;
		}

		D3DCALL(pD3D->SetTexture(0, pDiffuseTexture));
		++mTextureChangeCount;

		for (auto weakMesh : texGroupPair.second) {
			shared_ptr<Mesh> & pMesh = weakMesh.lock();

			if (!pMesh->IsHardwareBuffersGood()) {
				continue;
			}
			if (pMesh->GetVertices().size() == 0) {
				continue;
			}

			if (pMesh->mHeightTexture) {
				D3DCALL(pD3D->SetTexture(2, pMesh->mHeightTexture->GetInterface()));
				++mTextureChangeCount;
			}

			// prevent overhead with normal texture
			if (pMesh->mNormalTexture) {
				if (pMesh->mNormalTexture->GetInterface() != pNormalTexture) {
					D3DCALL(pD3D->SetTexture(1, pMesh->mNormalTexture->GetInterface()));
					++mTextureChangeCount;
					pNormalTexture = pMesh->mNormalTexture->GetInterface();
				}
			} else {
				D3DCALL(pD3D->SetTexture(1, mDefaultNormalMap));
				++mTextureChangeCount;
			}

			// render mesh
			auto & owners = pMesh->GetOwners();

			for (auto weakOwner : owners) {
				shared_ptr<SceneNode> & pOwner = weakOwner.lock();

				bool visible = pOwner->IsBone() ? false : pOwner->IsVisible();

				if (pOwner->mTwoSidedLighting) {
					D3DCALL(pD3D->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
				} else {
					D3DCALL(pD3D->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW));
				}

				if (visible && (pOwner->IsInFrustum() || pOwner->IsSkinned())) {
					camera->EnterDepthHack(pOwner->GetDepthHack());

					D3DXMATRIX world;
					if (pMesh->IsSkinned()) {
						D3DXMatrixIdentity(&world);
					} else {
						world = pOwner->GetWorldMatrix();
					}

					// Load pixel shader constants
					gpuBoolRegisterStack.Clear();
					gpuBoolRegisterStack.Push((IsParallaxEnabled() && pMesh->mHeightTexture) ? TRUE : FALSE);
					D3DCALL(pD3D->SetPixelShaderConstantB(0, gpuBoolRegisterStack.GetPointer(), gpuBoolRegisterStack.mBooleanCount));

					gpuFloatRegisterStack.Clear();
					gpuFloatRegisterStack.PushFloat(pOwner->GetAlbedo());
					gpuFloatRegisterStack.PushFloat(pMesh->GetOpacity());
					gpuFloatRegisterStack.PushFloat(camera->GetFrameBrightness() / 100.0f);
					gpuFloatRegisterStack.PushVector(camera->GetFrameColor() / 255.0f);
					D3DCALL(pD3D->SetPixelShaderConstantF(0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount));

					// Load vertex shader constants
					gpuBoolRegisterStack.Clear();
					gpuBoolRegisterStack.Push(pMesh->IsSkinned() ? TRUE : FALSE);
					gpuBoolRegisterStack.Push(pOwner->IsVegetation() ? TRUE : FALSE);
					D3DCALL(pD3D->SetVertexShaderConstantB(0, gpuBoolRegisterStack.GetPointer(), gpuBoolRegisterStack.mBooleanCount));

					static float vegetationAnimator = 0.0f;
					vegetationAnimator += 0.0001f;
					//vegetationAnimator = fmod(vegetationAnimator, 3.14159f);


					gpuFloatRegisterStack.Clear();
					gpuFloatRegisterStack.PushMatrix(world);
					gpuFloatRegisterStack.PushMatrix(world * camera->mViewProjection);
					gpuFloatRegisterStack.PushVector(camera->GetPosition());
					gpuFloatRegisterStack.PushFloat(pOwner->GetTexCoordFlow().x, pOwner->GetTexCoordFlow().y, vegetationAnimator);
					for (auto bone : pMesh->GetBones()) {
						shared_ptr<SceneNode> boneNode = bone->mNode.lock();
						if (boneNode) {
							gpuFloatRegisterStack.PushMatrix(TransformToMatrix(boneNode->GetRelativeTransform() * pOwner->GetLocalTransform()));
						}
					}
					D3DCALL(pD3D->SetVertexShaderConstantF(0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount));

					RenderMesh(pMesh);

					camera->LeaveDepthHack();
				}
			}
		}
	}

	//********************************
	// Lighting pass
	//
	// Samplers layout: 
	//	0 - depth
	//	1 - normal
	//	2 - diffuse
	//
	// Render Target layout: 
	//	0 - backbuffer, hdr buffer or temp color buffer

	D3DCALL(pD3D->SetTexture(0, nullptr));
	D3DCALL(pD3D->SetTexture(1, nullptr));
	D3DCALL(pD3D->SetTexture(2, nullptr));
	mTextureChangeCount += 3;

	D3DCALL(pD3D->SetSamplerState(0, D3DSAMP_SRGBTEXTURE, FALSE));

	// select render target 
	IDirect3DSurface9 * renderTarget = mFrameSurface[0];

	// If HDR enabled, render all in high-precision float texture
	if (mHDREnabled) {
		renderTarget = mHDRFrameSurface;
	}

	D3DCALL(pD3D->SetRenderTarget(0, renderTarget));
	D3DCALL(pD3D->SetRenderTarget(1, nullptr));
	D3DCALL(pD3D->SetRenderTarget(2, nullptr));

	D3DCALL(pD3D->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0, 0, 0), 1.0, 0));

	D3DCALL(pD3D->SetRenderState(D3DRS_ZENABLE, TRUE));
	D3DCALL(pD3D->SetRenderState(D3DRS_ZWRITEENABLE, FALSE));
	D3DCALL(pD3D->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE));
	D3DCALL(pD3D->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
	D3DCALL(pD3D->SetRenderState(D3DRS_STENCILENABLE, FALSE));

	D3DCALL(pD3D->SetPixelShader(mSkyboxPixelShader));
	D3DCALL(pD3D->SetVertexShader(mSkyboxVertexShader));
	mShadersChangeCount += 2;

	// Render Skybox
	// make sure that we do not see any seams across egdes
	D3DCALL(pD3D->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP));
	D3DCALL(pD3D->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP));

	if (camera->mSkybox) {
		D3DXMATRIX wvp;
		D3DXMatrixTranslation(&wvp, camera->GetPosition().x, camera->GetPosition().y, camera->GetPosition().z);

		gpuFloatRegisterStack.Clear();
		gpuFloatRegisterStack.PushMatrix(wvp * camera->mViewProjection);
		D3DCALL(pD3D->SetVertexShaderConstantF(0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount));

		D3DCALL(pD3D->SetIndices(mSkyboxIndexBuffer));
		D3DCALL(pD3D->SetStreamSource(0, mSkyboxVertexBuffer, 0, sizeof(Vertex)));
		for (int i = 0; i < 5; i++) {
			D3DCALL(pD3D->SetTexture(0, camera->mSkybox->mTextures[i]->GetInterface()));
			++mTextureChangeCount;

			D3DCALL(pD3D->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 20, i * 6, 2));
			++mDIPCount;
			mRenderedTriangleCount += 2;
		}
	}

	// enable wrap for diffuse textures
	D3DCALL(pD3D->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP));
	D3DCALL(pD3D->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP));

	// Begin light occlusion queries
	D3DCALL(pD3D->SetRenderState(D3DRS_COLORWRITEENABLE, 0x00000000));

	auto & pointLights = SceneFactory::GetPointLightList();
	for (auto & lWeak : pointLights) {
		shared_ptr<PointLight> & pLight = lWeak.lock();
		if (pLight) {
			if (pLight->mQueryDone) {
				if (camera->mFrustum.IsSphereInside(pLight->GetPosition(), pLight->GetRange()) && pLight->IsVisible()) {
					bool found = false;
					for (auto & lit : camera->GetNearestPathPoint()->GetListOfVisibleLights()) {
						if (lit.lock() == pLight) {
							found = true;
						}
					}
					if (!found) {
						D3DCALL(pLight->pOcclusionQuery->Issue(D3DISSUE_BEGIN));

						gpuFloatRegisterStack.Clear();
						gpuFloatRegisterStack.PushMatrix(SetUniformScaleTranslationMatrix(1.25f * pLight->mRadius, pLight->GetPosition()) * camera->mViewProjection);
						D3DCALL(pD3D->SetVertexShaderConstantF(0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount));

						D3DCALL(mBoundingStar->DrawSubset(0));
						D3DCALL(pD3D->SetVertexDeclaration(msVertexDeclaration));

						D3DCALL(pLight->pOcclusionQuery->Issue(D3DISSUE_END));
					}

					pLight->mInFrustum = true;
					pLight->mQueryDone = false;
				} else {
					pLight->mInFrustum = false;
				}
			}
		}
	}

	for (auto & lWeak : pointLights) {
		shared_ptr<PointLight> & pLight = lWeak.lock();
		if (pLight) {
			bool inFrustum = camera->mFrustum.IsSphereInside(pLight->GetPosition(), pLight->GetRange());
			if (pLight->IsVisible() && inFrustum) {
				bool found = false;
				for (auto & lit : camera->GetNearestPathPoint()->GetListOfVisibleLights()) {
					if (lit.lock() == pLight) {
						found = true;
						break;
					}
				}
				DWORD pixelsVisible = 0;
				if (pLight->mInFrustum && !pLight->mQueryDone) {
					if (!found) {
						HRESULT result = pLight->pOcclusionQuery->GetData(&pixelsVisible, sizeof(pixelsVisible), D3DGETDATA_FLUSH);
						if (result == S_OK) {
							pLight->mQueryDone = true;
							if (pixelsVisible > 0) {
								// add light to light list of nearest path point of camera									
								camera->GetNearestPathPoint()->GetListOfVisibleLights().push_back(pLight);
							}
						}
					}
				}
			}
		}
	}

	D3DCALL(pD3D->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF));

	float hdrLightIntensity = mHDREnabled ? 5.5f : 1.5f;

	// Bind G-Buffer Textures
	D3DCALL(pD3D->SetTexture(0, mDepthMap));
	D3DCALL(pD3D->SetTexture(1, mNormalMap));
	D3DCALL(pD3D->SetTexture(2, mDiffuseMap));
	mTextureChangeCount += 3;

	// Light passes rendered with additive blending
	D3DCALL(pD3D->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE));
	D3DCALL(pD3D->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE));
	D3DCALL(pD3D->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE));

	// Apply ambient lighting
	D3DCALL(pD3D->SetPixelShader(mAmbientPixelShader));
	++mShadersChangeCount;

	gpuFloatRegisterStack.Clear();
	gpuFloatRegisterStack.PushVector(mAmbientColor * (mHDREnabled ? hdrLightIntensity / 2 : 1.0f));
	D3DCALL(pD3D->SetPixelShaderConstantF(0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount));

	RenderFullscreenQuad();

	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Render point lights
	D3DCALL(pD3D->SetRenderState(D3DRS_STENCILENABLE, TRUE));

	D3DCALL(pD3D->SetPixelShader(mDeferredLightShader));
	++mShadersChangeCount;

	// sort list of visible light in order to increse distance to camera
	auto & visLightList = camera->GetNearestPathPoint()->GetListOfVisibleLights();
	sort
	(
		visLightList.begin(),
		visLightList.end(),
		[camera](const weak_ptr<PointLight> & lhs, const weak_ptr<PointLight> & rhs) -> bool {
		return (lhs.lock()->GetPosition() - camera->GetPosition()).Length2() < (rhs.lock()->GetPosition() - camera->GetPosition()).Length2();
	});

	// select proper shadow map for each light from shadow map cache
	int lightCounter = 0;
	for (auto lWeak : camera->GetNearestPathPoint()->GetListOfVisibleLights()) {
		shared_ptr<PointLight> pLight = lWeak.lock();
		if (lightCounter < mShadowMapCacheSize) {
			pLight->SetShadowMapIndex(lightCounter++);
		} else {
			pLight->SetShadowMapIndex(-1);
		}

		// also checkout for dynamic objects, that moving inside light
		auto & nodes = SceneFactory::GetNodeList();
		for (auto weakNode : nodes) {
			auto node = weakNode.lock();

			bool animated = node->GetCurrentAnimation() ? node->GetCurrentAnimation()->IsEnabled() : false;

			if ((pLight->GetPosition() - node->GetPosition()).Length2() < pLight->GetRange() * pLight->GetRange()) {
				if (node->IsMoving() || animated) {
					pLight->mNeedRecomputeShadowMap = true;
					break;
				}
			}
		}
	}

	for (auto lWeak : camera->GetNearestPathPoint()->GetListOfVisibleLights()) {
		shared_ptr<PointLight> pLight = lWeak.lock();
		if (pLight) {
			if (pLight->IsVisible()) {
				if (pLight->mInFrustum) {
					bool useShadows = false;

					if (pLight->GetShadowMapIndex() >= 0) {
						useShadows = mUsePointLightShadows;

						if (useShadows) {

							// Render shadow cube map
							if (pLight->mNeedRecomputeShadowMap) {
								D3DCALL(pD3D->SetRenderState(D3DRS_STENCILENABLE, FALSE));
								D3DCALL(pD3D->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE));
								D3DCALL(pD3D->SetRenderState(D3DRS_ZWRITEENABLE, TRUE));

								static CameraDirection directions[6] = {
									{ ruVector3(1.0f,  0.0f,  0.0f), ruVector3(0.0f, 1.0f,  0.0f) },
									{ ruVector3(-1.0f,  0.0f,  0.0f), ruVector3(0.0f, 1.0f,  0.0f) },
									{ ruVector3(0.0f,  1.0f,  0.0f), ruVector3(0.0f, 0.0f, -1.0f) },
									{ ruVector3(0.0f, -1.0f,  0.0f), ruVector3(0.0f, 0.0f,  1.0f) },
									{ ruVector3(0.0f,  0.0f,  1.0f), ruVector3(0.0f, 1.0f,  0.0f) },
									{ ruVector3(0.0f,  0.0f, -1.0f), ruVector3(0.0f, 1.0f,  0.0f) }
								};

								D3DCALL(pD3D->SetPixelShader(mShadowMapPixelShader));
								D3DCALL(pD3D->SetVertexShader(mShadowMapVertexShader));
								mShadersChangeCount += 2;

								D3DXMATRIX projectionMatrix;
								D3DXMatrixPerspectiveFovLH(&projectionMatrix, 1.570796, 1.0f, 0.05f, 1024.0f);

								for (int face = 0; face < 6; ++face) {
									IDirect3DSurface9 * faceSurface;
									D3DCALL(mCubeShadowMapCache[pLight->GetShadowMapIndex()]->GetCubeMapSurface((D3DCUBEMAP_FACES)face, 0, &faceSurface));

									D3DCALL(pD3D->SetRenderTarget(0, faceSurface));
									D3DCALL(pD3D->SetDepthStencilSurface(mCubeDepthStencilSurface));
									D3DCALL(pD3D->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0, 0, 0), 1.0, 0));

									ruVector3 eye = pLight->GetPosition();
									ruVector3 at = eye + directions[face].target;
									ruVector3 up = directions[face].up;

									D3DXMATRIX viewMatrix;
									D3DXMatrixLookAtLH(&viewMatrix, (D3DXVECTOR3*)(&eye), (D3DXVECTOR3*)(&at), (D3DXVECTOR3*)(&up));

									for (auto & texGroupPair : mDeferredMeshMap) {
										pD3D->SetTexture(7, texGroupPair.first);
										for (auto & weakMesh : texGroupPair.second) {
											shared_ptr<Mesh> mesh = weakMesh.lock();

											// semi-transparent meshes does not cast shadows - FIXME: Is this right?
											if (mesh->GetOpacity() < 0.95) {
												continue;
											}

											auto & owners = mesh->GetOwners();
											for (auto & weakOwner : owners) {
												shared_ptr<SceneNode> & pOwner = weakOwner.lock();

												if (!pOwner->IsShadowCastEnabled()) {
													continue;
												}

												if (mesh->mAABB.IsIntersectSphere(pOwner->GetPosition(), pLight->GetPosition(), pLight->GetRange()) || pOwner->IsSkinned()) {
													if ((pOwner->IsBone() ? false : pOwner->IsVisible())) {
														// Load vertex shader constants
														gpuFloatRegisterStack.Clear();
														gpuFloatRegisterStack.PushMatrix(pOwner->GetWorldMatrix() * viewMatrix * projectionMatrix);
														gpuFloatRegisterStack.PushMatrix(pOwner->GetWorldMatrix());
														for (auto bone : mesh->GetBones()) {
															shared_ptr<SceneNode> boneNode = bone->mNode.lock();
															if (boneNode) {
																gpuFloatRegisterStack.PushMatrix(TransformToMatrix(boneNode->GetRelativeTransform() * pOwner->GetLocalTransform()));
															}
														}
														pD3D->SetVertexShaderConstantF(0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount);

														gpuBoolRegisterStack.Clear();
														gpuBoolRegisterStack.Push(pOwner->IsSkinned());
														gpuBoolRegisterStack.Push(TRUE);
														pD3D->SetVertexShaderConstantB(0, gpuBoolRegisterStack.GetPointer(), gpuBoolRegisterStack.mBooleanCount);

														// Load pixel shader constants
														gpuFloatRegisterStack.Clear();
														gpuFloatRegisterStack.PushVector(pLight->GetPosition());
														pD3D->SetPixelShaderConstantF(0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount);

														gpuBoolRegisterStack.Clear();
														gpuBoolRegisterStack.Push(TRUE);
														pD3D->SetPixelShaderConstantB(0, gpuBoolRegisterStack.GetPointer(), gpuBoolRegisterStack.mBooleanCount);

														RenderMesh(mesh);
													}
												}
											}
										}
									}

									faceSurface->Release();
								}

								pLight->mNeedRecomputeShadowMap = false;
							}

							pD3D->SetRenderTarget(0, renderTarget);
							pD3D->SetDepthStencilSurface(mDefaultDepthStencil);

							pD3D->SetRenderState(D3DRS_STENCILENABLE, TRUE);
							pD3D->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
							pD3D->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

							pD3D->SetTexture(6, mCubeShadowMapCache[pLight->GetShadowMapIndex()]);
							++mTextureChangeCount;

							// Revert to light shader
							pD3D->SetPixelShader(mDeferredLightShader);
							pD3D->SetVertexShader(mQuadVertexShader);
							++mShadersChangeCount;
						}
					}

					pD3D->SetRenderState(D3DRS_COLORWRITEENABLE, 0x00000000);
					pD3D->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
					pD3D->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);

					pD3D->SetRenderState(D3DRS_ZENABLE, TRUE);

					if (pLight->GetPointTexture()) {
						pD3D->SetTexture(3, pLight->GetPointTexture()->mCubeTexture);
					} else {
						pD3D->SetTexture(3, mWhiteCubeMap);
					}
					++mTextureChangeCount;

					// Load pixel shader constants
					gpuFloatRegisterStack.Clear();
					gpuFloatRegisterStack.PushMatrix(camera->invViewProjection);
					gpuFloatRegisterStack.PushIdentityMatrix();
					gpuFloatRegisterStack.PushVector(pLight->GetPosition());
					gpuFloatRegisterStack.PushVector(pLight->GetColor());
					gpuFloatRegisterStack.PushFloat(pLight->GetRange());
					gpuFloatRegisterStack.PushFloat(hdrLightIntensity);
					gpuFloatRegisterStack.PushVector(camera->GetPosition());
					pD3D->SetPixelShaderConstantF(0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount);

					gpuBoolRegisterStack.Clear();
					gpuBoolRegisterStack.Push(TRUE);
					gpuBoolRegisterStack.Push(useShadows);
					pD3D->SetPixelShaderConstantB(0, gpuBoolRegisterStack.GetPointer(), gpuBoolRegisterStack.mBooleanCount);

					// Load vertex shader constants 
					gpuFloatRegisterStack.Clear();
					gpuFloatRegisterStack.PushMatrix(SetUniformScaleTranslationMatrix(1.5f * pLight->mRadius, pLight->GetPosition()) * camera->mViewProjection);
					pD3D->SetVertexShaderConstantF(0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount);

					// Render bounding sphere
					mBoundingSphere->DrawSubset(0);
					pD3D->SetVertexDeclaration(msVertexDeclaration);

					// Revert stencil
					pD3D->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);
					pD3D->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL);
					pD3D->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_ZERO);

					// Apply lighting to pixels, that marked by bounding sphere
					RenderFullscreenQuad();
				}
			}
		}
	}

	// Render spot lights
	pD3D->SetPixelShader(mDeferredLightShader);
	++mShadersChangeCount;

	auto & spotLights = SceneFactory::GetSpotLightList();
	for (auto & lWeak : spotLights) {
		shared_ptr<SpotLight> & pLight = lWeak.lock();
		if (pLight) {
			if (camera->mFrustum.IsSphereInside(pLight->GetPosition(), pLight->GetRange()) && pLight->IsVisible()) {
				// If shadows enabled, render shadowmap first
				if (IsSpotLightShadowsEnabled()) {
					pD3D->SetRenderState(D3DRS_STENCILENABLE, FALSE);
					pD3D->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
					pD3D->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

					pD3D->SetTexture(4, nullptr);
					++mTextureChangeCount;

					pD3D->SetRenderTarget(0, mShadowMapSurface);
					pD3D->SetDepthStencilSurface(mDepthStencilSurface);
					pD3D->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0, 0, 0), 1.0, 0);

					pD3D->SetPixelShader(mShadowMapPixelShader);
					pD3D->SetVertexShader(mShadowMapVertexShader);
					mShadersChangeCount += 2;

					pLight->BuildSpotProjectionMatrixAndFrustum();

					for (auto & texGroupPair : mDeferredMeshMap) {
						pD3D->SetTexture(7, texGroupPair.first);
						for (auto & weakMesh : texGroupPair.second) {
							shared_ptr<Mesh> mesh = weakMesh.lock();
							auto & owners = mesh->GetOwners();
							for (auto & weakOwner : owners) {
								shared_ptr<SceneNode> & pOwner = weakOwner.lock();

								if (pOwner->IsBone()) {
									continue;
								}

								if (pOwner->IsVisible() && pLight->GetFrustum().IsAABBInside(mesh->GetBoundingBox(), pOwner->GetPosition(), pOwner->GetWorldMatrix())) {
									// Load vertex shader constants
									gpuFloatRegisterStack.Clear();
									gpuFloatRegisterStack.PushMatrix(pOwner->GetWorldMatrix() * pLight->GetViewProjectionMatrix());
									gpuFloatRegisterStack.PushMatrix(pOwner->GetWorldMatrix());
									for (auto bone : mesh->GetBones()) {
										shared_ptr<SceneNode> boneNode = bone->mNode.lock();
										if (boneNode) {
											gpuFloatRegisterStack.PushMatrix(TransformToMatrix(boneNode->GetRelativeTransform() * pOwner->GetLocalTransform()));
										}
									}
									pD3D->SetVertexShaderConstantF(0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount);

									gpuBoolRegisterStack.Clear();
									gpuBoolRegisterStack.Push(pOwner->IsSkinned());
									gpuBoolRegisterStack.Push(FALSE);
									pD3D->SetVertexShaderConstantB(0, gpuBoolRegisterStack.GetPointer(), gpuBoolRegisterStack.mBooleanCount);

									// Load pixel shader constants
									gpuFloatRegisterStack.Clear();
									gpuFloatRegisterStack.PushVector(pLight->GetPosition());
									pD3D->SetPixelShaderConstantF(0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount);

									gpuBoolRegisterStack.Clear();
									gpuBoolRegisterStack.Push(FALSE);
									pD3D->SetPixelShaderConstantB(0, gpuBoolRegisterStack.GetPointer(), gpuBoolRegisterStack.mBooleanCount);

									RenderMesh(mesh);
								}
							}
						}
					}

					pD3D->SetRenderTarget(0, renderTarget);
					pD3D->SetDepthStencilSurface(mDefaultDepthStencil);

					pD3D->SetRenderState(D3DRS_STENCILENABLE, TRUE);
					pD3D->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
					pD3D->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

					pD3D->SetTexture(5, mShadowMap);
					++mTextureChangeCount;

					// Revert to light shader
					pD3D->SetPixelShader(mDeferredLightShader);
					pD3D->SetVertexShader(mQuadVertexShader);
					++mShadersChangeCount;
				}

				pD3D->SetTexture(4, pLight->GetSpotTexture()->GetInterface());
				++mTextureChangeCount;

				pD3D->SetRenderState(D3DRS_COLORWRITEENABLE, 0x00000000);
				pD3D->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
				pD3D->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);

				// Setup bounding cone properties
				D3DXMATRIX scale;
				float height = pLight->GetRange() * 2.5;
				float radius = height * sinf((pLight->GetOuterAngle() * 0.75f) * SIMD_PI / 180.0f);
				D3DXMatrixScaling(&scale, radius, height, radius);

				// Load vertex shader constants
				gpuFloatRegisterStack.Clear();
				gpuFloatRegisterStack.PushMatrix((scale * pLight->GetWorldMatrix()) * camera->mViewProjection);
				pD3D->SetVertexShaderConstantF(0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount);

				// Render Oriented Bounding Cone
				mBoundingCone->DrawSubset(0);
				pD3D->SetVertexDeclaration(msVertexDeclaration);

				pLight->BuildSpotProjectionMatrixAndFrustum();

				// Load pixel shader constants
				gpuFloatRegisterStack.Clear();
				gpuFloatRegisterStack.PushMatrix(camera->invViewProjection);
				gpuFloatRegisterStack.PushMatrix(pLight->GetViewProjectionMatrix());
				gpuFloatRegisterStack.PushVector(pLight->GetPosition());
				gpuFloatRegisterStack.PushVector(pLight->GetColor());
				gpuFloatRegisterStack.PushFloat(pLight->GetRange());
				gpuFloatRegisterStack.PushFloat(hdrLightIntensity);
				pD3D->SetPixelShaderConstantF(0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount);

				gpuBoolRegisterStack.Clear();
				gpuBoolRegisterStack.Push(FALSE);
				gpuBoolRegisterStack.Push(mUseSpotLightShadows);
				pD3D->SetPixelShaderConstantB(0, gpuBoolRegisterStack.GetPointer(), gpuBoolRegisterStack.mBooleanCount);

				pD3D->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);
				pD3D->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL);
				pD3D->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_ZERO);

				// Apply lighting to pixels, that marked by bounding cone
				RenderFullscreenQuad();
			}
		}
	}

	// Apply post-effects (HDR, FXAA, and so on)
	IDirect3DTexture9 * finalFrame = mFrame[0];

	pD3D->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	pD3D->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pD3D->SetRenderState(D3DRS_ZENABLE, FALSE);

	SetGenericSamplersFiltration(D3DTEXF_POINT, true);

	// Do HDR
	if (mHDREnabled) {
		// Build bloom texture
		pD3D->SetRenderTarget(0, mBloomTextureSurface);
		pD3D->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0, 0);

		pD3D->SetPixelShader(mBloomPixelShader);
		++mShadersChangeCount;

		pD3D->SetTexture(0, mHDRFrame);
		++mTextureChangeCount;

		gpuFloatRegisterStack.Clear();
		gpuFloatRegisterStack.PushVector(mBloomDX);
		gpuFloatRegisterStack.PushVector(mBloomDY);
		pD3D->SetPixelShaderConstantF(0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount);

		RenderFullscreenQuad();

		// Blur bloom vertically
		pD3D->SetRenderTarget(0, mBloomBlurredSurface);
		pD3D->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0, 0);

		pD3D->SetTexture(0, mBloomTexture);
		++mTextureChangeCount;

		pD3D->SetPixelShader(mGaussianBlurShader);
		++mShadersChangeCount;

		gpuFloatRegisterStack.Clear();
		gpuFloatRegisterStack.PushVector(mBloomDY);
		pD3D->SetPixelShaderConstantF(0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount);

		RenderFullscreenQuad();

		// Blur bloom horizontally
		pD3D->SetRenderTarget(0, mBloomTextureSurface);
		pD3D->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0, 0);

		pD3D->SetTexture(0, mBloomBlurredTexture);
		++mTextureChangeCount;

		pD3D->SetPixelShader(mGaussianBlurShader);
		++mShadersChangeCount;

		gpuFloatRegisterStack.Clear();
		gpuFloatRegisterStack.PushVector(mBloomDX);
		pD3D->SetPixelShaderConstantF(0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount);

		RenderFullscreenQuad();

		// Calculate HDR-frame luminance
		pD3D->SetTexture(0, mHDRFrame);
		++mTextureChangeCount;

		pD3D->SetRenderTarget(0, mScaledSceneSurf);
		pD3D->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0, 0);

		pD3D->SetPixelShader(mScaleScenePixelShader);
		++mShadersChangeCount;

		RenderFullscreenQuad();

		pD3D->SetPixelShader(mDownScalePixelShader);
		++mShadersChangeCount;

		gpuFloatRegisterStack.Clear();
		gpuFloatRegisterStack.PushFloat(1.0f / mResWidth);
		pD3D->SetPixelShaderConstantF(0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount);

		pD3D->SetTexture(0, mScaledScene);
		++mTextureChangeCount;

		for (int i = 0; i < mHDRDownSampleCount; i++) {
			float pixelSize = 1.0f / static_cast<float>(IntegerPow(2, mHDRDownSampleCount - i));

			pD3D->SetRenderTarget(0, mDownSampSurf[i]);
			pD3D->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0, 0);

			gpuFloatRegisterStack.Clear();
			gpuFloatRegisterStack.PushFloat(pixelSize);
			pD3D->SetPixelShaderConstantF(0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount);

			RenderFullscreenQuad();

			pD3D->SetTexture(0, mDownSampTex[i]);
			++mTextureChangeCount;
		}

		// Do adaptation (!actual adaptation speed depends on the FPS, so this must be fixed!)
		IDirect3DTexture9 * pTexSwap = mAdaptedLuminanceLast;
		mAdaptedLuminanceLast = mAdaptedLuminanceCurrent;
		mAdaptedLuminanceCurrent = pTexSwap;

		IDirect3DSurface9 * pSurfAdaptedLum = NULL;
		mAdaptedLuminanceCurrent->GetSurfaceLevel(0, &pSurfAdaptedLum);

		pD3D->SetRenderTarget(0, pSurfAdaptedLum);
		pD3D->SetTexture(0, mDownSampTex[mHDRDownSampleCount - 1]);
		++mTextureChangeCount;
		pD3D->SetTexture(1, mAdaptedLuminanceLast);
		++mTextureChangeCount;


		pD3D->SetPixelShader(mAdaptationPixelShader);
		++mShadersChangeCount;

		// Set pixel shader constants
		gpuFloatRegisterStack.Clear();
		gpuFloatRegisterStack.PushFloat(0.025f);
		pD3D->SetPixelShaderConstantF(0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount);

		RenderFullscreenQuad();

		pSurfAdaptedLum->Release();

		// And finally do tone-mapping
		pD3D->SetRenderTarget(0, mFrameSurface[0]);
		pD3D->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0, 0, 0), 1.0, 0);

		pD3D->SetTexture(0, mHDRFrame);
		++mTextureChangeCount;
		pD3D->SetTexture(1, mAdaptedLuminanceCurrent);
		++mTextureChangeCount;
		pD3D->SetTexture(2, mBloomTexture);
		++mTextureChangeCount;
		pD3D->SetTexture(3, mColorMap);
		++mTextureChangeCount;

		pD3D->SetPixelShader(mToneMapShader);
		++mShadersChangeCount;

		RenderFullscreenQuad();

		finalFrame = mFrame[0];
	};

	// Do blending for transparent lines (remember interlace rendering for transparent objects)
	pD3D->SetRenderTarget(0, mFrameSurface[1]);
	pD3D->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0, 0, 0), 1.0, 0);

	pD3D->SetTexture(0, finalFrame);
	++mTextureChangeCount;

	pD3D->SetTexture(1, mDiffuseMap);
	++mTextureChangeCount;

	pD3D->SetPixelShader(mDeferredBlending);
	++mShadersChangeCount;

	gpuFloatRegisterStack.Clear();
	gpuFloatRegisterStack.PushFloat(1.0f / GetResolutionHeight());
	pD3D->SetPixelShaderConstantF(0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount);

	RenderFullscreenQuad();

	finalFrame = mFrame[1];

	// Do FXAA
	if (mFXAAEnabled) {
		pD3D->SetRenderTarget(0, (finalFrame == mFrame[1]) ? mFrameSurface[0] : mFrameSurface[1]);
		pD3D->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0, 0, 0), 1.0, 0);

		pD3D->SetTexture(0, finalFrame);
		++mTextureChangeCount;

		pD3D->SetPixelShader(mFXAAPixelShader);
		++mShadersChangeCount;

		gpuFloatRegisterStack.Clear();
		gpuFloatRegisterStack.PushFloat(GetResolutionWidth(), GetResolutionHeight());
		pD3D->SetPixelShaderConstantF(0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount);

		RenderFullscreenQuad();

		finalFrame = (finalFrame == mFrame[1]) ? mFrame[0] : mFrame[1];
	}

	// Do SSAO
	if (mSSAOEnabled) {
		pD3D->SetRenderTarget(0, (finalFrame == mFrame[1]) ? mFrameSurface[0] : mFrameSurface[1]);
		pD3D->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0, 0, 0), 1.0, 0);

		pD3D->SetTexture(0, mDepthMap);
		++mTextureChangeCount;

		pD3D->SetTexture(1, mNormalMap);
		++mTextureChangeCount;

		pD3D->SetTexture(2, finalFrame);
		++mTextureChangeCount;

		finalFrame = (finalFrame == mFrame[1]) ? mFrame[0] : mFrame[1];
	}

	// Postprocessing
	//if (mHDREnabled) {
	//	pD3D->SetRenderState(D3DRS_SRGBWRITEENABLE, TRUE);
	//}

	pD3D->SetRenderTarget(0, mpBackBuffer);
	pD3D->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0, 0);

	pD3D->SetTexture(0, finalFrame);
	++mTextureChangeCount;

	pD3D->SetPixelShader(mSkyboxPixelShader);
	++mShadersChangeCount;

	RenderFullscreenQuad();

	if (IsAnisotropicFilteringEnabled()) {
		SetGenericSamplersFiltration(D3DTEXF_ANISOTROPIC, false);
	} else {
		SetGenericSamplersFiltration(D3DTEXF_LINEAR, false);
	}

	//pD3D->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);


	//********************************
	// Forward rendering for transparent meshes

	pD3D->SetRenderState(D3DRS_ZENABLE, TRUE);

	pD3D->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pD3D->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pD3D->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	pD3D->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	pD3D->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pD3D->SetRenderState(D3DRS_STENCILENABLE, FALSE);

	pD3D->SetPixelShader(mParticleSystemPixelShader);
	++mShadersChangeCount;

	ruVector3 rightVect = ruVector3(camera->mView._11, camera->mView._21, camera->mView._31).Normalize();
	ruVector3 upVect = ruVector3(camera->mView._12, camera->mView._22, camera->mView._32).Normalize();
	ruVector3 frontVect = ruVector3(camera->mView._13, camera->mView._23, camera->mView._33).Normalize();

	auto & particleSystems = SceneFactory::GetParticleSystemList();
	for (auto pWeak : particleSystems) {
		shared_ptr<ParticleSystem> & particleEmitter = pWeak.lock();

		if (!particleEmitter->IsVisible() || !particleEmitter->IsEnabled() || !particleEmitter->HasAliveParticles()) {
			continue;
		}

		if (particleEmitter->mTexture) {
			pD3D->SetTexture(0, particleEmitter->mTexture->GetInterface());
			++mTextureChangeCount;
		}

		pD3D->SetStreamSource(0, particleEmitter->mVertexBuffer, 0, sizeof(Vertex));
		pD3D->SetIndices(particleEmitter->mIndexBuffer);

		camera->EnterDepthHack(particleEmitter->GetDepthHack());

		pD3D->SetVertexShader(mParticleSystemVertexShader);
		++mShadersChangeCount;

		// pass float constants to vertex shader
		gpuFloatRegisterStack.Clear();
		gpuFloatRegisterStack.PushMatrix(camera->mViewProjection);
		gpuFloatRegisterStack.PushMatrix(particleEmitter->GetWorldMatrix());
		gpuFloatRegisterStack.PushFloat(upVect.x, upVect.y, upVect.z, 0.0f);
		gpuFloatRegisterStack.PushFloat(rightVect.x, rightVect.y, rightVect.z, 0.0f);
		gpuFloatRegisterStack.PushFloat(frontVect.x, frontVect.y, frontVect.z, 0.0f);

		pD3D->SetVertexShaderConstantF(0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount);

		// pass boolean constants to pixel shader
		gpuBoolRegisterStack.Clear();
		gpuBoolRegisterStack.Push(particleEmitter->IsLightingEnabled());

		pD3D->SetPixelShaderConstantB(0, gpuBoolRegisterStack.GetPointer(), gpuBoolRegisterStack.mBooleanCount);

		// this part must be refactored to support lighting by more than one light source
		if (particleEmitter->mUseLighting) {
			// find closest light source
			shared_ptr<ruLight> closestLightSource;
			float lastDistance = FLT_MAX;

			// check out point lights first
			auto & pointLights = SceneFactory::GetPointLightList();
			for (auto weakLight : pointLights) {
				auto light = weakLight.lock();
				float distance = (light->GetPosition() - particleEmitter->GetPosition()).Length2();
				if (distance < lastDistance) {
					closestLightSource = light;
					lastDistance = distance;
				}
			}

			// and finally check out spot lights
			auto & spotLights = SceneFactory::GetPointLightList();
			for (auto weakLight : spotLights) {
				auto light = weakLight.lock();
				float distance = (light->GetPosition() - particleEmitter->GetPosition()).Length2();
				if (distance < lastDistance) {
					closestLightSource = light;
					lastDistance = distance;
				}
			}

			// pass closest light info to pixel shader
			if (closestLightSource) {
				gpuFloatRegisterStack.Clear();
				gpuFloatRegisterStack.PushVector(closestLightSource->GetPosition());
				gpuFloatRegisterStack.PushVector(closestLightSource->GetColor());
				gpuFloatRegisterStack.PushFloat(closestLightSource->GetRange());

				pD3D->SetPixelShaderConstantF(0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount);
			}
		}

		++mDIPCount;
		pD3D->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, particleEmitter->mAliveParticleCount * 4, 0, particleEmitter->mAliveParticleCount * 2);
		mRenderedTriangleCount += particleEmitter->mAliveParticleCount * 2;

		camera->LeaveDepthHack();
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Render light flares without z-buffering

	// make sure that we do not see any seams across egdes
	pD3D->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	pD3D->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	pD3D->SetRenderState(D3DRS_ZENABLE, FALSE);


	pD3D->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pD3D->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	pD3D->SetPixelShader(mFlarePixelShader);
	pD3D->SetVertexShader(mFlareVertexShader);
	mShadersChangeCount += 2;

	pD3D->SetTexture(0, mFlareTexture);
	++mTextureChangeCount;

	for (auto & weakLight : pointLights) {
		auto light = weakLight.lock();

		if (light->IsVisible()) {

			if (!light->IsDrawFlare()) {
				continue;
			}

			auto picked = ((light->GetPosition() - camera->GetPosition()).Length2() > 0.001) ? ruPhysics::CastRay(light->GetPosition(), camera->GetPosition()) : nullptr;
			bool show = picked ? (picked->GetBodyType() == BodyType::Capsule) : false;
			if (show) {
				float size = (light->GetPosition() - camera->GetPosition()).Length() * 0.08f;
				if (size > 1) {
					size = 1;
				}
				if (size < 0.5) {
					size = 0.5;
				}

				pD3D->SetStreamSource(0, mFlareVertexBuffer, 0, sizeof(Vertex));
				pD3D->SetIndices(mFlareIndexBuffer);

				// pass float constants to vertex shader
				gpuFloatRegisterStack.Clear();
				gpuFloatRegisterStack.PushMatrix(camera->mViewProjection);
				gpuFloatRegisterStack.PushMatrix(light->GetWorldMatrix());
				gpuFloatRegisterStack.PushFloat(upVect.x, upVect.y, upVect.z, 0.0f);
				gpuFloatRegisterStack.PushFloat(rightVect.x, rightVect.y, rightVect.z, 0.0f);
				gpuFloatRegisterStack.PushFloat(frontVect.x, frontVect.y, frontVect.z, 0.0f);
				gpuFloatRegisterStack.PushFloat(light->GetColor().x, light->GetColor().y, light->GetColor().z, size);
				gpuFloatRegisterStack.PushFloat(size);
				pD3D->SetVertexShaderConstantF(0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount);

				pD3D->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
				mRenderedTriangleCount += 2;
				++mDIPCount;
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Render gui on top of all    

	pD3D->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pD3D->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	pD3D->SetPixelShader(mGUIPixelShader);
	pD3D->SetVertexShader(mGUIVertexShader);
	mShadersChangeCount += 2;

	// Load vertex shader matrices
	gpuFloatRegisterStack.Clear();
	gpuFloatRegisterStack.PushMatrix(mOrthoProjectionMatrix);
	pD3D->SetVertexShaderConstantF(0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount);

	// Render gui scenes
	auto & guiSceneList = GUIScene::GetSceneList();
	for (auto & weakScene : guiSceneList) {
		auto scene = weakScene.lock();

		// do not render if invisible
		if (!scene->IsVisible()) {
			continue;
		}

		// consistent rendering is quite inefficient, but layer management is easy
		auto & nodeList = scene->GetNodeList();
		for (auto & node : nodeList) {
			if (node->IsVisible()) {
				auto rect = dynamic_pointer_cast<GUIRect>(node);
				auto text = dynamic_pointer_cast<GUIText>(node);
				if (rect) {
					RenderRect(rect);
				} else if (text) {
					Vertex * vertices = nullptr;
					Triangle * triangles = nullptr;
					int n = 0;
					int totalLetters = 0;

					shared_ptr<BitmapFont> & font = text->GetFont();

					mTextVertexBuffer->Lock(0, mTextMaxChars * 4 * sizeof(Vertex), reinterpret_cast<void**>(&vertices), D3DLOCK_DISCARD);
					mTextIndexBuffer->Lock(0, mTextMaxChars * 2 * sizeof(Triangle), reinterpret_cast<void**>(&triangles), D3DLOCK_DISCARD);

					ruVector2 position = text->GetGlobalPosition();
					ruVector2 caret = position;
					for (auto & line : text->mLines) {
						caret.x = line.mX + position.x;
						caret.y = line.mY + position.y;

						for (int k = 0; k < line.mSubstring.length(); ++k) {
							int offsetX, offsetY;
							int stepX, stepY;
							int width, height;
							gft_texcoord_t texCoords[4];
							char32_t symbol = line.mSubstring[k];

							gft_glyph_get_caret_offset_x(font->mFont, symbol, &offsetX);
							gft_glyph_get_caret_offset_y(font->mFont, symbol, &offsetY);
							gft_glyph_get_caret_step_x(font->mFont, symbol, &stepX);
							gft_glyph_get_caret_step_y(font->mFont, symbol, &stepY);
							gft_glyph_get_width(font->mFont, symbol, &width);
							gft_glyph_get_height(font->mFont, symbol, &height);
							gft_glyph_get_texcoords(font->mFont, symbol, texCoords);

							int x = caret.x + offsetX;
							int y = caret.y + offsetY;

							float alpha = text->IsIndependentAlpha() ? text->GetAlpha() / 255.0f : scene->GetOpacity() * text->GetAlpha() / 255.0f;

							// vertices
							*(vertices++) = Vertex(ruVector3(x, y, 0.0f), ruVector2(texCoords[0].x, texCoords[0].y), ruVector4(text->GetColor(), alpha));
							*(vertices++) = Vertex(ruVector3(x + width, y, 0.0f), ruVector2(texCoords[1].x, texCoords[1].y), ruVector4(text->GetColor(), alpha));
							*(vertices++) = Vertex(ruVector3(x + width, y + height, 0.0f), ruVector2(texCoords[2].x, texCoords[2].y), ruVector4(text->GetColor(), alpha));
							*(vertices++) = Vertex(ruVector3(x, y + height, 0.0f), ruVector2(texCoords[3].x, texCoords[3].y), ruVector4(text->GetColor(), alpha));

							// indices
							*(triangles++) = Triangle(n, n + 1, n + 2);
							*(triangles++) = Triangle(n, n + 2, n + 3);

							n += 4;
							totalLetters++;

							if (symbol != '\n') {
								caret.x += stepX;
							}
						}
					}

					mTextVertexBuffer->Unlock();
					mTextIndexBuffer->Unlock();

					if (totalLetters > 0) {
						pD3D->SetTexture(0, font->mAtlas);
						++mTextureChangeCount;

						pD3D->SetStreamSource(0, mTextVertexBuffer, 0, sizeof(Vertex));
						pD3D->SetIndices(mTextIndexBuffer);

						pD3D->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, totalLetters * 4, 0, totalLetters * 2);
						mRenderedTriangleCount += totalLetters * 2;
						++mDIPCount;
					}
				}
			}
		}
	}

	// Render Cursor on top of all
	if (GetCursor()) {
		pD3D->SetStreamSource(0, mRectVertexBuffer, 0, sizeof(Vertex));
		if (GetCursor()->IsVisible()) {
			GetCursor()->SetPosition(ruInput::GetMouseX() / GetGUIWidthScaleFactor(), ruInput::GetMouseY() / GetGUIHeightScaleFactor());
			RenderRect(GetCursor());
		}
	}


	pD3D->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	pD3D->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	// End rendering
	pD3D->EndScene();
	pD3D->PresentEx(nullptr, nullptr, nullptr, nullptr, 0);
}

/*
=========
UpdateWorld
	- This method should be called in game update cycle (independent from rendering) with a fixed period
	- Performs global transforms calculations of all scene nodes
	- Performs keyframe and skeletal animation
=========
*/
void Renderer::UpdateWorld() {
	// build view and projection matrices, frustum, also attach sound listener to camera
	shared_ptr<Camera> & camera = Camera::msCurrentCamera.lock();
	if (camera) {
		camera->Update();
	}

	auto & nodes = SceneFactory::GetNodeList();

	for (auto & pWeak : nodes) {
		shared_ptr<SceneNode> & node = pWeak.lock();
		if (node) {
			node->CalculateGlobalTransform();
			// update all sounds attached to node, and physical interaction sounds( roll, hit )
			node->UpdateSounds();
			if (!node->IsSkinned()) {
				node->PerformAnimation();
			}
			if (camera) {
				node->CheckFrustum(camera.get());
			}
		}
	}

	// skinned animation is based on transforms of other nodes, so skin meshes in the end of all
	for (auto & pWeak : nodes) {
		shared_ptr<SceneNode> & node = pWeak.lock();
		if (node) {
			if (node->IsSkinned()) {
				node->PerformAnimation();
			}
		}
	}

	// update each gui scene
	auto & guiSceneList = GUIScene::GetSceneList();
	for (auto weakScene : guiSceneList) {
		auto scene = weakScene.lock();
		// update only if visible
		if (scene->IsVisible()) {
			auto & guiNodes = scene->GetNodeList();
			for (auto & pNode : guiNodes) {
				pNode->DoActions();
			}
		}
	}

	// update particle emitters
	auto & particleSystems = SceneFactory::GetParticleSystemList();
	for (auto pWeak : particleSystems) {
		shared_ptr<ParticleSystem> & particleEmitter = pWeak.lock();

		if (!particleEmitter->IsVisible() || !particleEmitter->IsEnabled() || !particleEmitter->HasAliveParticles()) {
			continue;
		}

		particleEmitter->Update();
	}

	// Update sound subsystem
	pfSystemUpdate();

	if (camera) {
		camera->Update();
	}
}

void Renderer::ChangeVideomode(int width, int height, bool fullscreen, bool vsync) {
	if (width == 0) {
		width = mNativeResolutionWidth;
	}
	if (height == 0) {
		height = mNativeResolutionHeight;
	}

	mResWidth = width;
	mResHeight = height;

	D3DDISPLAYMODE displayMode = { 0 };
	mpDirect3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode);

	// present parameters
	memset(&mPresentParameters, 0, sizeof(mPresentParameters));

	mPresentParameters.BackBufferCount = 2;
	mPresentParameters.EnableAutoDepthStencil = TRUE;
	if (vsync) {
		mPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	} else {
		mPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}
	mPresentParameters.AutoDepthStencilFormat = D3DFMT_D24S8;
	mPresentParameters.BackBufferWidth = width;
	mPresentParameters.BackBufferHeight = height;
	mPresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	mPresentParameters.hDeviceWindow = mWindowHandle;
	if (fullscreen) {
		mPresentParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
		mPresentParameters.Windowed = FALSE;
		mPresentParameters.FullScreen_RefreshRateInHz = displayMode.RefreshRate;
	} else {
		mPresentParameters.BackBufferFormat = displayMode.Format;
		mPresentParameters.Windowed = TRUE;
	}

	// no multisampling, because of deferred shading
	mPresentParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
	mPresentParameters.MultiSampleQuality = 0;

	if (fullscreen) {
		SetWindowLongPtr(mWindowHandle, GWL_STYLE, WS_POPUP);
		SetWindowPos(mWindowHandle, HWND_TOP, 0, 0, width, height, SWP_SHOWWINDOW);
	} else {
		RECT rect = { 0, 0, width, height };
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
		SetWindowLongPtr(mWindowHandle, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		SetWindowPos(mWindowHandle, HWND_TOP, 0, 0, rect.right, rect.bottom, SWP_SHOWWINDOW);
	}
}

void Renderer::SetSpotLightShadowMapSize(int size) {

}

int Renderer::GetDIPCount() {
	return mDIPCount;
}

float Renderer::GetResolutionHeight() {
	return mResHeight;
}

float Renderer::GetResolutionWidth() {
	return mResWidth;
}

bool Renderer::IsSpotLightShadowsEnabled() {
	return mUseSpotLightShadows;
}

bool Renderer::IsPointLightShadowsEnabled() {
	return mUsePointLightShadows;
}

void Renderer::SetSpotLightShadowsEnabled(bool state) {
	mUseSpotLightShadows = state;
}

void Renderer::SetPointLightShadowsEnabled(bool state) {
	mUsePointLightShadows = state;
}

void Renderer::SetAmbientColor(ruVector3 ambColor) {
	mAmbientColor = ambColor;
}

ruVector3 Renderer::GetAmbientColor() {
	return mAmbientColor;
}

void Renderer::SetHDREnabled(bool state) {
	mHDREnabled = state;
}

bool Renderer::IsHDREnabled() {
	return mHDREnabled;
}

void Renderer::SetFXAAEnabled(bool state) {
	mFXAAEnabled = state;
}

float Renderer::GetGUIHeightScaleFactor() const {
	return mResHeight / ruVirtualScreenHeight;
}

bool Renderer::IsFXAAEnabled() {
	return mFXAAEnabled;
}

void Renderer::Shutdown() {
	mRunning = false;
}

shared_ptr<Cursor> & Renderer::GetCursor() {
	return mCursor;
}

int Renderer::GetTextureChangeCount() {
	return mTextureChangeCount;
}

int Renderer::GetShaderChangeCount() {
	return mShadersChangeCount;
}

void Renderer::SetTextureStoragePath(const string & path) {
	mTextureStoragePath = path;
}

std::string Renderer::GetTextureStoragePath() {
	return mTextureStoragePath;
}

void Renderer::SetGenericSamplersFiltration(D3DTEXTUREFILTERTYPE filter, bool disableMips) {
	// number of generic samplers (i.e. for diffuse, normal and height textures )
	const int genericSamplersCount = 5;

	if (filter == D3DTEXF_NONE) { // invalid argument to min and mag filters
		for (int i = 0; i < genericSamplersCount; i++) {
			D3DCALL(pD3D->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR));
			D3DCALL(pD3D->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR));
		}
	} else if (filter == D3DTEXF_LINEAR) {
		for (int i = 0; i < genericSamplersCount; i++) {
			D3DCALL(pD3D->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR));
			D3DCALL(pD3D->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR));
		}
	} else if (filter == D3DTEXF_ANISOTROPIC) {
		D3DCALL(pD3D->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC));
		D3DCALL(pD3D->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR));
		// it's too expensive to set anisotropic filtration to normal and height maps, so set linear
		for (int i = 1; i < genericSamplersCount; i++) {
			D3DCALL(pD3D->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR));
			D3DCALL(pD3D->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR));
		}
	}

	// mip filters
	if (filter == D3DTEXF_NONE || disableMips) {
		for (int i = 0; i < genericSamplersCount; i++) {
			D3DCALL(pD3D->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_NONE));
		}
	} else if (filter == D3DTEXF_POINT) {
		for (int i = 0; i < genericSamplersCount; i++) {
			D3DCALL(pD3D->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_POINT));
		}
	} else if (filter == D3DTEXF_LINEAR || filter == D3DTEXF_ANISOTROPIC) {
		for (int i = 0; i < genericSamplersCount; i++) {
			D3DCALL(pD3D->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR));
		}
	}
}

void Renderer::SetAnisotropicTextureFiltration(bool state) {
	mAnisotropicFiltering = state;
}

bool Renderer::IsAnisotropicFilteringEnabled() {
	return mAnisotropicFiltering;
}

void Renderer::SetDefaults() {
	pD3D->SetRenderState(D3DRS_LIGHTING, FALSE);
	pD3D->SetRenderState(D3DRS_ZENABLE, TRUE);
	pD3D->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	pD3D->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pD3D->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	pD3D->SetRenderState(D3DRS_STENCILREF, 0x0);
	pD3D->SetRenderState(D3DRS_STENCILMASK, 0xFFFFFFFF);
	pD3D->SetRenderState(D3DRS_STENCILWRITEMASK, 0xFFFFFFFF);
	pD3D->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, TRUE);
	pD3D->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
	pD3D->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_DECR);
	pD3D->SetRenderState(D3DRS_CCW_STENCILZFAIL, D3DSTENCILOP_INCR);

	// setup samplers
	pD3D->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	pD3D->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pD3D->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	pD3D->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	pD3D->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pD3D->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	pD3D->SetSamplerState(3, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	pD3D->SetSamplerState(3, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pD3D->SetSamplerState(3, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	pD3D->SetSamplerState(4, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	pD3D->SetSamplerState(4, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	D3DCAPS9 dCaps;
	mpDirect3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &dCaps);
	if (dCaps.MaxAnisotropy > 4) {
		dCaps.MaxAnisotropy = 4;
	}

	pD3D->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, dCaps.MaxAnisotropy);
	pD3D->SetSamplerState(1, D3DSAMP_MAXANISOTROPY, dCaps.MaxAnisotropy);
	pD3D->SetSamplerState(2, D3DSAMP_MAXANISOTROPY, dCaps.MaxAnisotropy);
	pD3D->SetSamplerState(3, D3DSAMP_MAXANISOTROPY, dCaps.MaxAnisotropy);

	SetAnisotropicTextureFiltration(true);

	SetParallaxEnabled(true);
}

void Renderer::RenderMesh(const shared_ptr<Mesh> & mesh) {
	pD3D->SetStreamSource(0, mesh->mVertexBuffer, 0, sizeof(Vertex));
	pD3D->SetIndices(mesh->mIndexBuffer);

	mpDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, mesh->mVertices.size(), 0, mesh->mTriangles.size());
	mRenderedTriangleCount += mesh->mTriangles.size();
	++mDIPCount;
}


DWORD * Renderer::ReadEntireFile(const char * fileName) {
	ifstream pFile(fileName, ios_base::binary | ios_base::in);
	if (!pFile.good()) {
		Log::Error(StringBuilder("Failed to load pixel shader from '") << fileName << "' !");
	}
	pFile.seekg(0, ios_base::end);
	UINT fSize = pFile.tellg();
	pFile.seekg(0, ios_base::beg);
	DWORD * binaryData = new DWORD[fSize];
	pFile.read(reinterpret_cast<char*>(binaryData), fSize);
	return binaryData;
}

void Renderer::LoadPixelShader(COMPtr<IDirect3DPixelShader9> & pixelShader, const char * fileName) {
	DWORD * binaryData = ReadEntireFile(fileName);
	if (FAILED(pD3D->CreatePixelShader(binaryData, &pixelShader))) {
		Log::Error(StringBuilder("Unable to create pixel shader from '") << fileName << "' !");
	}
	delete[] binaryData;
}

void Renderer::LoadVertexShader(COMPtr<IDirect3DVertexShader9> & vertexShader, const char * fileName) {
	DWORD * binaryData = ReadEntireFile(fileName);
	if (FAILED(pD3D->CreateVertexShader(binaryData, &vertexShader))) {
		Log::Error(StringBuilder("Unable to create vertex shader from '") << fileName << "' !");
	}
	delete[] binaryData;
}

void Renderer::RenderFullscreenQuad() {
	pD3D->SetVertexShader(mQuadVertexShader);
	++mShadersChangeCount;

	pD3D->SetVertexShaderConstantF(0, &mOrthoProjectionMatrix.m[0][0], 4);
	pD3D->SetStreamSource(0, mQuadVertexBuffer, 0, sizeof(Vertex));

	pD3D->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
	++mDIPCount;
	mRenderedTriangleCount += 2;
}

void Renderer::RenderRect(const shared_ptr<GUIRect> & r) {
	if (!r->GetTexture()) {
		return;
	}
	pD3D->SetStreamSource(0, mRectVertexBuffer, 0, sizeof(Vertex));
	void * data = nullptr;
	Vertex vertices[6];
	r->CalculateTransform();
	r->GetSixVertices(vertices);
	mRectVertexBuffer->Lock(0, 0, &data, D3DLOCK_DISCARD);
	memcpy(data, vertices, 6 * sizeof(Vertex));
	mRectVertexBuffer->Unlock();
	pD3D->SetTexture(0, std::dynamic_pointer_cast<Texture>(r->GetTexture())->GetInterface());
	pD3D->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
	mRenderedTriangleCount += 2;
	++mDIPCount;
}

float Renderer::GetGUIWidthScaleFactor() const {
	return mResWidth / ruVirtualScreenWidth;
}

void Renderer::SetParallaxEnabled(bool state) {
	mParallaxEnabled = state;
}

void Renderer::LoadColorGradingMap(const char * fileName) {
	const int dim = 16;
	mColorMap.Reset();
	COMPtr<IDirect3DTexture9> unwrapped;
	D3DCALL(D3DXCreateTextureFromFileExA(pD3D, fileName, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_FROM_FILE, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, nullptr, nullptr, &unwrapped));
	D3DCALL(pD3D->CreateVolumeTexture(dim, dim, dim, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &mColorMap, nullptr));
	D3DLOCKED_BOX locked;
	D3DCALL(mColorMap->LockBox(0, &locked, nullptr, 0));
	int rowOffset = locked.RowPitch / sizeof(A8R8G8B8Pixel);
	int sliceOffset = locked.SlicePitch / sizeof(A8R8G8B8Pixel);
	A8R8G8B8Pixel * pixels = static_cast<A8R8G8B8Pixel*>(locked.pBits);
	for (int slice = 0; slice < dim; ++slice) {
		D3DLOCKED_RECT unwrapLocked;
		D3DCALL(unwrapped->LockRect(0, &unwrapLocked, nullptr, 0));
		A8R8G8B8Pixel * unwrappedPixels = static_cast<A8R8G8B8Pixel*>(unwrapLocked.pBits);
		int unwrapRowOffset = unwrapLocked.Pitch / sizeof(A8R8G8B8Pixel);
		for (int row = 0; row < dim; ++row) {
			for (int col = 0; col < dim; ++col) {
				pixels[(slice)* sliceOffset + (row)* rowOffset + (col)] = unwrappedPixels[row * unwrapRowOffset + slice * dim + col];
			}
		}
		unwrapped->UnlockRect(0);
	}
	mColorMap->UnlockBox(0);

	D3DXSaveTextureToFileA("colormap.dds", D3DXIFF_DDS, mColorMap, nullptr);
}

void Renderer::AddMesh(const shared_ptr<Mesh> & mesh) {
	mesh->CalculateAABB();

	IDirect3DTexture9 * d3dTexture = (mesh->mDiffuseTexture != nullptr) ? mesh->mDiffuseTexture->GetInterface() : mWhiteMap;

	auto textureGroup = mDeferredMeshMap.find(d3dTexture);
	if (textureGroup == mDeferredMeshMap.end()) {
		mDeferredMeshMap[d3dTexture] = vector<weak_ptr<Mesh>>();
	}
	mDeferredMeshMap[d3dTexture].push_back(mesh);
}

bool Renderer::IsParallaxEnabled() {
	return mParallaxEnabled;
}

void Renderer::SetCursorVisible(bool state) {
	if (state) {
		if (mCursor) {
			mCursor->Show();
		} else {
			::ShowCursor(1);
			pD3D->ShowCursor(1);
		}
	} else {
		if (mCursor) {
			mCursor->Hide();
		} else {
			::ShowCursor(0);
			pD3D->ShowCursor(0);
		}
	}
}

void Renderer::SetCursor(shared_ptr<ruTexture> texture, int w, int h) {
	mCursor = shared_ptr<Cursor>(new Cursor(w, h, std::dynamic_pointer_cast<Texture>(texture)));
}
