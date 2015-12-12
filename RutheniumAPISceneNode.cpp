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
#include "Precompiled.h"
#include "RutheniumAPI.h"
#include "SceneNode.h"
#include "Light.h"

ruSceneNode ruSceneNode::FindChild( const string & name ) {
	return SceneNode::HandleFromPointer( SceneNode::FindInObjectByName( SceneNode::CastHandle( *this ), name ));
}

ruSceneNode ruSceneNode::Create( ) {
	return SceneNode::HandleFromPointer( new SceneNode );
}

ruSceneNode ruSceneNode::LoadFromFile( const string & file ) {
	return SceneNode::HandleFromPointer( SceneNode::LoadScene( file ));
}

ruSceneNode ruSceneNode::FindByName( const string & name ) {
	return SceneNode::HandleFromPointer( SceneNode::FindByName( name ));
}

bool ruSceneNode::IsLight() {
	SceneNode * node = static_cast<SceneNode*>( pointer );
	return dynamic_cast<Light*>( node ) != nullptr;
}

ruSceneNode ruSceneNode::Duplicate( ruSceneNode source ) {
	if( SceneNode::CastHandle( source ) == nullptr ) {
		ruSceneNode null;
		null.pointer = nullptr;
		return null;
	}
	SceneNode * pNode = SceneNode::CastHandle( source );
	return SceneNode::HandleFromPointer( new SceneNode( *pNode ));
}

string ruSceneNode::GetProperty( string propName ) {
	return SceneNode::CastHandle( *this )->GetProperty( propName );
}

void ruSceneNode::Hide() {
	SceneNode::CastHandle( *this )->Hide();
}

void ruSceneNode::Show() {
	SceneNode::CastHandle( *this )->Show();
}

bool ruSceneNode::IsVisible() {
	return SceneNode::CastHandle( *this )->IsVisible();
}

void ruSceneNode::Free() {
	delete SceneNode::CastHandle( *this );
}

const string & ruSceneNode::GetName() {
	return SceneNode::CastHandle( *this )->GetName();
}

void ruSceneNode::SetDepthHack( float order ) {
	SceneNode::CastHandle( *this )->SetDepthHack( order );
}

void ruSceneNode::Attach( ruSceneNode parent ) {
	SceneNode::CastHandle( *this )->AttachTo( SceneNode::CastHandle( parent ) );
}

void ruSceneNode::Detach() {
	SceneNode::CastHandle( *this )->AttachTo( nullptr );
}

void ruSceneNode::SetDamping( float linearDamping, float angularDamping ) {
	SceneNode::CastHandle( *this )->SetDamping( linearDamping, angularDamping );
}

void ruSceneNode::SetPosition( ruVector3 position ) {
	SceneNode::CastHandle( *this )->SetPosition( position );
}

void ruSceneNode::SetRotation( ruQuaternion rotation ) {
	SceneNode::CastHandle( *this )->SetRotation( rotation );
}

void ruSceneNode::SetGravity( ruVector3 gravity ) {
	SceneNode::CastHandle( *this )->SetGravity( gravity );
}

int ruSceneNode::GetTextureCount() {
	return SceneNode::CastHandle( *this )->GetTextureCount();
}

ruTextureHandle ruSceneNode::GetTexture( int n ) {
	return SceneNode::CastHandle( *this )->GetTexture( n );
}

ruVector3 ruSceneNode::GetLookVector() {
	return SceneNode::CastHandle( *this )->GetLookVector();
}

ruVector3 ruSceneNode::GetRightVector() {
	return SceneNode::CastHandle( *this )->GetRightVector();
}

ruVector3 ruSceneNode::GetUpVector() {
	return SceneNode::CastHandle( *this )->GetUpVector();
}

ruVector3 ruSceneNode::GetPosition() {
	return SceneNode::CastHandle( *this )->GetPosition();
}

void ruSceneNode::SetMass( float mass ) {
	SceneNode::CastHandle( *this )->SetMass( mass );
}

ruQuaternion ruSceneNode::GetLocalRotation() {
	return SceneNode::CastHandle( *this )->GetLocalRotation();
}

ruVector3 ruSceneNode::GetLocalPosition() {
	return SceneNode::CastHandle( *this )->GetLocalPosition();
}

void ruSceneNode::SetLocalPosition( ruVector3 pos ) {
	 SceneNode::CastHandle( *this )->SetLocalPosition( pos );
}

void ruSceneNode::SetLocalRotation( ruQuaternion rot ) {
	 SceneNode::CastHandle( *this )->SetLocalRotation( rot );
}

void ruSceneNode::SetName( const string & name ) {
	SceneNode::CastHandle( *this )->mName = name;
}

