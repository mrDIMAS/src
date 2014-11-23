#ifndef _ENGINE_
#define _ENGINE_

#include <math.h>
#include <string>

using namespace std;

#ifdef _EXPORTS
#ifndef API
#define API _declspec( dllexport )
#endif
#else
#ifndef API
#define API
#endif
#endif

class API Vector3 {
public:
	union {
		struct {
			float x;
			float y;
			float z;
		};

		float elements[ 3 ];
	};


	Vector3( );
	Vector3( float x, float y, float z );
	Vector3( const Vector3 & v );
	Vector3( float * v );
	Vector3 operator + ( const Vector3 & v ) const;
	Vector3 operator - ( const Vector3 & v ) const;
	Vector3 operator * ( const Vector3 & v ) const;
	Vector3 operator * ( const float & f ) const;
	Vector3 operator / ( const Vector3 & v ) const;
	Vector3 operator / ( const float & f ) const;
	void operator *= ( const Vector3 & v );
	void operator /= ( const Vector3 & v );
	void operator += ( const Vector3 & v );
	void operator -= ( const Vector3 & v );
	void operator = ( const Vector3 & v );
	bool operator == ( const Vector3 & v );
	float Length( ) const;
	float Length2( ) const;
	Vector3 Normalize( );
	Vector3 Normalized() const;
	Vector3 Cross( const Vector3 & v ) const;
	float Dot( const Vector3 & v ) const;
	float Angle( const Vector3 & v );
	Vector3 Rotate( const Vector3 & axis, float angle );
	Vector3 Lerp( const Vector3 & v, float t ) const;
};

static inline Vector3 operator * ( const float & f, const Vector3 & v ) {
	return Vector3( v.x * f, v.y * f, v.z * f );
}

static inline Vector3 operator - ( const Vector3 & v ) {
	return Vector3( -v.x, -v.y, -v.z );
}

static inline float Lerp( const float & value, const float & from, const float & to, const float & t ) {
	return from + ( to - from ) * t;
}

struct Vector2 {
public:
	float x;
	float y;

	Vector2( ) {
		x = 0;
		y = 0;
	};

	Vector2( float x, float y ) {
		this->x = x;
		this->y = y;
	};
};

class API Quaternion {
public:
	float x;
	float y;
	float z;
	float w;

	Quaternion( );
	Quaternion( float x, float y, float z, float w );
	Quaternion( float pitch, float yaw, float roll );
	Quaternion( const Vector3 & axis, float angle );
};

static inline Quaternion operator *  (const Quaternion& q1, const Quaternion & q2 ) {
	return Quaternion(  q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
		q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z,
		q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x,
		q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z  );
}

#define BODY_MAX_CONTACTS ( 4 )

struct Contact {
	Vector3 normal;
	Vector3 position;
	float impulse;

	Contact() {
		impulse = 0;
	}
};

class GUIState {
public:
	bool mouseInside;
	bool mouseLeftClicked;
	bool mouseRightClicked;

	GUIState() {
		mouseInside = false;
		mouseLeftClicked = false;
		mouseRightClicked = false;
	}
};

struct LinePoint {
	Vector3 position;
	int color;

	LinePoint( ) {
		position = Vector3( 0, 0, 0 );
		color = 0xFFFFFFFF;
	}

	LinePoint( Vector3 pos, int clr ) {
		position = pos;
		color = clr;
	};

	LinePoint( Vector3 pos, Vector3 clr ) {
		position = pos;

		int r = (int)clr.x;
		int g = (int)clr.y;
		int b = (int)clr.z;

		color = ((((255)&0xFF)<<24)|(((r)&0xFF)<<16)|(((g)&0xFF)<<8)|((b)&0xFF));
	};
};

enum {
	LT_POINT,
	LT_SPOT,
};

class API RutheniumHandle {
public:
	void * pointer;

	explicit RutheniumHandle();
	virtual ~RutheniumHandle();

	virtual bool IsValid();
	virtual void Invalidate();
};

class API NodeHandle : public RutheniumHandle {
public:
	bool operator == ( const NodeHandle & node );
};

class API FontHandle : public RutheniumHandle {
public:
	bool operator == ( const FontHandle & node );
};

class API TextureHandle : public RutheniumHandle {
public:
	static TextureHandle Empty();
	bool operator == ( const TextureHandle & node );
};

