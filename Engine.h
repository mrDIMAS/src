#ifndef _ENGINE_
#define _ENGINE_

#include <math.h>
#include <string>
#include <windows.h>

using namespace std;

#ifdef _EXPORTS
#ifndef API
#define RUAPI _declspec( dllexport )
#endif
#else
#ifndef API
#define RUAPI
#endif
#endif

class RUAPI ruVector3
{
public:
    union
    {
        struct
        {
            float x;
            float y;
            float z;
        };

        float elements[ 3 ];
    };


    ruVector3( );
    ruVector3( float x, float y, float z );
    ruVector3( const ruVector3 & v );
    ruVector3( float * v );
    ruVector3 operator + ( const ruVector3 & v ) const;
    ruVector3 operator - ( const ruVector3 & v ) const;
    ruVector3 operator * ( const ruVector3 & v ) const;
    ruVector3 operator * ( const float & f ) const;
    ruVector3 operator / ( const ruVector3 & v ) const;
    ruVector3 operator / ( const float & f ) const;
    void operator *= ( const ruVector3 & v );
    void operator /= ( const ruVector3 & v );
    void operator += ( const ruVector3 & v );
    void operator -= ( const ruVector3 & v );
    void operator = ( const ruVector3 & v );
    bool operator == ( const ruVector3 & v );
    float Length( ) const;
    float Length2( ) const;
    ruVector3 Normalize( );
    ruVector3 Normalized() const;
    ruVector3 Cross( const ruVector3 & v ) const;
    float Dot( const ruVector3 & v ) const;
    float Angle( const ruVector3 & v );
    ruVector3 Rotate( const ruVector3 & axis, float angle );
    ruVector3 Lerp( const ruVector3 & v, float t ) const;
};

static inline ruVector3 operator * ( const float & f, const ruVector3 & v )
{
    return ruVector3( v.x * f, v.y * f, v.z * f );
}

static inline ruVector3 operator - ( const ruVector3 & v )
{
    return ruVector3( -v.x, -v.y, -v.z );
}

static inline float Lerp( const float & value, const float & from, const float & to, const float & t )
{
    return from + ( to - from ) * t;
}

struct ruVector2
{
public:
    float x;
    float y;

    ruVector2( )
    {
        x = 0;
        y = 0;
    };

    ruVector2( float x, float y )
    {
        this->x = x;
        this->y = y;
    };
};

class RUAPI ruQuaternion
{
public:
    float x;
    float y;
    float z;
    float w;

    ruQuaternion( );
    ruQuaternion( float x, float y, float z, float w );
    ruQuaternion( float pitch, float yaw, float roll );
    ruQuaternion( const ruVector3 & axis, float angle );
};

static inline ruQuaternion operator *  (const ruQuaternion& q1, const ruQuaternion & q2 )
{
    return ruQuaternion(  q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
                          q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z,
                          q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x,
                          q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z  );
}

#define BODY_MAX_CONTACTS ( 4 )

struct ruContact
{
    ruVector3 normal;
    ruVector3 position;
    float impulse;

    ruContact()
    {
        impulse = 0;
    }
};

class ruGUIState
{
public:
    bool mouseInside;
    bool mouseLeftClicked;
    bool mouseRightClicked;

    ruGUIState()
    {
        mouseInside = false;
        mouseLeftClicked = false;
        mouseRightClicked = false;
    }
};

struct ruLinePoint
{
    ruVector3 position;
    int color;

    ruLinePoint( )
    {
        position = ruVector3( 0, 0, 0 );
        color = 0xFFFFFFFF;
    }

    ruLinePoint( ruVector3 pos, int clr )
    {
        position = pos;
        color = clr;
    };

    ruLinePoint( ruVector3 pos, ruVector3 clr )
    {
        position = pos;

        int r = (int)clr.x;
        int g = (int)clr.y;
        int b = (int)clr.z;

        color = ((((255)&0xFF)<<24)|(((r)&0xFF)<<16)|(((g)&0xFF)<<8)|((b)&0xFF));
    };
};





enum
{
    LT_POINT,
    LT_SPOT,
};

class RUAPI ruRutheniumHandle
{
public:
    void * pointer;

    explicit ruRutheniumHandle();
    virtual ~ruRutheniumHandle();

    virtual bool IsValid();
    virtual void Invalidate();
};

