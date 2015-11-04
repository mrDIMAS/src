// C++11 API Header for Ruthenium Engine

#ifndef _ENGINE_
#define _ENGINE_

#include <vector>
#include <math.h>
#include <string>
#include <windows.h>
#include <sstream>
#include <iomanip>
#include <unordered_map>

using namespace std;

class ruIContainer {
public:
	virtual ~ruIContainer() {	

	};

	virtual void Call() = 0;
};

template< class T, class M > class ruContainer : public ruIContainer {
private:
	T * mClass;
	M mMethod;
public:
	explicit ruContainer( T * theClass, M  theMethod ) : mClass( theClass ), mMethod( theMethod ) {

	};

	virtual void Call() {
		(mClass->*mMethod)();
	}
};

class ruDelegate {
private:
	ruIContainer * mContainer;
public:
	ruDelegate() : mContainer( nullptr ) {

	};
	~ruDelegate() {
		if( mContainer ) {
			delete mContainer;
		}
	}
	ruDelegate( const ruDelegate & other ) {
		mContainer = other.mContainer;
		(const_cast<ruDelegate&>(other)).mContainer = nullptr;
	}
	template< class T, class M > static ruDelegate Bind( T * theClass, M  theMethod ) {
		ruDelegate delegat;
		delegat.mContainer = new ruContainer< T, M >( theClass, theMethod );
		return delegat;
	}
	void Call() {
		if( mContainer ) {
			mContainer->Call();
		}
	}
	void operator = ( const ruDelegate & other ) {
		mContainer = other.mContainer;
		(const_cast<ruDelegate&>(other)).mContainer = nullptr;
	}
};

class ruEvent {
private:
	vector<ruDelegate> mListenerList;
public:
	void AddListener( const ruDelegate & delegat ) {
		mListenerList.push_back( delegat );
	}

	void RemoveAllListeners() {
		mListenerList.clear();
	}

	int GetActionCount() {
		return mListenerList.size();
	}

	void DoActions() {
		for( auto iter = mListenerList.begin(); iter != mListenerList.end(); iter++ ) {
			iter->Call();
		}
	}
};

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
	ruVector3 Abs( ) const;
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
protected:
	
public:
	void * pointer;

	explicit ruRutheniumHandle();
    virtual ~ruRutheniumHandle();

    virtual bool IsValid();
    virtual void Invalidate();
};

struct ruContact;


// Animation
class ruAnimation {
private:
	friend class SceneNode;
	int beginFrame;
	int endFrame;
	int currentFrame;
	int nextFrame;
	float interpolator;
	string mName;
	class AnimationEvent {
	public:
		bool mState;
		ruEvent Event;
		AnimationEvent( ) : mState( false ) {

		}
	};
	// list of actions, which must be done on n-th frame 
	unordered_map<int,AnimationEvent> mFrameListenerList;
public:
	// list of all animations
	static vector<ruAnimation*> msAnimationList;
	float duration;
	bool looped;
	bool enabled;
	
	explicit ruAnimation();
	explicit ruAnimation( int theBeginFrame, int theEndFrame, float theDuration, bool theLooped = false );
	virtual ~ruAnimation();
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
	void SetName( const string & newName ) {
		mName = newName;
	}
	string GetName( ) {
		return mName;
	}
	void AddFrameListener( int frameNum, const ruDelegate & action );
	void Rewind();
	void Update( float dt = 1.0f / 60.0f );
};

enum class BodyType {
	None,
	Sphere,
	Box,
	Trimesh,
	Convex
};

class ruSceneNode : public ruRutheniumHandle {
public:
    bool operator == ( const ruSceneNode & node );

	virtual bool IsValid();

