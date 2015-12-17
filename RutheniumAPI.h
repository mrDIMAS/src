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

#ifndef _ENGINE_
#define _ENGINE_

#pragma warning( disable:4250 )

#include <vector>
#include <math.h>
#include <string>
#include <windows.h>
#include <sstream>
#include <iomanip>
#include <memory>
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


	ruVector3( ) : x( 0.0f ), y( 0.0f ), z( 0.0f ) { };
	ruVector3( float x, float y, float z ) : x( x ), y( y ), z( z ) { };

	ruVector3( const ruVector3 & v ) {
		x = v.x;
		y = v.y;
		z = v.z;
	};

	ruVector3( float * v ) {
		x = v[ 0 ];
		y = v[ 1 ];
		z = v[ 2 ];
	}

	ruVector3 operator + ( const ruVector3 & v ) const {
		return ruVector3( x + v.x, y + v.y, z + v.z );
	}

	ruVector3 operator - ( const ruVector3 & v ) const {
		return ruVector3( x - v.x, y - v.y, z - v.z );
	}

	ruVector3 operator * ( const ruVector3 & v ) const {
		return ruVector3( x * v.x, y * v.y, z * v.z );
	}

	ruVector3 operator * ( const float & f ) const {
		return ruVector3( x * f, y * f, z * f );
	}

	ruVector3 operator / ( const ruVector3 & v ) const {
		return ruVector3( x / v.x, y / v.y, z / v.z );
	}

	ruVector3 operator / ( const float & f ) const {
		return ruVector3( x / f, y / f, z / f );
	}

	void operator *= ( const ruVector3 & v ) {
		x *= v.x;
		y *= v.y;
		z *= v.z;
	}

	float Angle( const ruVector3 & v ) {
		return acosf( Dot(v) / sqrtf( Length2() * v.Length2()) );
	}

	ruVector3 Abs( ) const {
		return ruVector3( abs( x ), abs( y ), abs( z ));
	}

	void operator /= ( const ruVector3 & v ) {
		x /= v.x;
		y /= v.y;
		z /= v.z;
	}

	void operator += ( const ruVector3 & v ) {
		x += v.x;
		y += v.y;
		z += v.z;
	}

	void operator -= ( const ruVector3 & v ) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
	}

	void operator = ( const ruVector3 & v ) {
		x = v.x;
		y = v.y;
		z = v.z;
	}

	bool operator == ( const ruVector3 & v ) {
		float dx = abs( x - v.x );
		float dy = abs( y - v.y );
		float dz = abs( z - v.z );

		return dx > 0.0001f && dy > 0.0001f && dz > 0.0001f;
	}

	float Length( ) const {
		return sqrt( x * x + y * y + z * z );
	}

	float Length2( ) const {
		return x * x + y * y + z * z;
	}

	ruVector3 Normalize( ) {
		float l = 1.0f / Length();

		x *= l;
		y *= l;
		z *= l;

		return *this;
	}

	ruVector3 Normalized() const {
		float l = 1.0f / Length();
		return ruVector3( x * l, y * l, z * l );
	}

	ruVector3 Cross( const ruVector3 & v ) const {
		return ruVector3( y * v.z - z * v.x, z * v.x - x * v.z, x * v.y - y * v.x );
	}

	float Dot( const ruVector3 & v ) const {
		return x * v.x + y * v.y + z * v.z;
	}

	ruVector3 Rotate( const ruVector3 & axis, float angle ) {
		angle *= 3.14159f / 180.0f;

		ruVector3 o = axis * axis.Dot( *this );
		ruVector3 x = *this - o;
		ruVector3 y;

		y = axis.Cross( *this );

		return ( o + x * cosf( angle ) + y * sinf( angle ) );
	}

	ruVector3 Lerp( const ruVector3 & v, float t ) const {
		return ruVector3( x + ( v.x - x ) * t, y + ( v.y - y ) * t, z + ( v.z - z ) * t );
	}
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