class RUAPI ruNodeHandle : public ruRutheniumHandle
{
public:
    bool operator == ( const ruNodeHandle & node );
};

class RUAPI ruFontHandle : public ruRutheniumHandle
{
public:
    bool operator == ( const ruFontHandle & node );
};

class RUAPI ruTextureHandle : public ruRutheniumHandle
{
public:
    static ruTextureHandle Empty();
    bool operator == ( const ruTextureHandle & node );
};

class RUAPI ruCubeTextureHandle : public ruRutheniumHandle
{
public:
    static ruCubeTextureHandle Empty();
    bool operator == ( const ruCubeTextureHandle & node );
};

class RUAPI ruSoundHandle : public ruRutheniumHandle
{
public:
    int pfHandle;
    explicit ruSoundHandle();
    virtual ~ruSoundHandle();

    bool operator == ( const ruSoundHandle & node );
    virtual bool IsValid();
    virtual void Invalidate();
};

class RUAPI ruGUINodeHandle : public ruRutheniumHandle
{
public:
	bool operator == ( const ruGUINodeHandle & node );
};

class RUAPI ruButtonHandle : public ruRutheniumHandle
{
public:
	bool operator == ( const ruButtonHandle & node );
};

class RUAPI ruRectHandle : public ruGUINodeHandle
{
public:
	bool operator == ( const ruRectHandle & node );
};

class RUAPI ruTextHandle : public ruGUINodeHandle
{
public:
	bool operator == ( const ruTextHandle & node );
};

class RUAPI ruLineHandle : public ruGUINodeHandle
{
public:
	bool operator == ( const ruLineHandle & node );
};


////////////////////////////////////////////////////////////////////////////////////
// Renderer functions
////////////////////////////////////////////////////////////////////////////////////
RUAPI int ruCreateRenderer( int width, int height, int fullscreen, char vSync );
RUAPI int ruFreeRenderer( );
RUAPI int ruRenderWorld( float dt );
RUAPI int ruGetResolutionWidth( );
RUAPI int ruGetResolutionHeight( );
RUAPI void ruHideCursor( );
RUAPI void ruShowCursor( );
RUAPI void ruSetCursorSettings( ruTextureHandle texture, int w, int h );
RUAPI int ruDIPs( );
RUAPI int ruTextureUsedPerFrame( );
RUAPI void ruDebugDrawEnabled( int state );
RUAPI void ruSetAmbientColor( ruVector3 color );
RUAPI int ruGetAvailableTextureMemory();
RUAPI void ruEnableShadows( bool state );
RUAPI void ruSetRenderQuality( char renderQuality );

// FXAA
RUAPI void ruEnableFXAA( );
RUAPI void ruDisableFXAA( );
RUAPI bool ruFXAAEnabled();

// HDR
RUAPI void ruSetHDREnabled( bool state );
RUAPI bool ruIsHDREnabled( );
RUAPI void ruSetHDRExposure( float exposure );
RUAPI float ruGetHDRExposure( );

////////////////////////////////////////////////////////////////////////////////////
// Shadow functions
////////////////////////////////////////////////////////////////////////////////////
RUAPI void ruSetPointLightShadowMapSize( int size );
RUAPI void ruSetSpotLightShadowMapSize( int size );
RUAPI void ruEnablePointLightShadows( bool state );
RUAPI void ruEnableSpotLightShadows( bool state );
RUAPI bool ruIsPointLightShadowsEnabled();
RUAPI bool ruIsSpotLightShadowsEnabled();

////////////////////////////////////////////////////////////////////////////////////
// Texture functions
////////////////////////////////////////////////////////////////////////////////////
// Texture sampling
namespace ruTextureFilter
{
enum
{
    Nearest,
    Linear,
    Anisotropic
};
};
RUAPI void ruSetRendererTextureFiltering( const int & filter, int anisotropicQuality );
RUAPI int ruGetRendererMaxAnisotropy();
RUAPI ruTextureHandle ruGetTexture( const char * file );
RUAPI ruCubeTextureHandle ruGetCubeTexture( const char * file );
////////////////////////////////////////////////////////////////////////////////////
// Camera functions
////////////////////////////////////////////////////////////////////////////////////
RUAPI ruNodeHandle ruCreateCamera( float fov );
RUAPI void ruSetActiveCamera( ruNodeHandle node );
RUAPI int ruSetCameraSkybox( ruNodeHandle node, const char * path );
RUAPI void ruSetCameraFOV( ruNodeHandle camera, float fov );