	string GetProperty( string propName );
	void Hide();
	void Show();
	bool IsVisible();
	void Free();
	const string & GetName();
	void SetDepthHack( float order );
	void Attach( ruSceneNode parent );
	void Detach();
	void SetDamping( float linearDamping, float angularDamping );
	void SetPosition( ruVector3 position );
	void SetRotation( ruQuaternion rotation );
	void SetGravity( ruVector3 gravity );
	ruVector3 GetLookVector();
	ruVector3 GetRightVector();
	ruVector3 GetUpVector();
	ruVector3 GetPosition();
	void SetMass( float mass );
	ruQuaternion GetLocalRotation();
	ruVector3 GetLocalPosition();
	void SetLocalPosition( ruVector3 pos );
	void SetLocalRotation( ruQuaternion rot );
	void SetName( const string & name );
	ruVector3 GetAABBMin();
	ruVector3 GetTotalForce();
	ruVector3 GetAABBMax();
	int IsInsideNode( ruSceneNode node );
	ruSceneNode GetChild( int i );
	int GetCountChildren();
	bool IsInFrustum();
	void SetAlbedo( float albedo );
	ruVector3 GetLinearVelocity();
	int GetContactCount();
	ruContact GetContact( int num );
	void Freeze();
	void Unfreeze();
	void SetConvexBody();
	void SetCapsuleBody( float height, float radius );
	void SetAngularFactor( ruVector3 fact );
	void SetTrimeshBody( );
	void Move( ruVector3 speed );
	void SetFriction( float friction );
	void SetLocalScale( ruVector3 scale );
	void SetAnisotropicFriction( ruVector3 aniso );
	float GetMass();
	int IsFrozen();
	void SetLinearFactor( ruVector3 lin );
	void SetVelocity( ruVector3 velocity );
	void SetAngularVelocity( ruVector3 velocity );
	void SetAnimation( ruAnimation * newAnim, bool dontAffectChilds = false );

	int GetTotalAnimationFrameCount();
	ruAnimation * GetCurrentAnimation();
	ruVector3 GetEulerAngles();
	ruVector3 GetAbsoluteLookVector();
	BodyType GetBodyType();
	ruVector3 GetRotationAxis();
	float GetRotationAngle();
	void AddForce( ruVector3 force );
	void AddForceAtPoint( ruVector3 force, ruVector3 point );
	void AddTorque( ruVector3 torque );
	ruSceneNode GetParent();
};


class ruFontHandle : public ruRutheniumHandle {
public:
    bool operator == ( const ruFontHandle & node );

	void Free();
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

struct ruContact {
	ruVector3 normal;
	ruVector3 position;
	float impulse;
	ruSceneNode body;
	string textureName;
	ruContact() {
		impulse = 0;
	}
};

class ruSound : public ruRutheniumHandle {
public:
    int pfHandle;
    explicit ruSound();
    virtual ~ruSound();

    bool operator == ( const ruSound & node );
    virtual bool IsValid();
    virtual void Invalidate();

	static ruSound Load2D( const string & file );
	static ruSound Load3D( const string & file );
	static ruSound LoadMusic( const string & file );
	static int GetCount();
	static ruSound GetSound( int n );

