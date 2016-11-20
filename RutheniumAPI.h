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

#pragma once

#pragma warning( disable:4250 )

#include <vector>
#include <math.h>
#include <string>
#include <windows.h>
#include <sstream>
#include <iomanip>
#include <memory>
#include <unordered_map>
#include <functional>
#include <map>

using namespace std;


typedef function<void()> ruDelegate;

class ruEvent {
private:
	vector<ruDelegate> Actions;
public:
	void operator()() {
		for(auto & f : Actions) f();
	}

	ruEvent & operator += (const ruDelegate & fn) {
		Actions.push_back(fn);
		return *this;
	}

	void Clear() {
		Actions.clear();
	}
};

class StringBuilder {
protected:
	std::stringstream mStream;
public:
	explicit StringBuilder(const char * str) {
		stringstream::sync_with_stdio(false);
		mStream << str;
	}
	explicit StringBuilder() {
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

float frandom(float low, float high);

class ruVector3 {
public:
	union {
		struct { float x, y, z; };
		float elements[3];
	};

	ruVector3() : x(0.0f), y(0.0f), z(0.0f) {}
	ruVector3(float x, float y, float z) : x(x), y(y), z(z) {}
	ruVector3(const ruVector3 & v) : x(v.x), y(v.y), z(v.z) {}
	ruVector3(float * v) : x(v[0]), y(v[1]), z(v[2]) {}
	ruVector3 operator + (const ruVector3 & v) const { return ruVector3(x + v.x, y + v.y, z + v.z); }
	ruVector3 operator - (const ruVector3 & v) const { return ruVector3(x - v.x, y - v.y, z - v.z); }
	ruVector3 operator * (const ruVector3 & v) const { return ruVector3(x * v.x, y * v.y, z * v.z); }
	ruVector3 operator * (const float & f) const { return ruVector3(x * f, y * f, z * f); }
	ruVector3 operator / (const ruVector3 & v) const { return ruVector3(x / v.x, y / v.y, z / v.z); }
	ruVector3 operator / (const float & f) const { return ruVector3(x / f, y / f, z / f); }
	void operator *= (const ruVector3 & v) { x *= v.x; y *= v.y; z *= v.z; }
	float Angle(const ruVector3 & v) const { return acosf(Dot(v) / sqrtf(Length2() * v.Length2())); }
	ruVector3 Abs() const { return ruVector3(abs(x), abs(y), abs(z)); }
	void operator /= (const ruVector3 & v) { x /= v.x; y /= v.y; z /= v.z; }
	void operator /= (float a) { x /= a; y /= a; z /= a; }
	void operator += (const ruVector3 & v) { x += v.x; y += v.y; z += v.z; }
	void operator -= (const ruVector3 & v) { x -= v.x; y -= v.y; z -= v.z; }
	void operator = (const ruVector3 & v) { x = v.x; y = v.y; z = v.z; }
	bool operator == (const ruVector3 & v) {
		float dx = abs(x - v.x);
		float dy = abs(y - v.y);
		float dz = abs(z - v.z);
		return dx > 0.0001f && dy > 0.0001f && dz > 0.0001f;
	}
	float Length() const { return sqrt(x * x + y * y + z * z); }
	float Length2() const { return x * x + y * y + z * z; }
	ruVector3 Normalize() { float l = 1.0f / Length(); x *= l; y *= l; z *= l; return *this; }
	ruVector3 Normalized() const { float l = 1.0f / Length(); return ruVector3(x * l, y * l, z * l); }
	ruVector3 Cross(const ruVector3 & v) const { return ruVector3(y * v.z - z * v.x, z * v.x - x * v.z, x * v.y - y * v.x); }
	float Dot(const ruVector3 & v) const { return x * v.x + y * v.y + z * v.z; }
	ruVector3 Rotate(const ruVector3 & axis, float angle) {
		angle *= 3.14159f / 180.0f;
		ruVector3 o = axis * axis.Dot(*this);
		ruVector3 x = *this - o;
		ruVector3 y;
		y = axis.Cross(*this);
		return (o + x * cosf(angle) + y * sinf(angle));
	}
	ruVector3 Lerp(const ruVector3 & v, float t) const { return ruVector3(x + (v.x - x) * t, y + (v.y - y) * t, z + (v.z - z) * t); }
	ruVector3 Project(const ruVector3 & planeNormal) const { return (*this) - (planeNormal * (*this).Dot(planeNormal)) / planeNormal.Length2(); }
	float Distance(const ruVector3 & v) const { return (*this - v).Length(); }
};

static ruVector3 operator * (const float & f, const ruVector3 & v) {
	return ruVector3(v.x * f, v.y * f, v.z * f);
}

static ruVector3 operator - (const ruVector3 & v) {
	return ruVector3(-v.x, -v.y, -v.z);
}

static float Lerp(const float & from, const float & to, const float & t) {
	return from + (to - from) * t;
}

class ruVector2 {
public:
	float x, y;

	ruVector2() {
		x = 0;
		y = 0;
	};

	ruVector2(float _x, float _y) : x(_x), y(_y) {};

	void operator += (const ruVector2 & v) {
		x += v.x;
		y += v.y;
	}
};

class ruVector4 {
public:
	union {
		struct {
			float x, y, z, w;
		};
		float c[4];
	};

	ruVector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {};
	ruVector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {};
	ruVector4(const ruVector3 & vec3, float _w) : x(vec3.x), y(vec3.y), z(vec3.z), w(_w) {};
};

class ruQuaternion {
public:
	float x;
	float y;
	float z;
	float w;

	ruQuaternion();
	ruQuaternion(float x, float y, float z, float w);
	ruQuaternion(float pitch, float yaw, float roll);
	ruQuaternion(const ruVector3 & axis, float angle);

	static ruQuaternion LookAt(const ruVector3 & sourcePoint, const ruVector3 & destPoint) {
		ruVector3 forwardVector = (destPoint - sourcePoint).Normalize();
		float dot = ruVector3(0, 0, 1).Dot(forwardVector);
		if(abs(dot - (-1.0f)) < 0.000001f) {
			return ruQuaternion(0, 1, 0, 3.1415926535897932f);
		}
		if(abs(dot - (1.0f)) < 0.000001f) {
			return ruQuaternion();
		}
		float rotAngle = acos(dot) * 180.0f / 3.14159;
		ruVector3 rotAxis = ruVector3(0, 0, 1).Cross(forwardVector).Normalize();
		return ruQuaternion(rotAxis, rotAngle);
	}
};

static ruQuaternion operator *  (const ruQuaternion& q1, const ruQuaternion & q2) {
	return ruQuaternion(
		q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
		q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z,
		q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x,
		q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z);
}

class ruPlane {
public:
	ruVector3 normal;
	float distance;

	ruPlane() {}
	ruPlane(const ruVector3 & planeNormal, float planeDistance) : normal(planeNormal), distance(planeDistance) {}
	ruPlane(float a, float b, float c, float d) : normal(a, b, c), distance(d) {}
	float Dot(const ruVector3 & point) const {
		return normal.Dot(point) + distance;
	}
	ruPlane Normalize() {
		float d = normal.Length();
		normal /= d;
		distance /= d;
		return *this;
	}
};

class ruConfig {
private:
	string mFileName;
	map<string, string> mValues;
	map<string, string>::iterator GetExisting(const string & varName);
public:
	ruConfig();
	ruConfig(const string & filename);
	void LoadString(const string & str);
	void Load(const string & fileName);
	void Save(const string & fileName) const;
	void Save() const;
	bool IsEmpty() const;
	map<string, string> GetValuesCopy() const;
	string GetString(const string & varName);
	float GetNumber(const string & varName);
	bool GetBoolean(const string & varName);
	void SetNumber(const string & varName, float value);
	void SetNumber(const string & varName, int value);
	void SetBoolean(const string & varName, bool value);
	void SetString(const string & varName, const string & value);
	void AddNumber(const string & newVarName, float value);
	void AddNumber(const string & newVarName, int value);
	void AddString(const string & newVarName, const string & value);
	void AddBoolean(const string & varName, bool value);
};

#define BODY_MAX_CONTACTS ( 16 )

struct ruContact;

class ruEngine;
class ruSceneFactory;

// Animation
class ruAnimation {
public:
	enum class Direction {
		Forward,
		Reverse
	};
private:
	int mBeginFrame;
	int mEndFrame;
	int mCurrentFrame;
	int mNextFrame;
	float mInterpolator;
	float mDuration;
	bool mLooped;
	bool mEnabled;
	string mName;
	Direction mDirection;

	class AnimationEvent {
	public:
		bool mDone;
		ruEvent Event;
		AnimationEvent() : mDone(false) {}
	};

	// list of actions, which must be done on n-th frame 
	unordered_map<int, AnimationEvent> mFrameListenerList;
	void DoFramesActions();
public:
	ruAnimation();
	ruAnimation(int theBeginFrame, int theEndFrame, float theDuration, bool theLooped = false);
	virtual ~ruAnimation();
	void SetFrameInterval(int begin, int end);
	void SetCurrentFrame(int frame);
	int GetCurrentFrame() const;
	int GetEndFrame() const;
	int GetBeginFrame() const;
	int GetNextFrame() const;
	void SetName(const string & newName);
	void SetInterpolator(float interpolator);
	float GetInterpolator() const;
	string GetName() const;
	void SetDuration(float duration);
	float GetDuration() const;
	void SetEnabled(bool state);
	bool IsEnabled() const;
	void AddFrameListener(int frameNum, const ruDelegate & action);
	void RemoveFrameListeners(int frameNum);
	void Rewind();
	void Update(float dt = 1.0f / 60.0f);
	Direction GetDirection() const;
	void SetDirection(const Direction & direction);
};

enum class BodyType : int {
	None, Sphere, Cylinder, Box, Trimesh, Convex, Capsule
};

class ruTexture {
public:
	virtual ~ruTexture();
	static shared_ptr<ruTexture> Request(const string & file);
	virtual string GetName() = 0;
	virtual int GetWidth() = 0;
	virtual int GetHeight() = 0;
	virtual int GetColorDepth() = 0;
};

class ruCubeTexture {
protected:
	virtual ~ruCubeTexture();
public:
	static ruCubeTexture * Request(const string & file);
};

struct ruKeyFrame {
	ruVector3 mPosition;
	ruQuaternion mRotation;
};

class ruSceneNode {
public:
	virtual ~ruSceneNode();
	virtual ruSceneFactory * const GetFactory() const = 0;
	virtual void SetBlurAmount(float blurAmount) = 0;
	virtual float GetBlurAmount() = 0;
	virtual string GetProperty(string propName) = 0;
	virtual ruKeyFrame GetKeyFrame(int n) const = 0;
	virtual void Hide() = 0;
	virtual void Show() = 0;
	virtual bool IsVisible() = 0;
	virtual const string GetName() = 0;
	virtual void SetDepthHack(float order) = 0;
	virtual void Attach(const shared_ptr<ruSceneNode> & parent) = 0;
	virtual void Detach() = 0;
	virtual void SetDamping(float linearDamping, float angularDamping) = 0;
	virtual void SetPosition(ruVector3 position) = 0;
	virtual void SetRotation(ruQuaternion rotation) = 0;
	virtual void SetGravity(const ruVector3 & gravity) = 0;
	virtual ruVector3 GetLookVector() const = 0;
	virtual ruVector3 GetRightVector() const = 0;
	virtual ruVector3 GetUpVector() const = 0;
	virtual ruVector3 GetPosition() const = 0;
	virtual void SetMass(float mass) = 0;
	virtual ruQuaternion GetLocalRotation() = 0;
	virtual ruVector3 GetLocalPosition() = 0;
	virtual void SetLocalPosition(ruVector3 pos) = 0;
	virtual void SetLocalRotation(ruQuaternion rot) = 0;
	virtual void SetName(const string & name) = 0;
	virtual ruVector3 GetAABBMin() = 0;
	virtual ruVector3 GetTotalForce() = 0;
	virtual ruVector3 GetAABBMax() = 0;
	virtual bool IsInsideNode(shared_ptr<ruSceneNode> node) = 0;
	virtual shared_ptr<ruSceneNode> GetChild(int i) = 0;
	virtual int GetCountChildren() = 0;
	virtual bool IsInFrustum() = 0;
	virtual void SetAlbedo(float albedo) = 0;
	virtual ruVector3 GetLinearVelocity() = 0;
	virtual int GetContactCount() = 0;
	virtual ruContact GetContact(int num) = 0;
	virtual void Freeze() = 0;
	virtual shared_ptr<ruTexture> GetTexture(int n) = 0;
	virtual void Unfreeze() = 0;
	virtual void SetConvexBody() = 0;
	virtual void SetBoxBody() = 0;
	virtual void SetSphereBody() = 0;
	virtual void SetCylinderBody() = 0;
	virtual bool IsStatic() = 0;
	virtual bool IsDynamic() = 0;
	virtual void SetCapsuleBody(float height, float radius) = 0;
	virtual void SetAngularFactor(ruVector3 fact) = 0;
	virtual void SetTrimeshBody() = 0;
	virtual void Move(ruVector3 speed) = 0;
	virtual void SetFriction(float friction) = 0;
	virtual void SetLocalScale(ruVector3 scale) = 0;
	virtual void SetAnisotropicFriction(ruVector3 aniso) = 0;
	virtual float GetMass() = 0;
	virtual bool IsFrozen() = 0;
	virtual void SetLinearFactor(ruVector3 lin) = 0;
	virtual void SetVelocity(ruVector3 velocity) = 0;
	virtual void SetAngularVelocity(ruVector3 velocity) = 0;
	virtual void SetAnimation(ruAnimation * newAnim, bool dontAffectChilds = false) = 0;
	virtual int GetTotalAnimationFrameCount() = 0;
	virtual ruAnimation * GetCurrentAnimation() = 0;
	virtual ruVector3 GetEulerAngles() = 0;
	virtual ruVector3 GetAbsoluteLookVector() = 0;
	virtual BodyType GetBodyType() const = 0;
	virtual ruVector3 GetRotationAxis() = 0;
	virtual float GetRotationAngle() = 0;
	virtual void AddForce(ruVector3 force) = 0;
	virtual void AddForceAtPoint(ruVector3 force, ruVector3 point) = 0;
	virtual void AddTorque(ruVector3 torque) = 0;
	virtual shared_ptr<ruSceneNode> GetParent() = 0;
	virtual int GetTextureCount() = 0;
	virtual bool IsSkinned() const = 0;
	virtual string GetTag() const = 0;
	virtual void SetTag(const string & tag) = 0;
	virtual float GetDepthHack() const = 0;
	virtual float GetAlbedo() const = 0;
	virtual bool IsBone() const = 0;
	virtual int GetMeshCount() const = 0;
	virtual void SetShadowCastEnabled(bool state) = 0;
	virtual bool IsShadowCastEnabled() const = 0;
	virtual void SetCollisionEnabled(bool state) = 0;
	virtual bool IsCollisionEnabled() const = 0;
	virtual void SetTexCoordFlow(const ruVector2 & flow) = 0;
	virtual void SetVegetation(bool state) = 0;
	virtual bool IsVegetation() const = 0;
	virtual ruVector2 GetTexCoordFlow() const = 0;
	virtual void SetAnimationOverride(bool state) = 0;
	virtual bool IsAnimationOverride() const = 0;
	virtual void SetAnimationBlendingEnabled(bool state) = 0;
	virtual bool IsAnimationBlendingEnabled(bool state) const = 0;
	virtual void SetOpacity(float opacity) = 0;
	virtual shared_ptr<ruSceneNode> FindChild(const string & name) = 0;
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

struct PlaybackCallback {
	ruEvent PlayEvent;
	shared_ptr<class ruSound> Caller;
};

class ruSound : public std::enable_shared_from_this<ruSound> {
private:
	bool mIs3D;
public:
	int pfHandle;

	explicit ruSound();
	virtual ~ruSound();

	static PlaybackCallback PlayCallback;

	static shared_ptr<ruSound> Load2D(const string & file);
	static shared_ptr<ruSound> Load3D(const string & file);
	static shared_ptr<ruSound> LoadMusic(const string & file);
	static int GetCount();
	static shared_ptr<ruSound> GetSound(int n);
	static void SetAudioReverb(int reverb);
	static void SetMasterVolume(float volume);
	static float GetMasterVolume();
	void Attach(const shared_ptr<ruSceneNode> & node);
	void Play(int oneshot = 1);
	void Pause();
	void SetVolume(float vol);
	void SetPosition(ruVector3 pos);
	void SetReferenceDistance(float rd);
	void SetRolloffFactor(float rolloffDistance);
	void SetRoomRolloffFactor(float rolloffDistance);
	void SetMaxDistance(float maxDistance);
	int IsPlaying();
	void SetPitch(float pitch);
	bool IsPaused();
	float GetLength();
	void SetLoop(bool state);
	float GetPlaybackPosition();
	void Stop();
	bool Is3D() const;
	void SetPlaybackPosition(float timeSeconds);
	ruVector3 GetPosition() const;
};

enum class ruGUIAction : int {
	OnClick, OnMouseEnter, OnMouseLeave
};

enum class ruTextAlignment : int {
	Left,
	Center
};

class ruFont {
protected:
	virtual ~ruFont();
};

const float ruVirtualScreenWidth = 1024.0f;
const float ruVirtualScreenHeight = 768.0f;

class ruGUINode {
protected:
	virtual ~ruGUINode();
public:
	// Note: All size and position methods are operate on a virtual screen ruVirtualScreenWidth and ruVirtualScreenHeight
	virtual void Attach(const shared_ptr<ruGUINode> & parent) = 0;
	virtual void AddAction(ruGUIAction action, const ruDelegate & delegat) = 0;
	virtual void RemoveAction(ruGUIAction action) = 0;
	virtual void RemoveAllActions() = 0;
	virtual void SetPosition(float x, float y) = 0;
	virtual void SetSize(float w, float h) = 0;
	virtual void SetColor(ruVector3 color) = 0;
	virtual void SetAlpha(int alpha) = 0;
	virtual void SetVisible(bool visible) = 0;
	virtual bool IsVisible() const = 0;
	virtual ruVector2 GetPosition() = 0;
	virtual ruVector2 GetGlobalPosition() = 0;
	virtual ruVector2 GetSize() = 0;
	virtual ruVector3 GetColor() = 0;
	virtual void SetTexture(const shared_ptr<ruTexture> & pTexture) = 0;
	virtual int GetAlpha() = 0;
	virtual bool IsMouseInside() = 0;
	virtual shared_ptr<ruTexture> GetTexture() = 0;
	virtual weak_ptr<class ruGUIScene> GetScene() = 0;
	virtual void SetIndependentAlpha(bool useIndependent) = 0;
	virtual bool IsIndependentAlpha() const = 0;
	virtual void SetLayer(int layer) = 0;
	virtual int GetLayer() const = 0;
	virtual void Move(const ruVector2 & speed) = 0;
};

class ruRect : public virtual ruGUINode {
public:
	virtual ~ruRect() {	}
};

class ruText : public virtual ruGUINode {
public:
	virtual void SetText(const string & text) = 0;
};

class ruButton : public virtual ruRect {
public:
	virtual bool IsPressed() = 0;
	virtual bool IsHit() const = 0;
	virtual bool IsRightPressed() = 0;
	virtual bool IsRightHit() const = 0;
	virtual bool IsPicked() = 0;
	virtual shared_ptr<ruText> GetText() = 0;
	virtual void SetActive(bool state) = 0;
	virtual void SetPickedColor(ruVector3 val) = 0;
	virtual ruVector3 GetPickedColor() const = 0;
};

class ruGUIScene {
protected:
	ruGUIScene() {}
	virtual ~ruGUIScene() {}
public:
	virtual bool IsVisible() const = 0;
	virtual void SetVisible(bool visible) = 0;

	virtual void SetOpacity(float opacity) = 0;
	virtual float GetOpacity() const = 0;

	virtual ruEngine * const GetEngine() const = 0;

	virtual shared_ptr<ruGUINode> CreateNode() = 0;
	virtual shared_ptr<ruText> CreateText(const string & theText, float theX, float theY, float theWidth, float theHeight, const shared_ptr<ruFont> & theFont, ruVector3 theColor, ruTextAlignment theTextAlign, int theAlpha = 255) = 0;
	virtual shared_ptr<ruRect> CreateRect(float theX, float theY, float theWidth, float theHeight, const shared_ptr<ruTexture> & theTexture, ruVector3 = ruVector3(255, 255, 255), int theAlpha = 255) = 0;
	virtual shared_ptr<ruButton> CreateButton(int x, int y, int w, int h, const shared_ptr<ruTexture> & texture, const string & text, const shared_ptr<ruFont> & font, ruVector3 color, ruTextAlignment textAlign = ruTextAlignment::Center, int alpha = 255) = 0;
};

struct ruRayCastResultEx {
	bool valid;
	int index;
	shared_ptr<ruSceneNode> node;
	ruVector3 position;
	ruVector3 normal;
	string textureName;
};

class ruPhysics {
public:
	virtual ~ruPhysics() {}
	virtual shared_ptr<ruSceneNode> CastRay(ruVector3 begin, ruVector3 end, ruVector3 * outPickPoint) = 0;
	virtual ruRayCastResultEx CastRayEx(ruVector3 begin, ruVector3 end) = 0;
	virtual void Update(float timeStep, int subSteps, float fixedTimeStep) = 0;
	virtual shared_ptr<ruSceneNode> RayPick(int x, int y, ruVector3 * outPickPoint) = 0;
};

// Volumetric fog with lighting
class ruFog : public virtual ruSceneNode {
public:
	virtual ~ruFog() {}

	virtual void SetSize(const ruVector3 & min, const ruVector3 & max) = 0;
	virtual ruVector3 GetMin() const = 0;
	virtual ruVector3 GetMax() const = 0;

	virtual void SetSpeed(const ruVector3 & speed) = 0;
	virtual ruVector3 GetSpeed() const = 0;

	virtual void SetColor(const ruVector3 & color) = 0;
	virtual ruVector3 GetColor() const = 0;

	virtual void SetDensity(float density) = 0;
	virtual float GetDensity() const = 0;
};

class ruLight : public virtual ruSceneNode {
protected:
	virtual ~ruLight();
public:
	virtual void SetRange(float range) = 0;
	virtual float GetRange() const = 0;

	virtual void SetColor(const ruVector3 & clr) = 0;
	virtual ruVector3 GetColor() const = 0;

	virtual bool IsSeePoint(const ruVector3 & point) = 0;

	virtual void SetDrawFlare(bool state) = 0;
	virtual bool IsDrawFlare() const = 0;
};

class ruSpotLight : public virtual ruLight {
public:
	virtual ~ruSpotLight() {}

	static void SetSpotDefaultTexture(shared_ptr<ruTexture> defaultSpotTexture);

	virtual void SetSpotTexture(shared_ptr<ruTexture> texture) = 0;
	virtual void SetConeAngles(float innerAngle, float outerAngle) = 0;
	virtual bool IsSeePoint(const ruVector3 & point) = 0;
};

class ruDirectionalLight : public virtual ruLight {
public:
	virtual ~ruDirectionalLight() {}

	virtual bool IsSeePoint(const ruVector3 & point) = 0;
};

class ruPointLight : public virtual ruLight {
public:
	virtual ~ruPointLight() {}
	static void SetPointDefaultTexture(ruCubeTexture * defaultPointTexture);

	virtual void SetPointTexture(ruCubeTexture * cubeTexture) = 0;
	virtual bool IsSeePoint(const ruVector3 & point) = 0;
};

class ruCamera : public virtual ruSceneNode {
public:
	virtual ~ruCamera() {}

	virtual void SetActive() = 0;
	virtual void SetSkybox(const shared_ptr<ruTexture> & up, const shared_ptr<ruTexture> & left, const shared_ptr<ruTexture> & right, const shared_ptr<ruTexture> & forward, const shared_ptr<ruTexture> & back) = 0;
	virtual void SetFOV(float fov) = 0;
	virtual void SetFrameBrightness(float brightness) = 0; // in percent - 100% means fullbright. Range is [0; 100]
	virtual float GetFrameBrightness() const = 0;
	virtual void SetFrameColor(const ruVector3 & color) = 0; // in RGB. Range is [0; 255]
	virtual ruVector3 GetFrameColor() const = 0;
};

class ruTimer {
public:
	virtual ~ruTimer() {}
	static shared_ptr<ruTimer> Create();
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
	virtual ~ruParticleSystem() {}

	enum class Type {
		Box, Stream
	};

	virtual void SetType(ruParticleSystem::Type type) = 0;
	virtual ruParticleSystem::Type GetType() = 0;

	virtual int GetAliveParticles() = 0;
	virtual void ResurrectParticles() = 0;

	virtual bool IsEnabled() = 0;
	virtual void SetEnabled(bool state) = 0;

	virtual void SetPointSize(float size) = 0;
	virtual float GetPointSize() = 0;

	virtual void SetScaleFactor(float scaleFactor) = 0;
	virtual float GetScaleFactor() = 0;

	virtual void SetParticleThickness(float thickness) = 0;
	virtual float GetParticleThickness() = 0;

	virtual void SetAutoResurrection(bool state) = 0;
	virtual bool IsAutoResurrectionEnabled() = 0;

	virtual void SetLightingEnabled(bool state) = 0;
	virtual bool IsLightingEnabled() = 0;

	virtual void SetBoundingBox(const ruVector3 & bbMin, const ruVector3 & bbMax) = 0;
	virtual ruVector3 GetBoundingBoxMax() = 0;
	virtual ruVector3 GetBoundingBoxMin() = 0;

	virtual void SetSpeedDeviation(const ruVector3 & dMin, const ruVector3 & dMax) = 0;
	virtual ruVector3 GetSpeedDeviationMax() = 0;
	virtual ruVector3 GetSpeedDeviationMin() = 0;

	virtual void SetTexture(const shared_ptr<ruTexture> & texture) = 0;
	virtual shared_ptr<ruTexture> GetTexture() = 0;

	virtual void SetBoundingRadius(float radius) = 0;
	virtual float GetBoundingRadius() = 0;

	virtual void SetColorRange(const ruVector3 & cMin, const ruVector3 & cMax) = 0;
	virtual ruVector3 GetColorMin() = 0;
	virtual ruVector3 GetColorMax() = 0;

	virtual float GetAlphaOffset() = 0;
	virtual void SetAlphaOffset(float alphaOffset) = 0;
};


class ruVideomode {
public:
	int mWidth;
	int mHeight;
	int mRefreshRate;

	ruVideomode(int width, int height, int refreshRate) : mWidth(width), mHeight(height), mRefreshRate(refreshRate) {

	}
};

class ruRenderer {
public:
	virtual ~ruRenderer() {}
	virtual void RenderWorld() = 0;
	virtual float GetResolutionWidth() const = 0;
	virtual float GetResolutionHeight() const = 0;
	virtual void SetCursorVisible(bool state) = 0;
	virtual void SetCursor(shared_ptr<ruTexture> texture, int w, int h) = 0;
	virtual int GetDIPs() const = 0;
	virtual int GetTextureUsedPerFrame() const = 0;
	virtual int GetShaderUsedPerFrame() const = 0;
	virtual int GetRenderedTriangles() const = 0;
	virtual void SetAmbientColor(ruVector3 color) = 0;
	virtual ruVector3 GetAmbientColor() const = 0;
	virtual int GetAvailableTextureMemory() = 0;
	virtual void UpdateWorld() = 0;
	virtual void SetFXAAEnabled(bool state) = 0;
	virtual bool IsFXAAEnabled() = 0;
	virtual void SetHDREnabled(bool state) = 0;
	virtual bool IsHDREnabled() = 0;
	virtual void SetParallaxEnabled(bool state) = 0;
	virtual bool IsParallaxEnabled() = 0;
	virtual void SetSpotLightShadowMapSize(int size) = 0;
	virtual void SetSpotLightShadowsEnabled(bool state) = 0;
	virtual bool IsSpotLightShadowsEnabled() = 0;
	virtual void SetPointLightShadowMapSize(int size) = 0;
	virtual void SetPointLightShadowsEnabled(bool state) = 0;
	virtual bool IsPointLightShadowsEnabled() = 0;
	virtual int GetMaxIsotropyDegree() const = 0;
	virtual void SetIsotropyDegree(int degree) = 0;
	virtual void SetAnisotropicTextureFiltration(bool state) = 0;
	virtual bool IsAnisotropicTextureFiltrationEnabled() const = 0;
	virtual void SetVolumetricFogEnabled(bool state) = 0;
	virtual bool IsVolumetricFogEnabled() const = 0;
	virtual void SetDirectionalLightShadowMapSize(int size) = 0;
	virtual int GetDirectionalLightShadowMapSize() const = 0;
	virtual void SetDirectionalLightDynamicShadows(bool state) = 0;
	virtual bool IsDirectionalLightDynamicShadowsEnabled() = 0;
	virtual void SetBloomEnabled(bool state) = 0;
	virtual bool IsBloomEnabled() const = 0;
	virtual void SetSoftShadowsEnabled(bool state) = 0;
	virtual bool IsSoftShadowsEnabled() const = 0;
	virtual vector<ruVideomode> GetVideoModeList() = 0;
	virtual void LoadColorGradingMap(const char * fileName) = 0;
	virtual float GetGUIWidthScaleFactor() const = 0;
	virtual float GetGUIHeightScaleFactor() const = 0;
	virtual bool IsRunning() const = 0;
	virtual void Shutdown() = 0;
};

class ruSceneFactory {
public:
	virtual ~ruSceneFactory() {}

	virtual shared_ptr<ruSceneNode> CreateSceneNode() = 0;
	virtual shared_ptr<ruSceneNode> CreateSceneNodeDuplicate(shared_ptr<ruSceneNode> src) = 0;
	virtual shared_ptr<ruPointLight> CreatePointLight() = 0;
	virtual shared_ptr<ruSpotLight> CreateSpotLight() = 0;
	virtual shared_ptr<ruDirectionalLight> CreateDirectionalLight() = 0;
	virtual shared_ptr<ruCamera> CreateCamera(float fov) = 0;
	virtual shared_ptr<ruParticleSystem> CreateParticleSystem(int particleCount) = 0;
	virtual shared_ptr<ruFog> CreateFog(const ruVector3 & min, const ruVector3 & max, const ruVector3 & color, float density) = 0;

	virtual shared_ptr<ruSceneNode> FindByName(const string & name) = 0;
	virtual shared_ptr<ruSceneNode> LoadScene(const string & file) = 0;
	virtual int GetNodeCount() = 0;
	virtual shared_ptr<ruSceneNode> GetNode(int i) = 0;
	virtual vector<shared_ptr<ruSceneNode>> GetTaggedObjects(const string & tag) = 0;

	virtual int GetSpotLightCount() = 0;
	virtual shared_ptr<ruSpotLight> GetSpotLight(int n) = 0;

	virtual int GetPointLightCount() = 0;
	virtual shared_ptr<ruPointLight> GetPointLight(int n) = 0;

	virtual int GetDirectionalLightCount() = 0;
	virtual shared_ptr<ruDirectionalLight> GetDirectionalLight(int n) = 0;
};




class ruInput {
public:
	enum class Key : int {
		None = 0,
		Esc = 1,
		Num1,
		Num2,
		Num3,
		Num4,
		Num5,
		Num6,
		Num7,
		Num8,
		Num9,
		Num0,
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
		Subtract,
		Numpad4,
		Numpad5,
		Numpad6,
		Add,
		Numpad1,
		Numpad2,
		Numpad3,
		Numpad0,
		Decimal,
		OEM_102,
		F11,
		F12,
		F13,
		F14,
		F15,
		Kana,
		ABNT_C1,
		Convert,
		NoConvert,
		Yen,
		ABNT_C2,
		NumpadEquals,
		PrevTrack,
		AT,
		Colon,
		Underline,
		Kanji,
		Stop,
		AX,
		Ulabeled,
		NextTrack,
		NumpadEnter,
		RControl,
		Mute,
		Calculator,
		PlayPause,
		MediaStop,
		VolumeDown,
		VolumeUp,
		WebHome,
		NumpadComma,
		Divide,
		SysRQ,
		RMenu,
		Pause,
		Home,
		Up,
		Prior,
		Left,
		Right,
		End,
		Down,
		Next,
		Insert,
		Del,
		LWin,
		RWin,
		Apps,
		Power,
		Sleep,
		Wake,
		WebSearch,
		WebFavorites,
		WebRefresh,
		WebStop,
		WebForward,
		WebBack,
		MyComputer,
		Mail,
		MediaSelect,
		Count,
	};

	enum class MouseButton : int {
		Left,
		Right,
		Middle,
	};
	virtual ~ruInput() {}
	virtual string GetKeyName(ruInput::Key key) = 0;
	virtual void Update() = 0;
	virtual bool IsKeyDown(Key key) = 0;
	virtual bool IsKeyHit(Key key) = 0;
	virtual bool IsKeyUp(Key key) = 0;
	virtual bool IsMouseDown(MouseButton button) = 0;
	virtual bool IsMouseHit(MouseButton button) = 0;
	virtual int GetMouseX() = 0;
	virtual int GetMouseY() = 0;
	virtual int GetMouseWheel() = 0;
	virtual int GetMouseXSpeed() = 0;
	virtual int GetMouseYSpeed() = 0;
	virtual int GetMouseWheelSpeed() = 0;
};

class ruEngine {
public:
	static unique_ptr<ruEngine> Create(int width, int height, int fullscreen, char vSync);

public:
	virtual shared_ptr<ruFont> CreateBitmapFont(int size, const string & filename) = 0;
	virtual ruRenderer * const GetRenderer() const = 0;
	virtual ruSceneFactory * const GetSceneFactory() const = 0;
	virtual shared_ptr<ruGUIScene> CreateGUIScene() = 0;
	virtual ruPhysics * const GetPhysics() const = 0;
	virtual ruInput * const GetInput() const = 0;

	ruEngine() {}
	virtual ~ruEngine() {}
};