////////////////////////////////////////////////////////////////////////////////////
// Sounds functions
////////////////////////////////////////////////////////////////////////////////////
RUAPI ruSoundHandle ruLoadSound2D( const char * file );
RUAPI ruSoundHandle ruLoadSound3D( const char * file );
RUAPI ruSoundHandle ruLoadMusic( const char * file );
RUAPI void ruAttachSound( ruSoundHandle sound, ruNodeHandle node );
RUAPI void ruPlaySound( ruSoundHandle sound, int oneshot = 1 );
RUAPI void ruPauseSound( ruSoundHandle sound );
RUAPI void ruSetSoundVolume( ruSoundHandle sound, float vol );
RUAPI void ruSetSoundPosition( ruSoundHandle sound, ruVector3 pos );
RUAPI void ruSetSoundReferenceDistance( ruSoundHandle sound, float rd );
RUAPI void ruSetRolloffFactor( ruSoundHandle sound, float rolloffDistance );
RUAPI void ruSetSoundMaxDistance( ruSoundHandle sound, float maxDistance );
RUAPI int ruIsSoundPlaying( ruSoundHandle sound );
RUAPI void ruFreeSound( ruSoundHandle sound );
RUAPI void ruSetAudioReverb( int reverb );
RUAPI void ruSetMasterVolume( float volume );
RUAPI float ruGetMasterVolume();
RUAPI bool ruIsNodeHasBody( ruNodeHandle node );
RUAPI void ruSetSoundsPitch( ruSoundHandle sound, float pitch );
RUAPI bool ruIsSoundPaused( ruSoundHandle sound );
////////////////////////////////////////////////////////////////////////////////////
// Light functions
////////////////////////////////////////////////////////////////////////////////////
RUAPI ruNodeHandle ruCreateLight( int type = LT_POINT );
RUAPI void ruSetLightRange( ruNodeHandle node, float rad );
RUAPI float ruGetLightRange( ruNodeHandle node );
RUAPI void ruSetLightColor( ruNodeHandle node, ruVector3 clr );
RUAPI void ruSetConeAngles( ruNodeHandle node, float innerAngle, float outerAngle );
RUAPI void ruSetLightSpotTexture( ruNodeHandle node, ruTextureHandle texture );
RUAPI int ruGetWorldSpotLightCount();
RUAPI ruNodeHandle ruGetWorldSpotLight( int n );
RUAPI int ruGetWorldPointLightCount();
RUAPI ruNodeHandle ruGetWorldPointLight( int n );
RUAPI void ruSetLightFlare( ruNodeHandle node, ruTextureHandle flareTexture );
RUAPI void ruSetLightDefaultFlare( ruTextureHandle defaultFlareTexture );
RUAPI void ruSetLightSpotDefaultTexture( ruTextureHandle defaultSpotTexture );
RUAPI void ruSetLightPointTexture( ruNodeHandle node, ruCubeTextureHandle cubeTexture );
RUAPI void ruSetLightPointDefaultTexture( ruCubeTextureHandle defaultPointTexture );
RUAPI void ruSetLightFloatingLimits( ruNodeHandle node, ruVector3 floatMin, ruVector3 floatMax );
RUAPI void ruSetLightFloatingEnabled( ruNodeHandle node, bool state );
RUAPI bool ruIsLightFloatingEnabled( ruNodeHandle node );
RUAPI bool ruIsLight( ruNodeHandle node );
////////////////////////////////////////////////////////////////////////////////////
// Physics functions
////////////////////////////////////////////////////////////////////////////////////
RUAPI void ruUpdatePhysics( float timeStep, int subSteps, float fixedTimeStep );
RUAPI int ruGetContactCount( ruNodeHandle node );
RUAPI ruContact ruGetContact( ruNodeHandle node, int num );
RUAPI void ruFreeze( ruNodeHandle node );
RUAPI void ruUnfreeze( ruNodeHandle node );
RUAPI void ruSetConvexBody( ruNodeHandle node );
RUAPI void ruSetCapsuleBody( ruNodeHandle node, float height, float radius );
RUAPI void ruSetAngularFactor( ruNodeHandle node, ruVector3 fact );
RUAPI void ruSetTrimeshBody( ruNodeHandle node );
RUAPI void ruMoveNode( ruNodeHandle node, ruVector3 speed );
RUAPI void ruSetNodeFriction( ruNodeHandle node, float friction );
RUAPI void ruSetNodeAnisotropicFriction( ruNodeHandle node, ruVector3 aniso );
RUAPI float ruGetNodeMass( ruNodeHandle node );
RUAPI int ruIsNodeFrozen( ruNodeHandle node );
RUAPI void ruSetNodeLinearFactor( ruNodeHandle node, ruVector3 lin );
RUAPI void ruSetNodeVelocity( ruNodeHandle node, ruVector3 velocity );
RUAPI void ruSetNodeAngularVelocity( ruNodeHandle node, ruVector3 velocity );
RUAPI ruNodeHandle ruRayPick( int x, int y, ruVector3 * outPickPoint = 0 );
RUAPI ruNodeHandle ruCastRay( ruVector3 begin, ruVector3 end, ruVector3 * outPickPoint = 0 );
RUAPI ruVector3 ruGetNodeEulerAngles( ruNodeHandle node );
RUAPI ruVector3 ruGetNodeAbsoluteLookVector( ruNodeHandle node );