class ruVector2 {
public:
    float x, y;

    ruVector2( ) {
        x = 0;
        y = 0;
    };

    ruVector2( float _x, float _y ) : x( _x ), y( _y ) { };
};

class ruVector4 {
public:
	union {
		struct {
			float x, y, z, w;
		};
		float c[4];
	};

	ruVector4( float _x, float _y, float _z, float _w ) : x( _x ), y( _y ), z( _z ), w( _w ) { };
	ruVector4( ) : x( 0.0f ), y( 0.0f ), z( 0.0f ), w( 0.0f ) { };
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

#define BODY_MAX_CONTACTS ( 16 )

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
		AnimationEvent( ) : mState( false ) { }
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

enum class BodyType : int {
	None, Sphere, Cylinder,	Box, Trimesh, Convex
};

class ruObject {
protected:
	virtual ~ruObject() { };
public:
	virtual void Free();
};

class ruTexture {
public:	
	virtual ~ruTexture();
	static shared_ptr<ruTexture> Request( const string & file );
	virtual string GetName() = 0;
	virtual int GetWidth() = 0;
	virtual int GetHeight() = 0;
};

class ruCubeTexture {
protected:
	virtual ~ruCubeTexture();
public:
	static ruCubeTexture * Request( const string & file );	
};

class ruSceneNode : public ruObject {
protected:
	virtual ~ruSceneNode( );
public:
	virtual void SetBlurAmount( float blurAmount ) = 0;
	virtual float GetBlurAmount( ) = 0;
	virtual string GetProperty( string propName ) = 0;
	virtual void Hide() = 0;
	virtual void Show() = 0;
	virtual bool IsVisible() = 0;	
	virtual const string & GetName() = 0;
	virtual void SetDepthHack( float order ) = 0;
	virtual void Attach( ruSceneNode * parent ) = 0;
	virtual void Detach() = 0;
	virtual void SetDamping( float linearDamping, float angularDamping ) = 0;
	virtual void SetPosition( ruVector3 position ) = 0;
	virtual void SetRotation( ruQuaternion rotation ) = 0;
	virtual void SetGravity( const ruVector3 & gravity ) = 0;
	virtual ruVector3 GetLookVector() = 0;
	virtual ruVector3 GetRightVector() = 0;
	virtual ruVector3 GetUpVector() = 0;
	virtual ruVector3 GetPosition() = 0;
	virtual void SetMass( float mass ) = 0;
	virtual ruQuaternion GetLocalRotation() = 0;
	virtual ruVector3 GetLocalPosition() = 0;
	virtual void SetLocalPosition( ruVector3 pos ) = 0;
	virtual void SetLocalRotation( ruQuaternion rot ) = 0;
	virtual void SetName( const string & name ) = 0;
	virtual ruVector3 GetAABBMin() = 0;
	virtual ruVector3 GetTotalForce() = 0;
	virtual ruVector3 GetAABBMax() = 0;
	virtual bool IsInsideNode( ruSceneNode * node ) = 0;
	virtual ruSceneNode * GetChild( int i ) = 0;
	virtual int GetCountChildren() = 0;
	virtual bool IsInFrustum() = 0;
	virtual void SetAlbedo( float albedo ) = 0;
	virtual ruVector3 GetLinearVelocity() = 0;
	virtual int GetContactCount() = 0;
	virtual ruContact GetContact( int num ) = 0;
	virtual void Freeze() = 0;
	virtual void Unfreeze() = 0;
	virtual void SetConvexBody() = 0;
	virtual void SetCapsuleBody( float height, float radius ) = 0;
	virtual void SetAngularFactor( ruVector3 fact ) = 0;
	virtual void SetTrimeshBody( ) = 0;
	virtual void Move( ruVector3 speed ) = 0;
	virtual void SetFriction( float friction ) = 0;
	virtual void SetLocalScale( ruVector3 scale ) = 0;
	virtual void SetAnisotropicFriction( ruVector3 aniso ) = 0;
	virtual float GetMass() = 0;
	virtual bool IsFrozen() = 0;
	virtual void SetLinearFactor( ruVector3 lin ) = 0;
	virtual void SetVelocity( ruVector3 velocity ) = 0;
	virtual void SetAngularVelocity( ruVector3 velocity ) = 0;
	virtual void SetAnimation( ruAnimation * newAnim, bool dontAffectChilds = false ) = 0;
	virtual int GetTotalAnimationFrameCount() = 0;
	virtual ruAnimation * GetCurrentAnimation() = 0;
	virtual ruVector3 GetEulerAngles() = 0;
	virtual ruVector3 GetAbsoluteLookVector() = 0;
	virtual BodyType GetBodyType() const = 0;
	virtual ruVector3 GetRotationAxis() = 0;
	virtual float GetRotationAngle() = 0;
	virtual void AddForce( ruVector3 force ) = 0;
	virtual void AddForceAtPoint( ruVector3 force, ruVector3 point ) = 0;
	virtual void AddTorque( ruVector3 torque ) = 0;
	virtual ruSceneNode * GetParent() = 0;
	virtual int GetTextureCount() = 0;
	virtual shared_ptr<ruTexture> GetTexture( int n ) = 0;
	virtual ruSceneNode * FindChild( const string & name ) = 0;
	//virtual void Free();
	static ruSceneNode * Create( );
	static ruSceneNode * LoadFromFile( const string & file );
	static ruSceneNode * FindByName( const string & name );
	static ruSceneNode * Duplicate( ruSceneNode * source );
	static int GetWorldObjectsCount();
	static ruSceneNode * GetWorldObject( int i );
};

struct ruContact {
	ruVector3 normal;
	ruVector3 position;
	float impulse;
	ruSceneNode * body;
	string textureName;
	ruContact() {
		impulse = 0;
	}
};

class ruSound {
public:
    int pfHandle;