	void Attach( ruSceneNode node );
	void Play( int oneshot = 1 );
	void Pause();
	void SetVolume( float vol );
	void SetPosition( ruVector3 pos );
	void SetReferenceDistance( float rd );
	void SetRolloffFactor( float rolloffDistance );
	void SetRoomRolloffFactor( float rolloffDistance );
	void SetMaxDistance( float maxDistance );
	int IsPlaying();
	void Free();
	void SetPitch( float pitch );
	bool IsPaused();
	float GetLength();
	void SetLoop( bool state );
	float GetPlaybackPosition( );
	void SetPlaybackPosition( float timeSeconds );

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

class ruEngine {
public:
	static void Create( int width, int height, int fullscreen, char vSync );
	static void Free( );
	static void RenderWorld( );
	static int GetResolutionWidth( );
	static int GetResolutionHeight( );
	static void HideCursor( );
	static void ShowCursor( );
	static void SetCursorSettings( ruTextureHandle texture, int w, int h );
	static int GetDIPs( );
	static int GetTextureUsedPerFrame( );
	static void SetAmbientColor( ruVector3 color );
	static int GetAvailableTextureMemory();
	static void EnableShadows( bool state );
	static void UpdateWorld();
	static void SetAnisotropicTextureFiltration( bool state );
	// FXAA
	static void EnableFXAA( );
	static void DisableFXAA( );
	static bool IsFXAAEnabled();
	static void ChangeVideomode( int width, int height, int fullscreen, char vSync );
	// HDR
	static void SetHDREnabled( bool state );
	static bool IsHDREnabled( );
	// Shadow functions
	static void SetSpotLightShadowMapSize( int size );
	static void EnableSpotLightShadows( bool state );
	static bool IsSpotLightShadowsEnabled();
	static int GetMaxAnisotropy();
};


////////////////////////////////////////////////////////////////////////////////////
// Texture functions
////////////////////////////////////////////////////////////////////////////////////
ruTextureHandle ruGetTexture( const string & file );
ruCubeTextureHandle ruGetCubeTexture( const string & file );
int ruGetTextureWidth( ruTextureHandle texture );
int ruGetTextureHeight( ruTextureHandle texture );

////////////////////////////////////////////////////////////////////////////////////
// Camera functions
////////////////////////////////////////////////////////////////////////////////////
ruSceneNode ruCreateCamera( float fov );
void ruSetActiveCamera( ruSceneNode node );
int ruSetCameraSkybox( ruSceneNode node, ruTextureHandle up, ruTextureHandle left, ruTextureHandle right, ruTextureHandle forward, ruTextureHandle back );
void ruSetCameraFOV( ruSceneNode camera, float fov );

////////////////////////////////////////////////////////////////////////////////////
// Sounds functions
////////////////////////////////////////////////////////////////////////////////////

void ruSetAudioReverb( int reverb );
void ruSetMasterVolume( float volume );
float ruGetMasterVolume();
////////////////////////////////////////////////////////////////////////////////////
// Light functions
////////////////////////////////////////////////////////////////////////////////////
ruSceneNode ruCreateLight( int type = LT_POINT );
void ruSetLightRange( ruSceneNode node, float rad );
float ruGetLightRange( ruSceneNode node );
void ruSetLightColor( ruSceneNode node, ruVector3 clr );
void ruSetConeAngles( ruSceneNode node, float innerAngle, float outerAngle );
void ruSetLightSpotTexture( ruSceneNode node, ruTextureHandle texture );
int ruGetWorldSpotLightCount();
ruSceneNode ruGetWorldSpotLight( int n );
int ruGetWorldPointLightCount();
ruSceneNode ruGetWorldPointLight( int n );
void ruSetLightFlare( ruSceneNode node, ruTextureHandle flareTexture );
void ruSetLightDefaultFlare( ruTextureHandle defaultFlareTexture );
void ruSetLightSpotDefaultTexture( ruTextureHandle defaultSpotTexture );
void ruSetLightPointTexture( ruSceneNode node, ruCubeTextureHandle cubeTexture );
void ruSetLightPointDefaultTexture( ruCubeTextureHandle defaultPointTexture );
void ruSetLightFloatingLimits( ruSceneNode node, ruVector3 floatMin, ruVector3 floatMax );
void ruSetLightFloatingEnabled( ruSceneNode node, bool state );
bool ruIsLightFloatingEnabled( ruSceneNode node );
bool ruIsLight( ruSceneNode node );
////////////////////////////////////////////////////////////////////////////////////
// Physics functions
////////////////////////////////////////////////////////////////////////////////////

void ruUpdatePhysics( float timeStep, int subSteps, float fixedTimeStep );

ruSceneNode ruRayPick( int x, int y, ruVector3 * outPickPoint = 0 );
ruSceneNode ruCastRay( ruVector3 begin, ruVector3 end, ruVector3 * outPickPoint = 0 );

struct ruRayCastResultEx {
	bool valid;
	int index;
	ruSceneNode node;
	ruVector3 position;
	ruVector3 normal;
	string textureName;
};

ruRayCastResultEx ruCastRayEx( ruVector3 begin, ruVector3 end );

////////////////////////////////////////////////////////////////////////////////////
// Scene node functions
////////////////////////////////////////////////////////////////////////////////////
// Creation
ruSceneNode ruCreateSceneNode( );
ruSceneNode ruLoadScene( const string & file );
ruSceneNode ruFindByName( const string & name );
ruSceneNode ruFindInObjectByName( ruSceneNode node, const string & name );
ruSceneNode ruCreateNodeInstance( ruSceneNode source );
int ruGetWorldObjectsCount();
ruSceneNode ruGetWorldObject( int i );
// Common
bool ruIsLightSeePoint( ruSceneNode node, ruVector3 point );
bool ruIsNodeHandleValid( ruSceneNode handle );
bool ruIsLightHandeValid( ruSceneNode handle );
bool ruIsNodeHasBody( ruSceneNode node );
// Octree manipulation
// Note, that nodes with octree's optimization can't be moved or modified, so
// it can be used for large static geometry, like game levels
// for dynamic objects engine uses frustum culling
// Also octree can be assigned to object by ' octree="1"; ' user property in
// 3ds max or other editor.
// Octree can't be modified. So if you want set another split criteria
// you must call CreateOctree, instead using combination of DeleteOctree and
// CreateOctree
void ruCreateOctree( ruSceneNode node, int splitCriteria = 45 );
void ruDeleteOctree( ruSceneNode node );


////////////////////////////////////////////////////////////////////////////////////
// Font functions
////////////////////////////////////////////////////////////////////////////////////
ruFontHandle ruCreateGUIFont( int size, const string & name );

////////////////////////////////////////////////////////////////////////////////////
// GUI functions
////////////////////////////////////////////////////////////////////////////////////
enum class ruGUIAction {
	OnClick,
	OnMouseEnter,
	OnMouseLeave,
};



ruRectHandle ruCreateGUIRect( float x, float y, float w, float h, ruTextureHandle texture, ruVector3 color = ruVector3( 255, 255, 255 ), int alpha = 255 );
ruTextHandle ruCreateGUIText( const string & text, int x, int y, int w, int h, ruFontHandle font, ruVector3 color, int textAlign, int alpha = 255 );
ruButtonHandle ruCreateGUIButton( int x, int y, int w, int h, ruTextureHandle texture, const string & text, ruFontHandle font, ruVector3 color, int textAlign, int alpha = 255 );
void ruAttachGUINode( ruGUINodeHandle node, ruGUINodeHandle parent );
void ruAddGUINodeAction( ruGUINodeHandle node, ruGUIAction action, const ruDelegate & delegat );
void ruRemoveGUINodeAction( ruGUINodeHandle node, ruGUIAction action );
void ruRemoveAllGUINodeActions( ruGUINodeHandle node );
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
void ruSetGUIButtonActive( ruButtonHandle button, bool state );
void ruSetGUINodeChildAlphaControl( ruGUINodeHandle node, bool controlChildAlpha );
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
	float alphaOffset;

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
		alphaOffset = 0.0f;

        autoResurrectDeadParticles = true;
        useLighting = false;

        enabled = true;

        texture = ruTextureHandle::Empty();
    }
};

ruSceneNode ruCreateParticleSystem( int particleNum, ruParticleSystemProperties creationProps );
int ruGetParticleSystemAliveParticles( ruSceneNode ps );
void ruResurrectDeadParticles( ruSceneNode ps );
ruParticleSystemProperties * ruGetParticleSystemProperties( ruSceneNode ps );
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