ruVector3 ruSceneNode::GetAABBMin() {
	return SceneNode::CastHandle( *this )->GetAABBMin();
}
ruVector3 ruSceneNode::GetTotalForce() {
	return SceneNode::CastHandle( *this )->GetTotalForce();
}
ruVector3 ruSceneNode::GetAABBMax() {
	return SceneNode::CastHandle( *this )->GetAABBMax();
}
int ruSceneNode::IsInsideNode( ruSceneNode node ) {
	return SceneNode::CastHandle( *this )->IsNodeInside( SceneNode::CastHandle( node ));
}
ruSceneNode ruSceneNode::GetChild( int i ) {
	ruSceneNode handle;
	handle.pointer = SceneNode::CastHandle( *this )->GetChild( i );
	return handle;
}
int ruSceneNode::GetCountChildren() {
	return SceneNode::CastHandle( *this )->GetCountChildren();
}
bool ruSceneNode::IsInFrustum() {
	return SceneNode::CastHandle( *this )->mInFrustum;
}
void ruSceneNode::SetAlbedo( float albedo ) {
	SceneNode::CastHandle( *this )->mAlbedo = albedo;
}
ruVector3 ruSceneNode::GetLinearVelocity() {
	return SceneNode::CastHandle( *this )->GetLinearVelocity();
}

int ruSceneNode::GetContactCount() {
	return SceneNode::CastHandle( *this )->GetContactCount();
}

ruContact ruSceneNode::GetContact( int num ) {
	return SceneNode::CastHandle( *this )->GetContact( num );
}

void ruSceneNode::Freeze() {
	SceneNode::CastHandle( *this )->Freeze();
}

void ruSceneNode::Unfreeze() {
	SceneNode::CastHandle( *this )->Unfreeze();
}

void ruSceneNode::SetConvexBody() {
	SceneNode::CastHandle( *this )->SetConvexBody();
}

void ruSceneNode::SetCapsuleBody( float height, float radius ) {
	SceneNode::CastHandle( *this )->SetCapsuleBody( height, radius );
}

void ruSceneNode::SetAngularFactor( ruVector3 fact ) {
	SceneNode::CastHandle( *this )->SetAngularFactor( fact );
}

void ruSceneNode::SetTrimeshBody( ) {
	SceneNode::CastHandle( *this )->SetTrimeshBody();
}

void ruSceneNode::Move( ruVector3 speed ) {
	SceneNode::CastHandle( *this )->Move( speed );
}

void ruSceneNode::SetFriction( float friction ) {
	SceneNode::CastHandle( *this )->SetFriction( friction );
}

void ruSceneNode::SetLocalScale( ruVector3 scale ) {
	SceneNode::CastHandle( *this )->SetBodyLocalScaling( scale );
}

void ruSceneNode::SetAnisotropicFriction( ruVector3 aniso ) {
	SceneNode::CastHandle( *this )->SetAnisotropicFriction( aniso );
}

float ruSceneNode::GetMass() {
	return SceneNode::CastHandle( *this )->GetMass();
}
int ruSceneNode::IsFrozen() {
	return SceneNode::CastHandle( *this )->IsFrozen();
}

void ruSceneNode::SetLinearFactor( ruVector3 lin ) {
	SceneNode::CastHandle( *this )->SetLinearFactor( lin );
}

void ruSceneNode::SetVelocity( ruVector3 velocity ) {
	SceneNode::CastHandle( *this )->SetVelocity( velocity );
}
void ruSceneNode::SetAngularVelocity( ruVector3 velocity ) {
	SceneNode::CastHandle( *this )->SetAngularVelocity( velocity );
}

void ruSceneNode::SetAnimation( ruAnimation * newAnim, bool dontAffectChilds ) {
	SceneNode::CastHandle( *this )->SetAnimation( newAnim, dontAffectChilds );
}
int ruSceneNode::GetTotalAnimationFrameCount() {
	return SceneNode::CastHandle( *this )->mTotalFrameCount;
}
ruAnimation * ruSceneNode::GetCurrentAnimation() {
	return SceneNode::CastHandle( *this )->GetCurrentAnimation();
}

ruVector3 ruSceneNode::GetEulerAngles() {
	return SceneNode::CastHandle( *this )->GetEulerAngles();
}
ruVector3 ruSceneNode::GetAbsoluteLookVector() {
	return SceneNode::CastHandle( *this )->GetAbsoluteLookVector();
}
BodyType ruSceneNode::GetBodyType() {
	return SceneNode::CastHandle( *this )->GetBodyType();
}
ruVector3 ruSceneNode::GetRotationAxis() {
	return SceneNode::CastHandle( *this )->GetRotationAxis();
}
float ruSceneNode::GetRotationAngle() {
	return SceneNode::CastHandle( *this )->GetRotationAngle();
}
void ruSceneNode::AddForce( ruVector3 force ) {
	SceneNode::CastHandle( *this )->AddForce( force );
}
void ruSceneNode::AddForceAtPoint( ruVector3 force, ruVector3 point ) {
	SceneNode::CastHandle( *this )->AddForceAtPoint( force, point );
}
void ruSceneNode::AddTorque( ruVector3 torque ) {
	SceneNode::CastHandle( *this )->AddTorque( torque );
}

ruSceneNode ruSceneNode::GetParent() {
	ruSceneNode parent;
	parent.pointer = SceneNode::CastHandle( *this )->GetParent();
	return parent;
}