class API CubeTextureHandle : public RutheniumHandle {
public:
	static CubeTextureHandle Empty();
	bool operator == ( const CubeTextureHandle & node );
};

class API SoundHandle : public RutheniumHandle {
public:
	int pfHandle;
	explicit SoundHandle();
	virtual ~SoundHandle();

	bool operator == ( const SoundHandle & node );
	virtual bool IsValid();
	virtual void Invalidate();
};

////////////////////////////////////////////////////////////////////////////////////
// Renderer functions
////////////////////////////////////////////////////////////////////////////////////
API int CreateRenderer( int width, int height, int fullscreen );
API int FreeRenderer( );
API int RenderWorld( float dt );
API int GetResolutionWidth( );
API int GetResolutionHeight( );
API void HideCursor( );
API void ShowCursor( );
API void SetCursorSettings( TextureHandle texture, int w, int h );
API int DIPs( );
API int TextureUsedPerFrame( );
API void DebugDrawEnabled( int state );
API void SetAmbientColor( Vector3 color );
API int GetAvailableTextureMemory();
API void EnableShadows( bool state );

// FXAA
API void EnableFXAA( );
API void DisableFXAA( );
API bool FXAAEnabled();

// HDR
API void SetHDREnabled( bool state );
API bool IsHDREnabled( );
API void SetHDRExposure( float exposure );
API float GetHDRExposure( );

////////////////////////////////////////////////////////////////////////////////////
// Shadow functions
////////////////////////////////////////////////////////////////////////////////////
API void SetPointLightShadowMapSize( int size );
API void SetSpotLightShadowMapSize( int size );
API void EnablePointLightShadows( bool state );
API void EnableSpotLightShadows( bool state );
API bool IsPointLightShadowsEnabled();
API bool IsSpotLightShadowsEnabled();

////////////////////////////////////////////////////////////////////////////////////
// Texture functions
////////////////////////////////////////////////////////////////////////////////////
// Texture sampling
namespace TextureFilter {
	enum {
		Nearest,
		Linear,
		Anisotropic
	};
};
API void SetTextureFiltering( const int & filter, int anisotropicQuality );
API int GetMaxAnisotropy();
API TextureHandle GetTexture( const char * file );
API CubeTextureHandle GetCubeTexture( const char * file );
////////////////////////////////////////////////////////////////////////////////////
// Camera functions
////////////////////////////////////////////////////////////////////////////////////
API NodeHandle CreateCamera( float fov );
API void SetCamera( NodeHandle node );
API int SetSkybox( NodeHandle node, const char * path );
API void SetFOV( NodeHandle camera, float fov );