    explicit ruSound();
    virtual ~ruSound();

    bool operator == ( const ruSound & node );
    virtual bool IsValid() const;
    virtual void Invalidate();

	static ruSound Load2D( const string & file );
	static ruSound Load3D( const string & file );
	static ruSound LoadMusic( const string & file );
	static int GetCount();
	static ruSound GetSound( int n );
	static void SetAudioReverb( int reverb );
	static void SetMasterVolume( float volume );
	static float GetMasterVolume();

	void Attach( ruSceneNode * node );
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

enum class ruGUIAction : int {
	OnClick, OnMouseEnter, OnMouseLeave
};

enum class ruTextAlignment : int {
	Left, Center
};

class ruFont {
protected:
	virtual ~ruFont();
public:	
	static ruFont * LoadFromFile( int size, const string & name );
	virtual void Free();
};

class ruGUINode : public ruObject {
protected:
	virtual ~ruGUINode();
public:	
    virtual void Attach( ruGUINode * parent ) = 0;
	virtual void AddAction( ruGUIAction action, const ruDelegate & delegat ) = 0;
	virtual void RemoveAction( ruGUIAction action ) = 0;
	virtual void RemoveAllActions() = 0;
	virtual void SetPosition( float x, float y ) = 0;
	virtual void SetSize( float w, float h ) = 0;
	virtual void SetColor( ruVector3 color ) = 0;
	virtual void SetAlpha( int alpha ) = 0;
	virtual void SetVisible( bool visible ) = 0;
	virtual bool IsVisible() = 0;
	virtual ruVector2 GetPosition() = 0;
	virtual ruVector2 GetSize() = 0;
	virtual ruVector3 GetColor() = 0;
	virtual void SetTexture( shared_ptr<ruTexture> pTexture ) = 0;
	virtual int GetAlpha() = 0;
	//virtual void Free();
	virtual void SetChildAlphaControl( bool controlChildAlpha ) = 0;
	virtual shared_ptr<ruTexture> GetTexture( ) = 0;
};

class ruRect : public virtual ruGUINode {
public:
	static ruRect * Create( float x, float y, float w, float h, shared_ptr<ruTexture> texture, ruVector3 color = ruVector3( 255, 255, 255 ), int alpha = 255 );
};

class ruText : public virtual ruGUINode {
public:
	static ruText * Create( const string & text, int x, int y, int w, int h, ruFont * font, ruVector3 color, ruTextAlignment textAlign, int alpha = 255 );
	virtual void SetText( const string & text ) = 0;
};

class ruButton : public virtual ruRect {
public:
	static ruButton * Create( int x, int y, int w, int h, shared_ptr<ruTexture> texture, const string & text, ruFont * font, ruVector3 color, ruTextAlignment textAlign, int alpha = 255 );
	virtual bool IsPressed() = 0;
	virtual bool IsHit() const = 0;
	virtual bool IsRightPressed() = 0;
	virtual bool IsRightHit() const = 0;
	virtual bool IsPicked() = 0;
	virtual ruText * GetText() = 0;
	virtual void SetActive( bool state ) = 0;
	virtual void SetPickedColor(ruVector3 val ) = 0;
	virtual ruVector3 GetPickedColor() const = 0;
};

struct ruRayCastResultEx {
	bool valid;
	int index;
	ruSceneNode * node;
	ruVector3 position;
	ruVector3 normal;
	string textureName;
};

class ruEngine {
public:
	static void Create( int width, int height, int fullscreen, char vSync );
	static void Free( );
	static void RenderWorld( );
	static int GetResolutionWidth( );
	static int GetResolutionHeight( );
	static void HideCursor( );
	static void ShowCursor( );
	static void SetCursorSettings( shared_ptr<ruTexture> texture, int w, int h );
	static int GetDIPs( );
	static int GetTextureUsedPerFrame( );
	static void SetAmbientColor( ruVector3 color );
	static int GetAvailableTextureMemory();
	static void EnableShadows( bool state );
	static void UpdateWorld();
	static void SetAnisotropicTextureFiltration( bool state );
	static void ChangeVideomode( int width, int height, int fullscreen, char vSync );
	// FXAA
	static void SetFXAAEnabled( bool state );
	static bool IsFXAAEnabled();	
	// HDR
	static void SetHDREnabled( bool state );
	static bool IsHDREnabled( );