////////////////////////////////////////////////////////////////////////////////////
// Scene node functions
////////////////////////////////////////////////////////////////////////////////////
// Creation
RUAPI ruNodeHandle ruCreateSceneNode( );
RUAPI ruNodeHandle ruLoadScene( const char * file );
RUAPI ruNodeHandle ruFindByName( const char * name );
RUAPI ruNodeHandle ruFindInObjectByName( ruNodeHandle node, const char * name );
RUAPI int ruGetWorldObjectsCount();
RUAPI ruNodeHandle ruGetWorldObject( int i );
// Common
RUAPI string ruGetProperty( ruNodeHandle node, string propName );
RUAPI void ruHideNode( ruNodeHandle node );
RUAPI void ruShowNode( ruNodeHandle node );
RUAPI bool ruIsNodeVisible( ruNodeHandle node );
RUAPI void ruFreeSceneNode( ruNodeHandle node );
RUAPI const char * ruGetNodeName( ruNodeHandle node );
RUAPI void ruSetNodeDepthHack( ruNodeHandle node, float order );
RUAPI void ruAttachNode( ruNodeHandle node1, ruNodeHandle node2 );
RUAPI void ruDetachNode( ruNodeHandle node );
RUAPI void ruSetNodeDamping( ruNodeHandle node, float linearDamping, float angularDamping );
RUAPI void ruSetNodePosition( ruNodeHandle node, ruVector3 position );
RUAPI void ruSetNodeRotation( ruNodeHandle node, ruQuaternion rotation );
RUAPI void ruSetNodeGravity( ruNodeHandle node, ruVector3 gravity );
RUAPI ruVector3 ruGetNodeLookVector( ruNodeHandle node );
RUAPI ruVector3 ruGetNodeRightVector( ruNodeHandle node );
RUAPI ruVector3 ruGetNodeUpVector( ruNodeHandle node );
RUAPI ruVector3 ruGetNodePosition( ruNodeHandle node );
RUAPI void ruSetNodeMass( ruNodeHandle node, float mass );
RUAPI ruQuaternion ruGetNodeLocalRotation( ruNodeHandle node );
RUAPI ruVector3 ruGetNodeLocalPosition( ruNodeHandle node );
RUAPI void ruSetNodeLocalPosition( ruNodeHandle node, ruVector3 pos );
RUAPI void ruSetNodeLocalRotation( ruNodeHandle node, ruQuaternion rot );
RUAPI void ruSetNodeName( ruNodeHandle node, const char * name );
RUAPI ruVector3 ruGetNodeAABBMin( ruNodeHandle node );
RUAPI ruVector3 ruGetNodeAABBMax( ruNodeHandle node );
RUAPI int ruIsNodeInsideNode( ruNodeHandle node1, ruNodeHandle node2 );
RUAPI ruNodeHandle ruGetNodeChild( ruNodeHandle node, int i );
RUAPI int ruGetNodeCountChildren( ruNodeHandle node );
RUAPI bool ruIsNodeInFrustum( ruNodeHandle node );
RUAPI void ruSetNodeAlbedo( ruNodeHandle node, float albedo );
RUAPI bool ruIsLightSeePoint( ruNodeHandle node, ruVector3 point );
// Octree manipulation
// Note, that nodes with octree's optimization can't be moved or modified, so
// it can be used for large static geometry, like game levels
// for dynamic objects engine uses frustum culling
// Also octree can be assigned to object by ' octree="1"; ' user property in
// 3ds max or other editor.
// Octree can't be modified. So if you want set another split criteria
// you must call CreateOctree, instead using combination of DeleteOctree and
// CreateOctree
RUAPI void ruCreateOctree( ruNodeHandle node, int splitCriteria = 45 );
RUAPI void ruDeleteOctree( ruNodeHandle node );

