#ifndef _ENGINE_
#define _ENGINE_

#include <math.h>
#include <string>
#include <windows.h>
#include <sstream>
#include <iomanip>

using namespace std;

class StringBuilder {
protected:
	std::stringstream mStream;
public:
	explicit StringBuilder( const char * str ) {
		stringstream::sync_with_stdio(false);
		mStream << str;
	}
	explicit StringBuilder(  ) {
		stringstream::sync_with_stdio(false);
	}
	template<class T>
	StringBuilder & operator<< (const T& arg) {
		mStream << arg;
		return *this;
	}
	operator string() const {
		return mStream.str();
	}
	const char * ToCStr() {
		return mStream.str().c_str();
	}
};

float frandom( float low, float high );

class ruVector3 {
public:
    union {
        struct {
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

static inline ruVector3 operator * ( const float & f, const ruVector3 & v ) {
    return ruVector3( v.x * f, v.y * f, v.z * f );
}

static inline ruVector3 operator - ( const ruVector3 & v ) {
    return ruVector3( -v.x, -v.y, -v.z );
}

static inline float Lerp( const float & from, const float & to, const float & t ) {
    return from + ( to - from ) * t;
}

struct ruVector2 {
public:
    float x;
    float y;

    ruVector2( ) {
        x = 0;
        y = 0;
    };

    ruVector2( float x, float y ) {
        this->x = x;
        this->y = y;
    };
};

class ruQuaternion {
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

static inline ruQuaternion operator *  (const ruQuaternion& q1, const ruQuaternion & q2 ) {
    return ruQuaternion(  q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
                          q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z,
                          q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x,
                          q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z  );
}

#define BODY_MAX_CONTACTS ( 4 )



class ruGUIState {
public:
    bool mouseInside;
    bool mouseLeftClicked;
    bool mouseRightClicked;

    ruGUIState() {
        mouseInside = false;
        mouseLeftClicked = false;
        mouseRightClicked = false;
    }
};

struct ruLinePoint {
    ruVector3 position;
    int color;

    ruLinePoint( ) {
        position = ruVector3( 0, 0, 0 );
        color = 0xFFFFFFFF;
    }

    ruLinePoint( ruVector3 pos, int clr ) {
        position = pos;
        color = clr;
    };