	// Parallax Occlusion Mapping
	static void SetParallaxEnabled( bool state );
	static bool IsParallaxEnabled();

	// Shadow functions
	static void SetSpotLightShadowMapSize( int size );
	static void EnableSpotLightShadows( bool state );
	static bool IsSpotLightShadowsEnabled();
	static int GetMaxAnisotropy();
};

class ruPhysics {
public:
	static void Update( float timeStep, int subSteps, float fixedTimeStep );
	static ruRayCastResultEx CastRayEx( ruVector3 begin, ruVector3 end );
	static ruSceneNode * RayPick( int x, int y, ruVector3 * outPickPoint = 0 );
	static ruSceneNode * CastRay( ruVector3 begin, ruVector3 end, ruVector3 * outPickPoint = 0 );
};

class ruLight : public virtual ruSceneNode {
protected:
	virtual ~ruLight();
public:
	virtual void SetRange( float range ) = 0;
	virtual float GetRange() const = 0;

	virtual void SetColor( const ruVector3 & clr ) = 0;	
	virtual ruVector3 GetColor( ) const = 0;
	
	virtual void SetGreyscaleFactor( float factor ) = 0;
	virtual float GetGrayscaleFactor( ) const = 0;

	virtual bool IsSeePoint( const ruVector3 & point ) = 0;
};

class ruSpotLight : public virtual ruLight {
public:
	static ruSpotLight * Create();
	static int GetCount();
	static ruSpotLight * Get( int n );
	static void SetSpotDefaultTexture( shared_ptr<ruTexture> defaultSpotTexture );