// Animation
class RUAPI ruAnimation
{
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
    explicit ruAnimation();
    explicit ruAnimation( int theBeginFrame, int theEndFrame, float theTimeSeconds, bool theLooped = false );
    void SetFrameInterval( int begin, int end );
    void SetCurrentFrame( int frame );
    int GetCurrentFrame()
    {
        return currentFrame;
    }
    int GetEndFrame()
    {
        return endFrame;
    }
    int GetBeginFrame()
    {
        return beginFrame;
    }
    int GetNextFrame()
    {
        return nextFrame;
    }
    void Update( );
};

RUAPI int ruIsAnimationEnabled( ruNodeHandle node );
RUAPI void ruSetAnimationEnabled( ruNodeHandle node, bool state, bool dontAffectChilds = false );
RUAPI void ruSetAnimation( ruNodeHandle node, ruAnimation * newAnim, bool dontAffectChilds = false );
RUAPI int ruGetTotalAnimationFrameCount( ruNodeHandle node );
RUAPI ruAnimation * ruGetCurrentAnimation( ruNodeHandle node );

////////////////////////////////////////////////////////////////////////////////////
// Font functions
////////////////////////////////////////////////////////////////////////////////////
RUAPI ruFontHandle ruCreateGUIFont( int size, const char * name, int italic, int underlined );
RUAPI int ruGetTextWidth( const char * text, ruFontHandle font );
RUAPI int ruGetTextHeight( const char * text, ruFontHandle font, int boxWidth );
////////////////////////////////////////////////////////////////////////////////////
// GUI functions
////////////////////////////////////////////////////////////////////////////////////
/*
RUAPI void ruDraw3DLine( ruLinePoint begin, ruLinePoint end );
RUAPI void ruDrawWireBox( ruLinePoint min, ruLinePoint max );
RUAPI void ruDrawGUIRect( float x, float y, float w, float h, ruTextureHandle texture, ruVector3 color = ruVector3( 255, 255, 255 ), int alpha = 255 );
RUAPI void ruDrawGUIText( const char * text, int x, int y, int w, int h, ruFontHandle font, ruVector3 color, int textAlign, int alpha = 255 );
RUAPI ruGUIState ruDrawGUIButton( int x, int y, int w, int h, ruTextureHandle texture, const char * text, ruFontHandle font, ruVector3 color, int textAlign, int alpha = 255 );
*/
RUAPI ruRectHandle ruCreateGUIRect( float x, float y, float w, float h, ruTextureHandle texture, ruVector3 color = ruVector3( 255, 255, 255 ), int alpha = 255 );
RUAPI ruTextHandle ruCreateGUIText( const char * text, int x, int y, int w, int h, ruFontHandle font, ruVector3 color, int textAlign, int alpha = 255 );
RUAPI ruButtonHandle ruCreateGUIButton( int x, int y, int w, int h, ruTextureHandle texture, const char * text, ruFontHandle font, ruVector3 color, int textAlign, int alpha = 255 );
RUAPI ruLineHandle ruCreateGUILine( ruLinePoint begin, ruLinePoint end );
RUAPI void ruSetGUINodeText( ruTextHandle node, const char * text );
RUAPI void ruSetGUINodePosition( ruGUINodeHandle node, float x, float y );
RUAPI void ruSetGUINodeSize( ruGUINodeHandle node, float w, float h );
RUAPI void ruSetGUINodeColor( ruGUINodeHandle node, ruVector3 color );
RUAPI void ruSetGUINodeAlpha( ruGUINodeHandle node, int alpha );
RUAPI void ruSetGUINodeVisible( ruGUINodeHandle node, bool visible );
RUAPI bool ruIsGUINodeVisible( ruGUINodeHandle node );
RUAPI ruVector2 ruGetGUINodePosition( ruGUINodeHandle node );
RUAPI ruVector2 ruGetGUINodeSize( ruGUINodeHandle node );
RUAPI ruVector3 ruGetGUINodeColor( ruGUINodeHandle node );
RUAPI int ruGetGUINodeAlpha( ruGUINodeHandle node );
RUAPI bool ruIsButtonPressed( ruButtonHandle node );
RUAPI bool ruIsButtonPicked( ruButtonHandle node );