////////////////////////////////////////////////////////////////////////////////////
// Sounds functions
////////////////////////////////////////////////////////////////////////////////////
API SoundHandle CreateSound2D( const char * file );
API SoundHandle CreateSound3D( const char * file );
API SoundHandle CreateMusic( const char * file );
API void AttachSound( SoundHandle sound, NodeHandle node );
API void PlaySoundSource( SoundHandle sound, int oneshot = 1 );
API void PauseSoundSource( SoundHandle sound );
API void SetVolume( SoundHandle sound, float vol );
API void SetSoundPosition( SoundHandle sound, Vector3 pos );
API void SetSoundReferenceDistance( SoundHandle sound, float rd );
API void SetRolloffFactor( SoundHandle sound, float rolloffDistance );
API void SetMaxDistance( SoundHandle sound, float maxDistance );
API int SoundPlaying( SoundHandle sound );
API void FreeSoundSource( SoundHandle sound );
API void SetReverb( int reverb );
API void SetMasterVolume( float volume );
API float GetMasterVolume();
API bool IsNodeHasBody( NodeHandle node );
API void SetPitch( SoundHandle sound, float pitch );
API bool IsSoundPaused( SoundHandle sound );
////////////////////////////////////////////////////////////////////////////////////
// Light functions
////////////////////////////////////////////////////////////////////////////////////
API NodeHandle CreateLight( int type = LT_POINT );
API void SetLightRange( NodeHandle node, float rad );
API float GetLightRange( NodeHandle node );
API void SetLightColor( NodeHandle node, Vector3 clr );
API void SetConeAngles( NodeHandle node, float innerAngle, float outerAngle );
API void SetSpotTexture( NodeHandle node, TextureHandle texture );
API int GetWorldSpotLightCount();
API NodeHandle GetWorldSpotLight( int n );
API int GetWorldPointLightCount();
API NodeHandle GetWorldPointLight( int n );
API void SetLightFlare( NodeHandle node, TextureHandle flareTexture );
API void SetLightDefaultFlare( TextureHandle defaultFlareTexture );
API void SetSpotDefaultTexture( TextureHandle defaultSpotTexture );
API void SetPointTexture( NodeHandle node, CubeTextureHandle cubeTexture );
API void SetPointDefaultTexture( CubeTextureHandle defaultPointTexture );
API void SetLightFloatingLimits( NodeHandle node, Vector3 floatMin, Vector3 floatMax );
API void SetLightFloatingEnabled( NodeHandle node, bool state );
API bool IsLightFloatingEnabled( NodeHandle node );
////////////////////////////////////////////////////////////////////////////////////
// Physics functions
////////////////////////////////////////////////////////////////////////////////////
API void PausePhysics();
API void ResumePhysics();
API int GetContactCount( NodeHandle node );
API Contact GetContact( NodeHandle node, int num );
API void Freeze( NodeHandle node );
API void Unfreeze( NodeHandle node );
API void SetConvexBody( NodeHandle node );
API void SetCapsuleBody( NodeHandle node, float height, float radius );
API void SetAngularFactor( NodeHandle node, Vector3 fact );
API void SetTrimeshBody( NodeHandle node );
API void Move( NodeHandle node, Vector3 speed );
API void SetFriction( NodeHandle node, float friction );
API void SetAnisotropicFriction( NodeHandle node, Vector3 aniso );
API float GetMass( NodeHandle node );
API int IsNodeFrozen( NodeHandle node );
API void SetLinearFactor( NodeHandle node, Vector3 lin );
API void SetVelocity( NodeHandle node, Vector3 velocity );
API void SetAngularVelocity( NodeHandle node, Vector3 velocity );
API NodeHandle RayPick( int x, int y, Vector3 * outPickPoint = 0 );
API NodeHandle RayTest( Vector3 begin, Vector3 end, Vector3 * outPickPoint = 0 );
API Vector3 GetEulerAngles( NodeHandle node );
API Vector3 GetAbsoluteLookVector( NodeHandle node );

////////////////////////////////////////////////////////////////////////////////////
// Scene node functions
////////////////////////////////////////////////////////////////////////////////////
// Creation
API NodeHandle CreateSceneNode( );
API NodeHandle LoadScene( const char * file );
API NodeHandle FindByName( const char * name );
API NodeHandle FindInObjectByName( NodeHandle node, const char * name );
API int GetWorldObjectsCount();
API NodeHandle GetWorldObject( int i );
// Common
API string GetProperty( NodeHandle node, string propName );
API void HideNode( NodeHandle node );
API void ShowNode( NodeHandle node );
API bool IsNodeVisible( NodeHandle node );
API void FreeSceneNode( NodeHandle node );
API const char * GetName( NodeHandle node );
API void SetDepthHack( NodeHandle node, float order );
API void Attach( NodeHandle node1, NodeHandle node2 );
API void Detach( NodeHandle node );
API void SetDamping( NodeHandle node, float linearDamping, float angularDamping );
API void SetPosition( NodeHandle node, Vector3 position );
API void SetRotation( NodeHandle node, Quaternion rotation );
API void SetGravity( NodeHandle node, Vector3 gravity );
API Vector3 GetLookVector( NodeHandle node );
API Vector3 GetRightVector( NodeHandle node );
API Vector3 GetUpVector( NodeHandle node );
API Vector3 GetPosition( NodeHandle node );
API void SetMass( NodeHandle node, float mass );
API Quaternion GetLocalRotation( NodeHandle node );
API Vector3 GetLocalPosition( NodeHandle node );
API void SetLocalPosition( NodeHandle node, Vector3 pos );
API void SetLocalRotation( NodeHandle node, Quaternion rot );
API void SetName( NodeHandle node, const char * name );
API Vector3 GetAABBMin( NodeHandle node );
API Vector3 GetAABBMax( NodeHandle node );
API int IsNodeInside( NodeHandle node1, NodeHandle node2 );
API NodeHandle GetChild( NodeHandle node, int i );
API int GetCountChildren( NodeHandle node );
API bool IsNodeInFrustum( NodeHandle node );
API void SetAlbedo( NodeHandle node, float albedo );
API bool IsLightViewPoint( NodeHandle node, Vector3 point );
// Octree manipulation
// Note, that nodes with octree's optimization can't be moved or modified, so
// it can be used for large static geometry, like game levels
// for dynamic objects engine uses frustum culling
// Also octree can be assigned to object by ' octree="1"; ' user property in
// 3ds max or other editor.
// Octree can't be modified. So if you want set another split criteria
// you must call CreateOctree, instead using combination of DeleteOctree and
// CreateOctree
API void CreateOctree( NodeHandle node, int splitCriteria = 45 );
API void DeleteOctree( NodeHandle node );