	virtual void SetSpotTexture( shared_ptr<ruTexture> texture ) = 0;	
	virtual void SetConeAngles( float innerAngle, float outerAngle ) = 0;
	virtual bool IsSeePoint( const ruVector3 & point ) = 0;
};

class ruPointLight : public virtual ruLight {
public:
	static ruPointLight * Create();
	static int GetCount();
	static ruPointLight * Get( int n );
	static void SetPointDefaultTexture( ruCubeTexture * defaultPointTexture );

	virtual void SetPointTexture( ruCubeTexture * cubeTexture ) = 0;
	virtual bool IsSeePoint( const ruVector3 & point ) = 0;
};

class ruCamera : public virtual ruSceneNode {
public:
	static ruCamera * Create( float fov );
	virtual void SetActive() = 0;
	virtual void SetSkybox( shared_ptr<ruTexture> up, shared_ptr<ruTexture> left, shared_ptr<ruTexture> right, shared_ptr<ruTexture> forward, shared_ptr<ruTexture> back ) = 0;
	virtual void SetFOV( float fov ) = 0;
};

class ruTimer : public ruObject {
protected:
	virtual ~ruTimer();
public:
	static ruTimer * Create( );
	virtual void Restart() = 0;
	virtual double GetTimeInSeconds() = 0;
	virtual double GetTimeInMilliSeconds() = 0;
	virtual double GetTimeInMicroSeconds() = 0;
	virtual double GetElapsedTimeInSeconds() = 0;
	virtual double GetElapsedTimeInMilliSeconds() = 0;
	virtual double GetElapsedTimeInMicroSeconds() = 0;
};

class ruParticleSystem : public virtual ruSceneNode {
public:
	enum class Type {
		Box, Stream
	};

	static ruParticleSystem * Create( int particleNum );

	virtual void SetType( ruParticleSystem::Type type ) = 0;
	virtual ruParticleSystem::Type GetType() = 0;

	virtual int GetAliveParticles() = 0;
	virtual void ResurrectParticles() = 0;

	virtual bool IsEnabled() = 0;
	virtual void SetEnabled( bool state ) = 0;

	virtual void SetPointSize( float size ) = 0;
	virtual float GetPointSize() = 0;

	virtual void SetScaleFactor( float scaleFactor ) = 0;
	virtual float GetScaleFactor( ) = 0;

	virtual void SetParticleThickness( float thickness ) = 0;
	virtual float GetParticleThickness( ) = 0;

	virtual void SetAutoResurrection( bool state ) = 0;
	virtual bool IsAutoResurrectionEnabled() = 0;

	virtual void SetLightingEnabled( bool state ) = 0;
	virtual bool IsLightingEnabled( ) = 0;

	virtual void SetBoundingBox( const ruVector3 & bbMin, const ruVector3 & bbMax ) = 0;
	virtual ruVector3 GetBoundingBoxMax( ) = 0;
	virtual ruVector3 GetBoundingBoxMin( ) = 0;

	virtual void SetSpeedDeviation( const ruVector3 & dMin, const ruVector3 & dMax ) = 0;
	virtual ruVector3 GetSpeedDeviationMax() = 0;
	virtual ruVector3 GetSpeedDeviationMin() = 0;

	virtual void SetTexture( shared_ptr<ruTexture> texture ) = 0;
	virtual shared_ptr<ruTexture> GetTexture( ) = 0;

	virtual void SetBoundingRadius( float radius ) = 0;
	virtual float GetBoundingRadius( ) = 0;

	virtual void SetColorRange( const ruVector3 & cMin, const ruVector3 & cMax ) = 0;
	virtual ruVector3 GetColorMin( ) = 0;
	virtual ruVector3 GetColorMax( ) = 0;

	virtual float GetAlphaOffset( ) = 0;
	virtual void SetAlphaOffset( float alphaOffset ) = 0;
};


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

void ruInputInit( HWND window );
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