    ruLinePoint( ruVector3 pos, ruVector3 clr ) {
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

class ruRutheniumHandle {
public:
    void * pointer;

    explicit ruRutheniumHandle();
    virtual ~ruRutheniumHandle();

    virtual bool IsValid();
    virtual void Invalidate();
};

class ruNodeHandle : public ruRutheniumHandle {
public:
    bool operator == ( const ruNodeHandle & node );

	virtual bool IsValid();
};

struct ruContact {
	ruVector3 normal;
	ruVector3 position;
	float impulse;
	ruNodeHandle body;

	ruContact() {
		impulse = 0;
	}
};

class ruFontHandle : public ruRutheniumHandle {
public:
    bool operator == ( const ruFontHandle & node );
};

class ruTextureHandle : public ruRutheniumHandle {
public:
    static ruTextureHandle Empty();
    bool operator == ( const ruTextureHandle & node );
};

class ruCubeTextureHandle : public ruRutheniumHandle {
public:
    static ruCubeTextureHandle Empty();
    bool operator == ( const ruCubeTextureHandle & node );
};

class ruSoundHandle : public ruRutheniumHandle {
public:
    int pfHandle;
    explicit ruSoundHandle();
    virtual ~ruSoundHandle();

    bool operator == ( const ruSoundHandle & node );
    virtual bool IsValid();
    virtual void Invalidate();
};

class ruGUINodeHandle : public ruRutheniumHandle {
public:
    bool operator == ( const ruGUINodeHandle & node );
};

class ruButtonHandle : public ruGUINodeHandle {
public:
    bool operator == ( const ruButtonHandle & node );
};

class ruRectHandle : public ruGUINodeHandle {
public:
    bool operator == ( const ruRectHandle & node );
};

class ruTextHandle : public ruGUINodeHandle {
public:
    bool operator == ( const ruTextHandle & node );
};

class ruLineHandle : public ruGUINodeHandle {
public:
    bool operator == ( const ruLineHandle & node );
};


////////////////////////////////////////////////////////////////////////////////////
// Renderer functions
////////////////////////////////////////////////////////////////////////////////////
int ruCreateRenderer( int width, int height, int fullscreen, char vSync );
int ruFreeRenderer( );
int ruRenderWorld( );
int ruGetResolutionWidth( );
int ruGetResolutionHeight( );
void ruHideCursor( );
void ruShowCursor( );
void ruSetCursorSettings( ruTextureHandle texture, int w, int h );
int ruDIPs( );
int ruTextureUsedPerFrame( );
void ruSetAmbientColor( ruVector3 color );
int ruGetAvailableTextureMemory();
void ruEnableShadows( bool state );
void ruUpdateWorld();
// FXAA
void ruEnableFXAA( );
void ruDisableFXAA( );
bool ruFXAAEnabled();

// HDR
void ruSetHDREnabled( bool state );
bool ruIsHDREnabled( );

////////////////////////////////////////////////////////////////////////////////////
// Shadow functions
////////////////////////////////////////////////////////////////////////////////////
void ruSetSpotLightShadowMapSize( int size );
void ruEnableSpotLightShadows( bool state );
bool ruIsSpotLightShadowsEnabled();

////////////////////////////////////////////////////////////////////////////////////
// Texture functions
////////////////////////////////////////////////////////////////////////////////////
// Texture sampling
namespace ruTextureFilter {
enum {
    Nearest,
    Linear,
    Anisotropic
};
};
void ruSetRendererTextureFiltering( const int & filter, int anisotropicQuality );
int ruGetRendererMaxAnisotropy();
ruTextureHandle ruGetTexture( const string & file );
ruCubeTextureHandle ruGetCubeTexture( const string & file );
////////////////////////////////////////////////////////////////////////////////////
// Camera functions
////////////////////////////////////////////////////////////////////////////////////
ruNodeHandle ruCreateCamera( float fov );
void ruSetActiveCamera( ruNodeHandle node );
int ruSetCameraSkybox( ruNodeHandle node, const string & path );
void ruSetCameraFOV( ruNodeHandle camera, float fov );

////////////////////////////////////////////////////////////////////////////////////
// Sounds functions
////////////////////////////////////////////////////////////////////////////////////
ruSoundHandle ruLoadSound2D( const string & file );
ruSoundHandle ruLoadSound3D( const string & file );
ruSoundHandle ruLoadMusic( const string & file );
void ruAttachSound( ruSoundHandle sound, ruNodeHandle node );
void ruPlaySound( ruSoundHandle sound, int oneshot = 1 );
void ruPauseSound( ruSoundHandle sound );
void ruSetSoundVolume( ruSoundHandle sound, float vol );
void ruSetSoundPosition( ruSoundHandle sound, ruVector3 pos );
void ruSetSoundReferenceDistance( ruSoundHandle sound, float rd );
void ruSetRolloffFactor( ruSoundHandle sound, float rolloffDistance );
void ruSetRoomRolloffFactor( ruSoundHandle sound, float rolloffDistance );
void ruSetSoundMaxDistance( ruSoundHandle sound, float maxDistance );
int ruIsSoundPlaying( ruSoundHandle sound );
void ruFreeSound( ruSoundHandle sound );
void ruSetAudioReverb( int reverb );
void ruSetMasterVolume( float volume );
float ruGetMasterVolume();
bool ruIsNodeHasBody( ruNodeHandle node );
void ruSetSoundsPitch( ruSoundHandle sound, float pitch );
bool ruIsSoundPaused( ruSoundHandle sound );
////////////////////////////////////////////////////////////////////////////////////
// Light functions
////////////////////////////////////////////////////////////////////////////////////
ruNodeHandle ruCreateLight( int type = LT_POINT );
void ruSetLightRange( ruNodeHandle node, float rad );
float ruGetLightRange( ruNodeHandle node );
void ruSetLightColor( ruNodeHandle node, ruVector3 clr );
void ruSetConeAngles( ruNodeHandle node, float innerAngle, float outerAngle );
void ruSetLightSpotTexture( ruNodeHandle node, ruTextureHandle texture );
int ruGetWorldSpotLightCount();
ruNodeHandle ruGetWorldSpotLight( int n );
int ruGetWorldPointLightCount();
ruNodeHandle ruGetWorldPointLight( int n );
void ruSetLightFlare( ruNodeHandle node, ruTextureHandle flareTexture );
void ruSetLightDefaultFlare( ruTextureHandle defaultFlareTexture );
void ruSetLightSpotDefaultTexture( ruTextureHandle defaultSpotTexture );
void ruSetLightPointTexture( ruNodeHandle node, ruCubeTextureHandle cubeTexture );
void ruSetLightPointDefaultTexture( ruCubeTextureHandle defaultPointTexture );
void ruSetLightFloatingLimits( ruNodeHandle node, ruVector3 floatMin, ruVector3 floatMax );
void ruSetLightFloatingEnabled( ruNodeHandle node, bool state );
bool ruIsLightFloatingEnabled( ruNodeHandle node );
bool ruIsLight( ruNodeHandle node );
////////////////////////////////////////////////////////////////////////////////////
// Physics functions
////////////////////////////////////////////////////////////////////////////////////
enum class BodyType {
	None,
	Sphere,
	Box,
	Trimesh,
	Convex
};
void ruUpdatePhysics( float timeStep, int subSteps, float fixedTimeStep );
int ruGetContactCount( ruNodeHandle node );
ruContact ruGetContact( ruNodeHandle node, int num );
void ruFreeze( ruNodeHandle node );
void ruUnfreeze( ruNodeHandle node );
void ruSetConvexBody( ruNodeHandle node );
void ruSetCapsuleBody( ruNodeHandle node, float height, float radius );
void ruSetAngularFactor( ruNodeHandle node, ruVector3 fact );
void ruSetTrimeshBody( ruNodeHandle node );
void ruMoveNode( ruNodeHandle node, ruVector3 speed );
void ruSetNodeFriction( ruNodeHandle node, float friction );
void ruSetNodeBodyLocalScale( ruNodeHandle node, ruVector3 scale );
void ruSetNodeAnisotropicFriction( ruNodeHandle node, ruVector3 aniso );
float ruGetNodeMass( ruNodeHandle node );
int ruIsNodeFrozen( ruNodeHandle node );
void ruSetNodeLinearFactor( ruNodeHandle node, ruVector3 lin );
void ruSetNodeVelocity( ruNodeHandle node, ruVector3 velocity );
void ruSetNodeAngularVelocity( ruNodeHandle node, ruVector3 velocity );
ruNodeHandle ruRayPick( int x, int y, ruVector3 * outPickPoint = 0 );
ruNodeHandle ruCastRay( ruVector3 begin, ruVector3 end, ruVector3 * outPickPoint = 0 );
ruVector3 ruGetNodeEulerAngles( ruNodeHandle node );
ruVector3 ruGetNodeAbsoluteLookVector( ruNodeHandle node );
BodyType ruGetNodeBodyType( ruNodeHandle node );
ruVector3 ruGetNodeRotationAxis( ruNodeHandle node );
float ruGetNodeRotationAngle( ruNodeHandle node );
void ruNodeAddForce( ruNodeHandle node, ruVector3 force );
void ruNodeAddForceAtPoint( ruNodeHandle node, ruVector3 force, ruVector3 point );
void ruNodeAddTorque( ruNodeHandle node, ruVector3 torque );
////////////////////////////////////////////////////////////////////////////////////
// Scene node functions
////////////////////////////////////////////////////////////////////////////////////
// Creation
ruNodeHandle ruCreateSceneNode( );
ruNodeHandle ruLoadScene( const string & file );
ruNodeHandle ruFindByName( const string & name );
ruNodeHandle ruFindInObjectByName( ruNodeHandle node, const string & name );
ruNodeHandle ruCreateNodeInstance( ruNodeHandle source );
int ruGetWorldObjectsCount();
ruNodeHandle ruGetWorldObject( int i );
// Common
string ruGetProperty( ruNodeHandle node, string propName );
void ruHideNode( ruNodeHandle node );
void ruShowNode( ruNodeHandle node );
bool ruIsNodeVisible( ruNodeHandle node );
void ruFreeSceneNode( ruNodeHandle node );
const string & ruGetNodeName( ruNodeHandle node );
void ruSetNodeDepthHack( ruNodeHandle node, float order );
void ruAttachNode( ruNodeHandle node1, ruNodeHandle node2 );
void ruDetachNode( ruNodeHandle node );
void ruSetNodeDamping( ruNodeHandle node, float linearDamping, float angularDamping );
void ruSetNodePosition( ruNodeHandle node, ruVector3 position );
void ruSetNodeRotation( ruNodeHandle node, ruQuaternion rotation );
void ruSetNodeGravity( ruNodeHandle node, ruVector3 gravity );
ruVector3 ruGetNodeLookVector( ruNodeHandle node );
ruVector3 ruGetNodeRightVector( ruNodeHandle node );
ruVector3 ruGetNodeUpVector( ruNodeHandle node );
ruVector3 ruGetNodePosition( ruNodeHandle node );
void ruSetNodeMass( ruNodeHandle node, float mass );
ruQuaternion ruGetNodeLocalRotation( ruNodeHandle node );
ruVector3 ruGetNodeLocalPosition( ruNodeHandle node );
void ruSetNodeLocalPosition( ruNodeHandle node, ruVector3 pos );
void ruSetNodeLocalRotation( ruNodeHandle node, ruQuaternion rot );
void ruSetNodeName( ruNodeHandle node, const string & name );
ruVector3 ruGetNodeAABBMin( ruNodeHandle node );
ruVector3 ruGetNodeBodyTotalForce( ruNodeHandle node );
ruVector3 ruGetNodeAABBMax( ruNodeHandle node );
int ruIsNodeInsideNode( ruNodeHandle node1, ruNodeHandle node2 );
ruNodeHandle ruGetNodeChild( ruNodeHandle node, int i );
int ruGetNodeCountChildren( ruNodeHandle node );
bool ruIsNodeInFrustum( ruNodeHandle node );
void ruSetNodeAlbedo( ruNodeHandle node, float albedo );
bool ruIsLightSeePoint( ruNodeHandle node, ruVector3 point );
ruVector3 ruGetNodeLinearVelocity( ruNodeHandle node );
bool ruIsNodeHandleValid( ruNodeHandle handle );
bool ruIsLightHandeValid( ruNodeHandle handle );
// Octree manipulation
// Note, that nodes with octree's optimization can't be moved or modified, so
// it can be used for large static geometry, like game levels
// for dynamic objects engine uses frustum culling
// Also octree can be assigned to object by ' octree="1"; ' user property in
// 3ds max or other editor.
// Octree can't be modified. So if you want set another split criteria
// you must call CreateOctree, instead using combination of DeleteOctree and
// CreateOctree
void ruCreateOctree( ruNodeHandle node, int splitCriteria = 45 );
void ruDeleteOctree( ruNodeHandle node );

// Animation
class ruAnimation {
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
    void Update( float dt = 1.0f / 60.0f );
};

int ruIsAnimationEnabled( ruNodeHandle node );
void ruSetAnimationEnabled( ruNodeHandle node, bool state, bool dontAffectChilds = false );
void ruSetAnimation( ruNodeHandle node, ruAnimation * newAnim, bool dontAffectChilds = false );
int ruGetTotalAnimationFrameCount( ruNodeHandle node );
ruAnimation * ruGetCurrentAnimation( ruNodeHandle node );

////////////////////////////////////////////////////////////////////////////////////
// Font functions
////////////////////////////////////////////////////////////////////////////////////
ruFontHandle ruCreateGUIFont( int size, const string & name, int italic, int underlined );

////////////////////////////////////////////////////////////////////////////////////
// GUI functions
////////////////////////////////////////////////////////////////////////////////////
ruRectHandle ruCreateGUIRect( float x, float y, float w, float h, ruTextureHandle texture, ruVector3 color = ruVector3( 255, 255, 255 ), int alpha = 255 );
ruTextHandle ruCreateGUIText( const string & text, int x, int y, int w, int h, ruFontHandle font, ruVector3 color, int textAlign, int alpha = 255 );
ruButtonHandle ruCreateGUIButton( int x, int y, int w, int h, ruTextureHandle texture, const string & text, ruFontHandle font, ruVector3 color, int textAlign, int alpha = 255 );
void ruSetGUINodeText( ruTextHandle node, const string & text );
void ruSetGUINodePosition( ruGUINodeHandle node, float x, float y );
void ruSetGUINodeSize( ruGUINodeHandle node, float w, float h );
void ruSetGUINodeColor( ruGUINodeHandle node, ruVector3 color );
void ruSetGUINodeAlpha( ruGUINodeHandle node, int alpha );
void ruSetGUINodeVisible( ruGUINodeHandle node, bool visible );
bool ruIsGUINodeVisible( ruGUINodeHandle node );
ruVector2 ruGetGUINodePosition( ruGUINodeHandle node );
ruVector2 ruGetGUINodeSize( ruGUINodeHandle node );
ruVector3 ruGetGUINodeColor( ruGUINodeHandle node );
void ruSetGUINodeTexture( ruGUINodeHandle node, ruTextureHandle texture );
int ruGetGUINodeAlpha( ruGUINodeHandle node );
bool ruIsButtonPressed( ruButtonHandle node );
bool ruIsButtonHit( ruButtonHandle node );
bool ruIsButtonPicked( ruButtonHandle node );
ruTextHandle ruGetButtonText( ruButtonHandle node );
void ruFreeGUINode( ruGUINodeHandle node );
////////////////////////////////////////////////////////////////////////////////////
// Time functions
////////////////////////////////////////////////////////////////////////////////////
typedef int ruTimerHandle;

ruTimerHandle ruCreateTimer( );
void ruRestartTimer( ruTimerHandle timer );
double ruGetElapsedTimeInSeconds( ruTimerHandle timer );
double ruGetElapsedTimeInMilliSeconds( ruTimerHandle timer );
double ruGetElapsedTimeInMicroSeconds( ruTimerHandle timer );
double ruGetTimeInSeconds( ruTimerHandle timer );
double ruGetTimeInMilliSeconds( ruTimerHandle timer );
double ruGetTimeInMicroSeconds( ruTimerHandle timer );
////////////////////////////////////////////////////////////////////////////////////
// Particle system functions
////////////////////////////////////////////////////////////////////////////////////
#define PS_BOX ( 0 )
#define PS_STREAM ( 1 )

class ruParticleSystemProperties {
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
	float depthHack;

    ruTextureHandle texture;

    explicit ruParticleSystemProperties() {
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

		depthHack = 0.0f;

        autoResurrectDeadParticles = true;
        useLighting = false;

        enabled = true;

        texture = ruTextureHandle::Empty();
    }
};

ruNodeHandle ruCreateParticleSystem( int particleNum, ruParticleSystemProperties creationProps );
int ruGetParticleSystemAliveParticles( ruNodeHandle ps );
void ruResurrectDeadParticles( ruNodeHandle ps );
ruParticleSystemProperties * ruGetParticleSystemProperties( ruNodeHandle ps );
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

void ruInputInit( void * window );
void ruInputDestroy( );
int	ruIsMouseDown( int button );
int	ruIsMouseHit( int button );
int	ruGetMouseX( );
int	ruGetMouseY( );
int	ruGetMouseWheel( );
int	ruGetMouseXSpeed( );
int	ruGetMouseYSpeed( );
int	ruGetMouseWheelSpeed	( );
int	ruIsKeyDown( int key );
int	ruIsKeyHit( int key );
int	ruIsKeyUp( int key );
void ruInputUpdate( );


#endif