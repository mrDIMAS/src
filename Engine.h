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
API int RenderWorld( float dt = 1.0f );
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

// FXAA
API void EnableFXAA( );
API void DisableFXAA( );
API bool FXAAEnabled();

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

////////////////////////////////////////////////////////////////////////////////////
// Resource functions
////////////////////////////////////////////////////////////////////////////////////
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
API Vector3 GetEulerAngles( NodeHandle node );

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
API void SetOrder( NodeHandle node, int order );
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
API int Animating( NodeHandle node );
API void Animate( NodeHandle node, float speed, int mode );
API void SetAnimationSequence( NodeHandle node, int begin, int end );

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

API NodeHandle CreateParticleSystem( int particleNum, TextureHandle texture, int type = PS_BOX );
API void SetParticleSystemSpeedDeviation( NodeHandle ps, Vector3 min, Vector3 max );
API void SetParticleSystemColors( NodeHandle ps, Vector3 begin, Vector3 end );
API void SetParticleSystemPointSize( NodeHandle ps, float pointSize );
API void SetParticleSystemBox( NodeHandle ps, Vector3 min, Vector3 max );
API void SetParticleSystemAutoResurrect( NodeHandle ps, int ar );
API int GetParticleSystemAliveParticles( NodeHandle ps );
API void RestartParticleSystem( NodeHandle ps );
API void EnableParticleSystem( NodeHandle ps );
API void DisableParticleSystem( NodeHandle ps );
API void EnableParticleSystemLighting( NodeHandle ps );
API void DisableParticleSystemLighting( NodeHandle ps );
API void SetParticleSystemThickness( NodeHandle ps, float thickness );
API void SetParticleSystemRadius( NodeHandle ps, float radius );
API void SetParticleSystemParticleScaling( NodeHandle ps, float scl );

////////////////////////////////////////////////////////////////////////////////////
// Input functions
////////////////////////////////////////////////////////////////////////////////////
namespace mi {

typedef enum {
    Esc = 1,
    _1,
    _2,
    _3,
    _4,
    _5,
    _6,
    _7,
    _8,
    _9,
    _0,
    Minus,
    Equals,
    Backspace,
    Tab,
    Q,
    W,
    E,
    R,
    T,
    Y,
    U,
    I,
    O,
    P,
    LeftBracket,
    RightBracket,
    Enter,
    LeftControl,
    A,
    S,
    D,
    F,
    G,
    H,
    J,
    K,
    L,
    Semicolon,
    Apostrophe,
    Grave,
    LeftShift,
    BackSlash,
    Z,
    X,
    C,
    V,
    B,
    N,
    M,
    Comma,
    Period,
    Slash,
    RightShift,
    Multiply,
    LeftAlt,
    Space,
    Capital,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    NumLock,
    Scroll,
    NumPad7,
    NumPad8,
    NumPad9,
    SUBTRACT,
    NUMPAD4,
    NUMPAD5,
    NUMPAD6,
    ADD,
    NUMPAD1,
    NUMPAD2,
    NUMPAD3,
    NUMPAD0,
    Decimal,
    OEM_102,
    F11,
    F12,
    F13,
    F14,
    F15,
    KANA,
    ABNT_C1,
    CONVERT,
    NOCONVERT,
    YEN,
    ABNT_C2,
    NUMPADEQUALS,
    PREVTRACK,
    AT,
    COLON,
    UNDERLINE,
    KANJI,
    STOP,
    AX,
    UNLABELED,
    NEXTTRACK,
    NUMPADENTER,
    RCONTROL,
    MUTE,
    CALCULATOR,
    PLAYPAUSE,
    MEDIASTOP,
    VOLUMEDOWN,
    VOLUMEUP,
    WEBHOME,
    NUMPADCOMMA,
    DIVIDE,
    SYSRQ,
    RMENU,
    PAUSE,
    HOME,
    UP,
    PRIOR,
    LEFT,
    RIGHT,
    END,
    DOWN,
    NEXT,
    INSERT,
    DEL,
    LWIN,
    RWIN,
    APPS,
    POWER,
    SLEEP,
    WAKE,
    WEBSEARCH,
    WEBFAVORITES,
    WEBREFRESH,
    WEBSTOP,
    WEBFORWARD,
    WEBBACK,
    MYCOMPUTER,
    MAIL,
    MEDIASELECT,
} Key;




typedef enum {
    Left		= 0,
    Right		= 1,
    Middle		= 2,
    Wheel		= 3,
} MouseButton;

typedef enum {
    UnableToInitializeDirectInput = -1000,
    UnableToCreateKeyboardDevice,
    UnableToCreateMouseDevice,
    DeviceInitializedSuccessfully,
    DeviceDestroyedSuccessfully,
} Message;

API Message	Init			( void * window );
API Message	Destroy			( );
API int		MouseDown		( MouseButton button );
API int		MouseHit		( MouseButton button );
API int		MouseX			( );
API int		MouseY			( );
API int		MouseWheel		( );
API int		MouseXSpeed		( );
API int		MouseYSpeed		( );
API int		MouseWheelSpeed	( );
API int		KeyDown			( Key key );
API int		KeyHit			( Key key );
API int		KeyUp			( Key key );
API void		Update			( );

};

#endif