////////////////////////////////////////////////////////////////////////////////////
// Time functions
////////////////////////////////////////////////////////////////////////////////////
typedef int ruTimerHandle;

RUAPI ruTimerHandle ruCreateTimer( );
RUAPI void ruRestartTimer( ruTimerHandle timer );
RUAPI double ruGetElapsedTimeInSeconds( ruTimerHandle timer );
RUAPI double ruGetElapsedTimeInMilliSeconds( ruTimerHandle timer );
RUAPI double ruGetElapsedTimeInMicroSeconds( ruTimerHandle timer );
RUAPI double ruGetTimeInSeconds( ruTimerHandle timer );
RUAPI double ruGetTimeInMilliSeconds( ruTimerHandle timer );
RUAPI double ruGetTimeInMicroSeconds( ruTimerHandle timer );
////////////////////////////////////////////////////////////////////////////////////
// Particle system functions
////////////////////////////////////////////////////////////////////////////////////
#define PS_BOX ( 0 )
#define PS_STREAM ( 1 )

class ruParticleSystemProperties
{
public:
    int type; // PS_BOX or PS_STREAM

    ruVector3 colorBegin;
    ruVector3 colorEnd;

    ruVector3 speedDeviationMin;
    ruVector3 speedDeviationMax;

    // set these values, if type == PS_BOX
    ruVector3 boundingBoxMin;
    ruVector3 boundingBoxMax;

    float particleThickness;
    float boundingRadius; // set this if type == PS_STREAM
    float pointSize;
    float scaleFactor;

    bool autoResurrectDeadParticles;
    bool useLighting;

    bool enabled;

    ruTextureHandle texture;

    explicit ruParticleSystemProperties()
    {
        type = PS_BOX;

        colorBegin = ruVector3( 0, 0, 0 );
        colorEnd = ruVector3( 255, 255, 255 );

        speedDeviationMin = ruVector3( -1, -1, -1 );
        speedDeviationMax = ruVector3( 1, 1, 1 );

        boundingBoxMin = ruVector3( 100, 100, 100 );
        boundingBoxMax = ruVector3( -100, -100, -100 );

        particleThickness = 1.0f;
        boundingRadius = 1.0f;
        pointSize = 1.0f;
        scaleFactor = 0.0f;

        autoResurrectDeadParticles = true;
        useLighting = false;

        enabled = true;

        texture = ruTextureHandle::Empty();
    }
};

RUAPI ruNodeHandle ruCreateParticleSystem( int particleNum, ruParticleSystemProperties creationProps );
RUAPI int ruGetParticleSystemAliveParticles( ruNodeHandle ps );
RUAPI void ruResurrectDeadParticles( ruNodeHandle ps );
RUAPI ruParticleSystemProperties * ruGetParticleSystemProperties( ruNodeHandle ps );
////////////////////////////////////////////////////////////////////////////////////
// Input functions
////////////////////////////////////////////////////////////////////////////////////
enum
{
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

enum
{
    MB_Left,
    MB_Right,
    MB_Middle,
    MB_Wheel,
};

RUAPI void ruInputInit( void * window );
RUAPI void ruInputDestroy( );
RUAPI int	ruIsMouseDown( int button );
RUAPI int	ruIsMouseHit( int button );
RUAPI int	ruGetMouseX( );
RUAPI int	ruGetMouseY( );
RUAPI int	ruGetMouseWheel( );
RUAPI int	ruGetMouseXSpeed( );
RUAPI int	ruGetMouseYSpeed( );
RUAPI int	ruGetMouseWheelSpeed	( );
RUAPI int	ruIsKeyDown( int key );
RUAPI int	ruIsKeyHit( int key );
RUAPI int	ruIsKeyUp( int key );
RUAPI void ruInputUpdate( );


#endif