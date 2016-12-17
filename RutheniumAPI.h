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

typedef function<void()> Delegate;

class Event {
private:
	vector<Delegate> Actions;
public:
	void operator()() {
		for(auto & f : Actions) f();
	}

	Event & operator += (const Delegate & fn) {
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

class Vector3 {
public:
	union {
		struct { float x, y, z; };
		float elements[3];
	};

	Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
	Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
	Vector3(const Vector3 & v) : x(v.x), y(v.y), z(v.z) {}
	Vector3(float * v) : x(v[0]), y(v[1]), z(v[2]) {}
	Vector3 operator + (const Vector3 & v) const { return Vector3(x + v.x, y + v.y, z + v.z); }
	Vector3 operator - (const Vector3 & v) const { return Vector3(x - v.x, y - v.y, z - v.z); }
	Vector3 operator * (const Vector3 & v) const { return Vector3(x * v.x, y * v.y, z * v.z); }
	Vector3 operator * (const float & f) const { return Vector3(x * f, y * f, z * f); }
	Vector3 operator / (const Vector3 & v) const { return Vector3(x / v.x, y / v.y, z / v.z); }
	Vector3 operator / (const float & f) const { return Vector3(x / f, y / f, z / f); }
	void operator *= (const Vector3 & v) { x *= v.x; y *= v.y; z *= v.z; }
	float Angle(const Vector3 & v) const { return acosf(Dot(v) / sqrtf(Length2() * v.Length2())); }
	Vector3 Abs() const { return Vector3(abs(x), abs(y), abs(z)); }
	void operator /= (const Vector3 & v) { x /= v.x; y /= v.y; z /= v.z; }
	void operator /= (float a) { x /= a; y /= a; z /= a; }
	void operator += (const Vector3 & v) { x += v.x; y += v.y; z += v.z; }
	void operator -= (const Vector3 & v) { x -= v.x; y -= v.y; z -= v.z; }
	void operator = (const Vector3 & v) { x = v.x; y = v.y; z = v.z; }
	bool operator == (const Vector3 & v) {
		float dx = abs(x - v.x);
		float dy = abs(y - v.y);
		float dz = abs(z - v.z);
		return dx > 0.0001f && dy > 0.0001f && dz > 0.0001f;
	}
	float Length() const { return sqrt(x * x + y * y + z * z); }
	float Length2() const { return x * x + y * y + z * z; }
	Vector3 Normalize() { float l = 1.0f / Length(); x *= l; y *= l; z *= l; return *this; }
	Vector3 Normalized() const { float l = 1.0f / Length(); return Vector3(x * l, y * l, z * l); }
	Vector3 Cross(const Vector3 & v) const { return Vector3(y * v.z - z * v.x, z * v.x - x * v.z, x * v.y - y * v.x); }
	float Dot(const Vector3 & v) const { return x * v.x + y * v.y + z * v.z; }
	Vector3 Rotate(const Vector3 & axis, float angle) {
		angle *= 3.14159f / 180.0f;
		Vector3 o = axis * axis.Dot(*this);
		Vector3 x = *this - o;
		Vector3 y = axis.Cross(*this);
		return (o + x * cosf(angle) + y * sinf(angle));
	}
	Vector3 Lerp(const Vector3 & v, float t) const { return Vector3(x + (v.x - x) * t, y + (v.y - y) * t, z + (v.z - z) * t); }
	Vector3 Project(const Vector3 & planeNormal) const { return (*this) - (planeNormal * (*this).Dot(planeNormal)) / planeNormal.Length2(); }
	float Distance(const Vector3 & v) const { return (*this - v).Length(); }
};
static Vector3 operator * (const float & f, const Vector3 & v) { return Vector3(v.x * f, v.y * f, v.z * f); }
static Vector3 operator - (const Vector3 & v) { return Vector3(-v.x, -v.y, -v.z); }
static float Lerp(const float & from, const float & to, const float & t) { return from + (to - from) * t; }

class Vector2 {
public:
	float x, y;
	Vector2() : x(0), y(0) { };
	Vector2(float x, float y) : x(x), y(y) {};
	void operator += (const Vector2 & v) { x += v.x; y += v.y; }
	void operator -= (const Vector2 & v) { x -= v.x; y -= v.y; }
	void operator *= (const Vector2 & v) { x *= v.x; y *= v.y; }
	void operator /= (const Vector2 & v) { x /= v.x; y /= v.y; }
	void operator *= (float a) { x *= a; y *= a; }
	Vector2 operator - (const Vector2 & v) { return Vector2(x - v.x, y - v.y); }
	Vector2 operator + (const Vector2 & v) { return Vector2(x + v.x, y + v.y); }
	Vector2 operator * (const Vector2 & v) { return Vector2(x * v.x, y * v.y); }
	Vector2 operator / (const Vector2 & v) { return Vector2(x / v.x, y / v.y); }
};

class Vector4 {
public:
	union {
		struct {
			float x, y, z, w;
		};
		float c[4];
	};

	Vector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {};
	Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {};
	Vector4(const Vector3 & vec3, float _w) : x(vec3.x), y(vec3.y), z(vec3.z), w(_w) {};
};

class Quaternion {
public:
	float x;
	float y;
	float z;
	float w;

	Quaternion();
	Quaternion(float x, float y, float z, float w);
	Quaternion(float pitch, float yaw, float roll);
	Quaternion(const Vector3 & axis, float angle);

	static Quaternion LookAt(const Vector3 & sourcePoint, const Vector3 & destPoint) {
		Vector3 forwardVector = (destPoint - sourcePoint).Normalize();
		float dot = Vector3(0, 0, 1).Dot(forwardVector);
		if(abs(dot - (-1.0f)) < 0.000001f) {
			return Quaternion(0, 1, 0, 3.1415926535897932f);
		}
		if(abs(dot - (1.0f)) < 0.000001f) {
			return Quaternion();
		}
		float rotAngle = acos(dot) * 180.0f / 3.14159;
		Vector3 rotAxis = Vector3(0, 0, 1).Cross(forwardVector).Normalize();
		return Quaternion(rotAxis, rotAngle);
	}
};

static Quaternion operator *  (const Quaternion& q1, const Quaternion & q2) {
	return Quaternion(
		q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
		q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z,
		q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x,
		q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z);
}

class Plane {
public:
	Vector3 normal;
	float distance;

	Plane() {}
	Plane(const Vector3 & planeNormal, float planeDistance) : normal(planeNormal), distance(planeDistance) {}
	Plane(float a, float b, float c, float d) : normal(a, b, c), distance(d) {}
	float Dot(const Vector3 & point) const {
		return normal.Dot(point) + distance;
	}
	Plane Normalize() {
		float d = normal.Length();
		normal /= d;
		distance /= d;
		return *this;
	}
};

class Config {
private:
	string mFileName;
	map<string, string> mValues;
	map<string, string>::iterator GetExisting(const string & varName);
public:
	Config();
	Config(const string & filename);
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

struct Contact;

class IEngine;
class ISceneFactory;

// Animation
class Animation {
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
		Event Event;
		AnimationEvent() : mDone(false) {}
	};

	// list of actions, which must be done on n-th frame 
	unordered_map<int, AnimationEvent> mFrameListenerList;
	void DoFramesActions();
public:
	Animation();
	Animation(int theBeginFrame, int theEndFrame, float theDuration, bool theLooped = false);
	virtual ~Animation();
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
	void AddFrameListener(int frameNum, const Delegate & action);
	void RemoveFrameListeners(int frameNum);
	void Rewind();
	void Update(float dt = 1.0f / 60.0f);
	Direction GetDirection() const;
	void SetDirection(const Direction & direction);
};

enum class BodyType : int {
	None, Sphere, Cylinder, Box, Trimesh, Convex, Capsule
};

class ITexture {
public:
	virtual ~ITexture();
	virtual string GetName() const = 0;
	virtual int GetWidth() const = 0;
	virtual int GetHeight() const = 0;
	virtual int GetColorDepth() const = 0;
};

class ICubeTexture {
public:
	virtual ~ICubeTexture();
};

struct KeyFrame {
	Vector3 mPosition;
	Quaternion mRotation;
};

class ISceneNode {
public:
	virtual ~ISceneNode();
	virtual ISceneFactory * const GetFactory() const = 0;
	virtual void SetBlurAmount(float blurAmount) = 0;
	virtual float GetBlurAmount() = 0;
	virtual string GetProperty(string propName) = 0;
	virtual KeyFrame GetKeyFrame(int n) const = 0;
	virtual void Hide() = 0;
	virtual void Show() = 0;
	virtual bool IsVisible() = 0;
	virtual const string GetName() = 0;
	virtual void SetDepthHack(float order) = 0;
	virtual void Attach(const shared_ptr<ISceneNode> & parent) = 0;
	virtual void Detach() = 0;
	virtual void SetDamping(float linearDamping, float angularDamping) = 0;
	virtual void SetPosition(Vector3 position) = 0;
	virtual void SetRotation(Quaternion rotation) = 0;
	virtual void SetGravity(const Vector3 & gravity) = 0;
	virtual Vector3 GetLookVector() const = 0;
	virtual Vector3 GetRightVector() const = 0;
	virtual Vector3 GetUpVector() const = 0;
	virtual Vector3 GetPosition() const = 0;
	virtual void SetMass(float mass) = 0;
	virtual Quaternion GetLocalRotation() = 0;
	virtual Vector3 GetLocalPosition() = 0;
	virtual void SetLocalPosition(Vector3 pos) = 0;
	virtual void SetLocalRotation(Quaternion rot) = 0;
	virtual void SetName(const string & name) = 0;
	virtual Vector3 GetAABBMin() = 0;
	virtual Vector3 GetTotalForce() = 0;
	virtual Vector3 GetAABBMax() = 0;
	virtual bool IsInsideNode(shared_ptr<ISceneNode> node) = 0;
	virtual shared_ptr<ISceneNode> GetChild(int i) = 0;
	virtual int GetCountChildren() = 0;
	virtual bool IsInFrustum() = 0;
	virtual void SetAlbedo(float albedo) = 0;
	virtual Vector3 GetLinearVelocity() = 0;
	virtual int GetContactCount() = 0;
	virtual Contact GetContact(int num) = 0;
	virtual void Freeze() = 0;
	virtual shared_ptr<ITexture> GetTexture(int n) = 0;
	virtual void Unfreeze() = 0;
	virtual void SetConvexBody() = 0;
	virtual void SetBoxBody() = 0;
	virtual void SetSphereBody() = 0;
	virtual void SetCylinderBody() = 0;
	virtual bool IsStatic() = 0;
	virtual bool IsDynamic() = 0;
	virtual void SetCapsuleBody(float height, float radius) = 0;
	virtual void SetAngularFactor(Vector3 fact) = 0;
	virtual void SetTrimeshBody() = 0;
	virtual void Move(Vector3 speed) = 0;
	virtual void SetFriction(float friction) = 0;
	virtual void SetLocalScale(Vector3 scale) = 0;
	virtual void SetAnisotropicFriction(Vector3 aniso) = 0;
	virtual float GetMass() = 0;
	virtual bool IsFrozen() = 0;
	virtual void SetLinearFactor(Vector3 lin) = 0;
	virtual void SetVelocity(Vector3 velocity) = 0;
	virtual void SetAngularVelocity(Vector3 velocity) = 0;
	virtual void SetAnimation(Animation * newAnim, bool dontAffectChilds = false) = 0;
	virtual int GetTotalAnimationFrameCount() = 0;
	virtual Animation * GetCurrentAnimation() = 0;
	virtual Vector3 GetEulerAngles() = 0;
	virtual Vector3 GetAbsoluteLookVector() = 0;
	virtual BodyType GetBodyType() const = 0;
	virtual Vector3 GetRotationAxis() = 0;
	virtual float GetRotationAngle() = 0;
	virtual void AddForce(Vector3 force) = 0;
	virtual void AddForceAtPoint(Vector3 force, Vector3 point) = 0;
	virtual void AddTorque(Vector3 torque) = 0;
	virtual shared_ptr<ISceneNode> GetParent() = 0;
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
	virtual void SetTexCoordFlow(const Vector2 & flow) = 0;
	virtual void SetVegetation(bool state) = 0;
	virtual bool IsVegetation() const = 0;
	virtual Vector2 GetTexCoordFlow() const = 0;
	virtual void SetAnimationOverride(bool state) = 0;
	virtual bool IsAnimationOverride() const = 0;
	virtual void SetAnimationBlendingEnabled(bool state) = 0;
	virtual bool IsAnimationBlendingEnabled(bool state) const = 0;
	virtual void SetOpacity(float opacity) = 0;
	virtual shared_ptr<ISceneNode> FindChild(const string & name) = 0;
};

struct Contact {
	Vector3 normal;
	Vector3 position;
	float impulse;
	ISceneNode * body;
	string textureName;
	Contact() {
		impulse = 0;
	}
};

struct PlaybackCallback {
	Event PlayEvent;
	shared_ptr<class ISound> Caller;
};



enum class GUIAction : int {
	OnClick, OnMouseEnter, OnMouseLeave
};

enum class TextAlignment : int {
	Left,
	Center
};

class IFont {
protected:
	virtual ~IFont();
};

const float ruVirtualScreenWidth = 1024.0f;
const float ruVirtualScreenHeight = 768.0f;

class IGUINode {
protected:
	virtual ~IGUINode();
public:
	// Note: All size and position methods are operate on a virtual screen ruVirtualScreenWidth and ruVirtualScreenHeight
	virtual void Attach(const shared_ptr<IGUINode> & parent) = 0;
	virtual void AddAction(GUIAction action, const Delegate & delegat) = 0;
	virtual void RemoveAction(GUIAction action) = 0;
	virtual void RemoveAllActions() = 0;
	virtual void SetPosition(float x, float y) = 0;
	virtual void SetSize(float w, float h) = 0;
	virtual void SetColor(Vector3 color) = 0;
	virtual void SetAlpha(int alpha) = 0;
	virtual void SetVisible(bool visible) = 0;
	virtual bool IsVisible() const = 0;
	virtual Vector2 GetPosition() = 0;
	virtual Vector2 GetGlobalPosition() = 0;
	virtual Vector2 GetSize() = 0;
	virtual Vector3 GetColor() = 0;
	virtual void SetTexture(const shared_ptr<ITexture> & pTexture) = 0;
	virtual int GetAlpha() = 0;
	virtual bool IsMouseInside() = 0;
	virtual shared_ptr<ITexture> GetTexture() = 0;
	virtual weak_ptr<class IGUIScene> GetScene() = 0;
	virtual void SetIndependentAlpha(bool useIndependent) = 0;
	virtual bool IsIndependentAlpha() const = 0;
	virtual void SetLayer(int layer) = 0;
	virtual int GetLayer() const = 0;
	virtual void Move(const Vector2 & speed) = 0;
};

class IRect : public virtual IGUINode {
public:
	virtual ~IRect() {	}
};

class IText : public virtual IGUINode {
public:
	virtual void SetText(const string & text) = 0;
};

class IButton : public virtual IRect {
public:
	virtual bool IsPressed() = 0;
	virtual bool IsHit() const = 0;
	virtual bool IsRightPressed() = 0;
	virtual bool IsRightHit() const = 0;
	virtual bool IsPicked() = 0;
	virtual shared_ptr<IText> GetText() = 0;
	virtual void SetActive(bool state) = 0;
	virtual void SetPickedColor(Vector3 val) = 0;
	virtual Vector3 GetPickedColor() const = 0;
};

class IGUIScene {
protected:
	IGUIScene() {}
	virtual ~IGUIScene() {}
public:
	virtual bool IsVisible() const = 0;
	virtual void SetVisible(bool visible) = 0;

	virtual void SetOpacity(float opacity) = 0;
	virtual float GetOpacity() const = 0;

	virtual IEngine * const GetEngine() const = 0;

	virtual shared_ptr<IGUINode> CreateNode() = 0;
	virtual shared_ptr<IText> CreateText(const string & theText, float theX, float theY, float theWidth, float theHeight, const shared_ptr<IFont> & theFont, Vector3 theColor, TextAlignment theTextAlign, int theAlpha = 255) = 0;
	virtual shared_ptr<IRect> CreateRect(float theX, float theY, float theWidth, float theHeight, const shared_ptr<ITexture> & theTexture, Vector3 = Vector3(255, 255, 255), int theAlpha = 255) = 0;
	virtual shared_ptr<IButton> CreateButton(int x, int y, int w, int h, const shared_ptr<ITexture> & texture, const string & text, const shared_ptr<IFont> & font, Vector3 color, TextAlignment textAlign = TextAlignment::Center, int alpha = 255) = 0;
};

struct RayCastResultEx {
	bool valid;
	int index;
	shared_ptr<ISceneNode> node;
	Vector3 position;
	Vector3 normal;
	string textureName;
};

class IPhysics {
public:
	virtual ~IPhysics() {}
	virtual shared_ptr<ISceneNode> CastRay(Vector3 begin, Vector3 end, Vector3 * outPickPoint) = 0;
	virtual RayCastResultEx CastRayEx(Vector3 begin, Vector3 end) = 0;
	virtual void Update(float timeStep, int subSteps, float fixedTimeStep) = 0;
	virtual shared_ptr<ISceneNode> RayPick(int x, int y, Vector3 * outPickPoint) = 0;
};

// Volumetric fog with lighting
class IFog : public virtual ISceneNode {
public:
	virtual ~IFog() {}

	virtual void SetSize(const Vector3 & min, const Vector3 & max) = 0;
	virtual Vector3 GetMin() const = 0;
	virtual Vector3 GetMax() const = 0;

	virtual void SetSpeed(const Vector3 & speed) = 0;
	virtual Vector3 GetSpeed() const = 0;

	virtual void SetColor(const Vector3 & color) = 0;
	virtual Vector3 GetColor() const = 0;

	virtual void SetDensity(float density) = 0;
	virtual float GetDensity() const = 0;
};

class ILight : public virtual ISceneNode {
protected:
	virtual ~ILight();
public:
	virtual void SetRange(float range) = 0;
	virtual float GetRange() const = 0;

	virtual void SetColor(const Vector3 & clr) = 0;
	virtual Vector3 GetColor() const = 0;

	virtual bool IsSeePoint(const Vector3 & point) = 0;

	virtual void SetDrawFlare(bool state) = 0;
	virtual bool IsDrawFlare() const = 0;
};

class ISpotLight : public virtual ILight {
public:
	virtual ~ISpotLight() {}



	virtual void SetSpotTexture(shared_ptr<ITexture> texture) = 0;
	virtual void SetConeAngles(float innerAngle, float outerAngle) = 0;
	virtual bool IsSeePoint(const Vector3 & point) = 0;
};

class IDirectionalLight : public virtual ILight {
public:
	virtual ~IDirectionalLight() {}

	virtual bool IsSeePoint(const Vector3 & point) = 0;
};

class IPointLight : public virtual ILight {
public:
	virtual ~IPointLight() {}

	virtual void SetPointTexture(const shared_ptr<ICubeTexture> & cubeTexture) = 0;
	virtual bool IsSeePoint(const Vector3 & point) = 0;
};

class ICamera : public virtual ISceneNode {
public:
	virtual ~ICamera() {}

	virtual void SetActive() = 0;
	virtual void SetSkybox(const shared_ptr<ITexture> & up, const shared_ptr<ITexture> & left, const shared_ptr<ITexture> & right, const shared_ptr<ITexture> & forward, const shared_ptr<ITexture> & back) = 0;
	virtual void SetFOV(float fov) = 0;
	virtual void SetFrameBrightness(float brightness) = 0; // in percent - 100% means fullbright. Range is [0; 100]
	virtual float GetFrameBrightness() const = 0;
	virtual void SetFrameColor(const Vector3 & color) = 0; // in RGB. Range is [0; 255]
	virtual Vector3 GetFrameColor() const = 0;
};

class ITimer {
public:
	virtual ~ITimer() {}
	static shared_ptr<ITimer> Create();
	virtual void Restart() = 0;
	virtual double GetTimeInSeconds() = 0;
	virtual double GetTimeInMilliSeconds() = 0;
	virtual double GetTimeInMicroSeconds() = 0;
	virtual double GetElapsedTimeInSeconds() = 0;
	virtual double GetElapsedTimeInMilliSeconds() = 0;
	virtual double GetElapsedTimeInMicroSeconds() = 0;
};

class IParticleSystem : public virtual ISceneNode {
public:
	virtual ~IParticleSystem() {}

	enum class Type {
		Box, Stream
	};

	virtual void SetType(IParticleSystem::Type type) = 0;
	virtual IParticleSystem::Type GetType() = 0;

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

	virtual void SetBoundingBox(const Vector3 & bbMin, const Vector3 & bbMax) = 0;
	virtual Vector3 GetBoundingBoxMax() = 0;
	virtual Vector3 GetBoundingBoxMin() = 0;

	virtual void SetSpeedDeviation(const Vector3 & dMin, const Vector3 & dMax) = 0;
	virtual Vector3 GetSpeedDeviationMax() = 0;
	virtual Vector3 GetSpeedDeviationMin() = 0;

	virtual void SetTexture(const shared_ptr<ITexture> & texture) = 0;
	virtual shared_ptr<ITexture> GetTexture() = 0;

	virtual void SetBoundingRadius(float radius) = 0;
	virtual float GetBoundingRadius() = 0;

	virtual void SetColorRange(const Vector3 & cMin, const Vector3 & cMax) = 0;
	virtual Vector3 GetColorMin() = 0;
	virtual Vector3 GetColorMax() = 0;

	virtual float GetAlphaOffset() = 0;
	virtual void SetAlphaOffset(float alphaOffset) = 0;
};


class Videomode {
public:
	int mWidth;
	int mHeight;
	int mRefreshRate;

	Videomode(int width, int height, int refreshRate) : mWidth(width), mHeight(height), mRefreshRate(refreshRate) {

	}
};

class IRenderer {
public:
	virtual ~IRenderer() {}
	virtual shared_ptr<ITexture> GetTexture(const string & filename) = 0;
	virtual shared_ptr<ICubeTexture> GetCubeTexture(const string & filename) = 0;
	virtual void RenderWorld() = 0;
	virtual float GetResolutionWidth() const = 0;
	virtual float GetResolutionHeight() const = 0;
	virtual void SetCursorVisible(bool state) = 0;
	virtual void SetCursor(shared_ptr<ITexture> texture, int w, int h) = 0;
	virtual int GetDIPs() const = 0;
	virtual int GetTextureUsedPerFrame() const = 0;
	virtual int GetShaderUsedPerFrame() const = 0;
	virtual int GetRenderedTriangles() const = 0;
	virtual void SetAmbientColor(Vector3 color) = 0;
	virtual Vector3 GetAmbientColor() const = 0;
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
	virtual vector<Videomode> GetVideoModeList() = 0;
	virtual void LoadColorGradingMap(const char * fileName) = 0;
	virtual float GetGUIWidthScaleFactor() const = 0;
	virtual float GetGUIHeightScaleFactor() const = 0;
	virtual bool IsRunning() const = 0;
	virtual void Shutdown() = 0;
};

class ISceneFactory {
public:
	virtual ~ISceneFactory() {}

	virtual IEngine * const GetEngineInterface() const = 0;

	virtual shared_ptr<ISceneNode> CreateSceneNode() = 0;
	virtual shared_ptr<ISceneNode> CreateSceneNodeDuplicate(shared_ptr<ISceneNode> src) = 0;
	virtual shared_ptr<IPointLight> CreatePointLight() = 0;
	virtual shared_ptr<ISpotLight> CreateSpotLight() = 0;
	virtual shared_ptr<IDirectionalLight> CreateDirectionalLight() = 0;
	virtual shared_ptr<ICamera> CreateCamera(float fov) = 0;
	virtual shared_ptr<IParticleSystem> CreateParticleSystem(int particleCount) = 0;
	virtual shared_ptr<IFog> CreateFog(const Vector3 & min, const Vector3 & max, const Vector3 & color, float density) = 0;

	virtual shared_ptr<ISceneNode> FindByName(const string & name) = 0;
	virtual shared_ptr<ISceneNode> LoadScene(const string & file) = 0;
	virtual int GetNodeCount() = 0;
	virtual shared_ptr<ISceneNode> GetNode(int i) = 0;
	virtual vector<shared_ptr<ISceneNode>> GetTaggedObjects(const string & tag) = 0;

	virtual int GetSpotLightCount() = 0;
	virtual shared_ptr<ISpotLight> GetSpotLight(int n) = 0;

	virtual int GetPointLightCount() = 0;
	virtual shared_ptr<IPointLight> GetPointLight(int n) = 0;

	virtual int GetDirectionalLightCount() = 0;
	virtual shared_ptr<IDirectionalLight> GetDirectionalLight(int n) = 0;

	virtual void SetPointLightDefaultTexture(const shared_ptr<ICubeTexture> & defaultPointTexture) = 0;
	virtual shared_ptr<ICubeTexture> GetPointLightDefaultTexture() const = 0;
	virtual void SetSpotLightDefaultTexture(const shared_ptr<ITexture> & defaultSpotTexture) = 0;
	virtual shared_ptr<ITexture> GetSpotLightDefaultTexture() const = 0;
};

class ISound : public std::enable_shared_from_this<ISound> {
public:
	virtual ~ISound() {}
	static PlaybackCallback PlayCallback;
	virtual void Attach(const shared_ptr<ISceneNode> & node) = 0;
	virtual void Play(int oneshot = 1) = 0;
	virtual void Pause() = 0;
	virtual void SetVolume(float vol) = 0;
	virtual void SetPosition(Vector3 pos) = 0;
	virtual void SetReferenceDistance(float rd) = 0;
	virtual void SetRolloffFactor(float rolloffDistance) = 0;
	virtual void SetRoomRolloffFactor(float rolloffDistance) = 0;
	virtual void SetMaxDistance(float maxDistance) = 0;
	virtual int IsPlaying() = 0;
	virtual void SetPitch(float pitch) = 0;
	virtual bool IsPaused() = 0;
	virtual float GetLength() = 0;
	virtual void SetLoop(bool state) = 0;
	virtual float GetPlaybackPosition() = 0;
	virtual void Stop() = 0;
	virtual bool Is3D() const = 0;
	virtual void SetPlaybackPosition(float timeSeconds) = 0;
	virtual Vector3 GetPosition() const = 0;
};

enum class ReverbPreset : int {
	Generic, Paddedcell, Room, Bathroom, Livingroom, Stoneroom, Auditorium, Concerthall, Cave, Arena, Hangar, Carpettedhallway, Hallway,
	Stonecorridor, Alley, Forest, City, Mountains, Quarry, Plain, Parkinglot, Sewerpipe, Underwater, Drugged, Dizzy, Psychotic, CastleSmallroom,
	CastleShortpassage, CastleMediumroom, CastleLongpassage, CastleLargeroom, CastleHall, CastleCupboard, CastleCourtyard, CastleAlcove,
	FactoryAlcove, FactoryShortpassage, FactoryMediumroom, FactoryLongpassage, FactoryLargeroom, FactoryHall, FactoryCupboard, FactoryCourtyard,
	FactorySmallroom, IcepalaceAlcove, IcepalaceShortpassage, IcepalaceMediumroom, IcepalaceLongpassage, IcepalaceLargeroom, IcepalaceHall,
	IcepalaceCupboard, IcepalaceCourtyard, IcepalaceSmallroom, SpacestationAlcove, SpacestationMediumroom, SpacestationShortpassage,
	SpacestationLongpassage, SpacestationLargeroom, SpacestationHall, SpacestationCupboard, SpacestationSmallroom, WoodenAlcove, WoodenShortpassage,
	WoodenMediumroom, WoodenLongpassage, WoodenLargeroom, WoodenHall, WoodenCupboard, WoodenSmallroom, WoodenCourtyard, SportEmptystadium,
	SportSquashcourt, SportSmallswimmingpool, SportLargeswimmingpool, SportGymnasium, SportFullstadium, SportStadiumtannoy, PrefabWorkshop,
	PrefabSchoolroom, PrefabPractiseroom, PrefabOuthouse, PrefabCaravan, DomeTomb, PipeSmall, DomeSaintpauls, PipeLongthin, PipeLarge, PipeResonant,
	OutdoorsBackyard, OutdoorsRollingplains, OutdoorsDeepcanyon, OutdoorsCreek, OutdoorsValley, MoodHeaven, MoodHell, MoodMemory, DrivingCommentator,
	DrivingPitgarage, DrivingIncarRacer, DrivingIncarSports, DrivingIncarLuxury, DrivingFullgrandstand, DrivingEmptygrandstand, DrivingTunnel,
	CityStreets, CitySubway, CityMuseum, CityLibrary, CityUnderpass, CityAbandoned, Dustyroom, Chapel, Smallwaterroom
};

class ISoundSystem {
public:
	virtual ~ISoundSystem() {}

	virtual void SetMasterVolume(float volume) = 0;
	virtual float GetMasterVolume() = 0;
	virtual void SetReverbPreset(ReverbPreset preset) = 0;
	virtual int GetSoundCount() = 0;
	virtual shared_ptr<ISound> GetSound(int i) = 0;
	virtual shared_ptr<ISound> LoadSound2D(const string & file) = 0;
	virtual shared_ptr<ISound> LoadSound3D(const string & file) = 0;
	virtual shared_ptr<ISound> LoadMusic(const string & file) = 0;
	virtual shared_ptr<ISound> LoadMusic3D(const string & file) = 0;
};

class IInput {
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
	virtual ~IInput() {}
	virtual string GetKeyName(IInput::Key key) = 0;
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

class IEngine {
public:
	static unique_ptr<IEngine> Create(int width, int height, bool fullscreen, bool vSync);

public:
	virtual shared_ptr<IFont> CreateBitmapFont(int size, const string & filename) = 0;
	virtual IRenderer * const GetRenderer() const = 0;
	virtual ISceneFactory * const GetSceneFactory() const = 0;
	virtual shared_ptr<IGUIScene> CreateGUIScene() = 0;
	virtual IPhysics * const GetPhysics() const = 0;
	virtual IInput * const GetInput() const = 0;
	virtual ISoundSystem * const GetSoundSystem() const = 0;

	IEngine() {}
	virtual ~IEngine() {}
};