// Animation
class API Animation {
private:
	friend class SceneNode;
	int beginFrame;
	int endFrame;
	int currentFrame;
	int nextFrame;
	float interpolator;
public:	
	float timeSeconds;
	bool looped;
	explicit Animation();
	explicit Animation( int theBeginFrame, int theEndFrame, float theTimeSeconds, bool theLooped = false );
	void SetFrameInterval( int begin, int end );
	void SetCurrentFrame( int frame );
	int GetCurrentFrame() {
		return currentFrame;
	}
	int GetEndFrame() {
		return endFrame;
	}
	int GetBeginFrame() {
		return beginFrame;
	}
	int GetNextFrame() {
		return nextFrame;
	}
	void Update( );
};

API int IsAnimationEnabled( NodeHandle node );
API void SetAnimationEnabled( NodeHandle node, bool state, bool dontAffectChilds = false );
API void SetAnimation( NodeHandle node, Animation * newAnim, bool dontAffectChilds = false );
API int GetTotalAnimationFrameCount( NodeHandle node );
API Animation * GetCurrentAnimation( NodeHandle node );

////////////////////////////////////////////////////////////////////////////////////
// Font functions
////////////////////////////////////////////////////////////////////////////////////
API FontHandle CreateGUIFont( int size, const char * name, int italic, int underlined );
API int GetTextWidth( const char * text, FontHandle font );
API int GetTextHeight( const char * text, FontHandle font, int boxWidth );
////////////////////////////////////////////////////////////////////////////////////
// GUI functions
////////////////////////////////////////////////////////////////////////////////////
API void Draw3DLine( LinePoint begin, LinePoint end );
API void DrawWireBox( LinePoint min, LinePoint max );
API void DrawGUIRect( float x, float y, float w, float h, TextureHandle texture, Vector3 color = Vector3( 255, 255, 255 ), int alpha = 255 );
API void DrawGUIText( const char * text, int x, int y, int w, int h, FontHandle font, Vector3 color, int textAlign, int alpha = 255 );
API GUIState DrawGUIButton( int x, int y, int w, int h, TextureHandle texture, const char * text, FontHandle font, Vector3 color, int textAlign, int alpha = 255 );

////////////////////////////////////////////////////////////////////////////////////
// Time functions
////////////////////////////////////////////////////////////////////////////////////
typedef int TimerHandle;

API TimerHandle CreateTimer( );
API void RestartTimer( TimerHandle timer );
API double GetElapsedTimeInSeconds( TimerHandle timer );
API double GetElapsedTimeInMilliSeconds( TimerHandle timer );
API double GetElapsedTimeInMicroSeconds( TimerHandle timer );

////////////////////////////////////////////////////////////////////////////////////
// Particle system functions
////////////////////////////////////////////////////////////////////////////////////
#define PS_BOX ( 0 )
#define PS_STREAM ( 1 )

class ParticleSystemProperties {
public:
	int type; // PS_BOX or PS_STREAM

	Vector3 colorBegin;
	Vector3 colorEnd;

	Vector3 speedDeviationMin;
	Vector3 speedDeviationMax;

	// set these values, if type == PS_BOX
	Vector3 boundingBoxMin;
	Vector3 boundingBoxMax;

	float particleThickness;
	float boundingRadius; // set this if type == PS_STREAM
	float pointSize;
	float scaleFactor;

	bool autoResurrectDeadParticles;
	bool useLighting;

	bool enabled;

	TextureHandle texture;

	explicit ParticleSystemProperties() {
		type = PS_BOX;

		colorBegin = Vector3( 0, 0, 0 );
		colorEnd = Vector3( 255, 255, 255 );

		speedDeviationMin = Vector3( -1, -1, -1 );
		speedDeviationMax = Vector3( 1, 1, 1 );

		boundingBoxMin = Vector3( 100, 100, 100 );
		boundingBoxMax = Vector3( -100, -100, -100 );

		particleThickness = 1.0f;
		boundingRadius = 1.0f;
		pointSize = 1.0f;
		scaleFactor = 0.0f;

		autoResurrectDeadParticles = true;
		useLighting = false;

		enabled = true;

		texture = TextureHandle::Empty();
	}
};

API NodeHandle CreateParticleSystem( int particleNum, ParticleSystemProperties creationProps );
API int GetParticleSystemAliveParticles( NodeHandle ps );
API void ResurrectDeadParticles( NodeHandle ps );
API ParticleSystemProperties * GetParticleSystemProperties( NodeHandle ps );
////////////////////////////////////////////////////////////////////////////////////
// Input functions
////////////////////////////////////////////////////////////////////////////////////
enum {
	KEY_Esc = 1,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_0,
	KEY_Minus,
	KEY_Equals,
	KEY_Backspace,
	KEY_Tab,
	KEY_Q,
	KEY_W,
	KEY_E,
	KEY_R,
	KEY_T,
	KEY_Y,
	KEY_U,
	KEY_I,
	KEY_O,
	KEY_P,
	KEY_LeftBracket,
	KEY_RightBracket,
	KEY_Enter,
	KEY_LeftControl,
	KEY_A,
	KEY_S,
	KEY_D,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_Semicolon,
	KEY_Apostrophe,
	KEY_Grave,
	KEY_LeftShift,
	KEY_BackSlash,
	KEY_Z,
	KEY_X,
	KEY_C,
	KEY_V,
	KEY_B,
	KEY_N,
	KEY_M,
	KEY_Comma,
	KEY_Period,
	KEY_Slash,
	KEY_RightShift,
	KEY_Multiply,
	KEY_LeftAlt,
	KEY_Space,
	KEY_Capital,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_NumLock,
	KEY_Scroll,
	KEY_NumPad7,
	KEY_NumPad8,
	KEY_NumPad9,
	KEY_Subtract,
	KEY_Numpad4,
	KEY_Numpad5,
	KEY_Numpad6,
	KEY_Add,
	KEY_Numpad1,
	KEY_Numpad2,
	KEY_Numpad3,
	KEY_Numpad0,
	KEY_Decimal,
	KEY_OEM_102,
	KEY_F11,
	KEY_F12,
	KEY_F13,
	KEY_F14,
	KEY_F15,
	KEY_Kana,
	KEY_ABNT_C1,
	KEY_Convert,
	KEY_NoConvert,
	KEY_Yen,
	KEY_ABNT_C2,
	KEY_NumpadEquals,
	KEY_PrevTrack,
	KEY_AT,
	KEY_Colon,
	KEY_Underline,
	KEY_Kanji,
	KEY_Stop,
	KEY_AX,
	KEY_Ulabeled,
	KEY_NextTrack,
	KEY_NumpadEnter,
	KEY_RControl,
	KEY_Mute,
	KEY_Calculator,
	KEY_PlayPause,
	KEY_MediaStop,
	KEY_VolumeDown,
	KEY_VolumeUp,
	KEY_WebHome,
	KEY_NumpadComma,
	KEY_Divide,
	KEY_SysRQ,
	KEY_RMenu,
	KEY_Pause,
	KEY_Home,
	KEY_Up,
	KEY_Prior,
	KEY_Left,
	KEY_Right,
	KEY_End,
	KEY_Down,
	KEY_Next,
	KEY_Insert,
	KEY_Del,
	KEY_LWin,
	KEY_RWin,
	KEY_Apps,
	KEY_Power,
	KEY_Sleep,
	KEY_Wake,
	KEY_WebSearch,
	KEY_WebFavorites,
	KEY_WebRefresh,
	KEY_WebStop,
	KEY_WebForward,
	KEY_WebBack,
	KEY_MyComputer,
	KEY_Mail,
	KEY_MediaSelect,
	KEY_Count,
};

enum {
	MB_Left,
	MB_Right,
	MB_Middle,
	MB_Wheel,
};

API void InputInit( void * window );
API void InputDestroy( );
API int	IsMouseDown( int button );
API int	IsMouseHit( int button );
API int	GetMouseX( );
API int	GetMouseY( );
API int	GetMouseWheel( );
API int	GetMouseXSpeed( );
API int	GetMouseYSpeed( );
API int	GetMouseWheelSpeed	( );
API int	IsKeyDown( int key );
API int	IsKeyHit( int key );
API int	IsKeyUp( int key );
API void InputUpdate( );


#endif