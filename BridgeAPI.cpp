#include "Engine.h"
#include "SceneNode.h"

ruNodeHandle ruNodeHandle::GetChildByName( const char * name ) {
	return ruNodeHandle( SceneNode::FindInObjectByName( ((SceneNode*)pointer), name ));
}

string ruNodeHandle::GetProperty( string propName ) {
	return ((SceneNode*)pointer)->GetProperty( propName );
}

void ruNodeHandle::Hide() {
	((SceneNode*)pointer)->Hide();
}

void ruNodeHandle::Show() {
	((SceneNode*)pointer)->Show();
}

bool ruNodeHandle::IsVisible() {
	return ((SceneNode*)pointer)->IsVisible();
}

const char * ruNodeHandle::GetName() {
	return ((SceneNode*)pointer)->GetName();
}

void ruNodeHandle::SetDepthHack( float order ) {
	((SceneNode*)pointer)->SetDepthHack( order );
}

void ruNodeHandle::AttachTo( ruNodeHandle node2 ) {
	((SceneNode*)pointer)->AttachTo( ((SceneNode*)node2.pointer) );
}

void ruNodeHandle::Detach() {
	if( ((SceneNode*)pointer)->parent ) {
		((SceneNode*)pointer)->parent->EraseChild( ((SceneNode*)this->pointer) );
	}
	((SceneNode*)pointer)->parent = 0;
}

void ruNodeHandle::SetBodyDamping( float linearDamping, float angularDamping ) {
	((SceneNode*)pointer)->SetDamping( linearDamping, angularDamping );
}

void ruNodeHandle::SetPosition( ruVector3 position ) {
	((SceneNode*)pointer)->SetPosition( position );
}

void ruNodeHandle::SetRotation( ruQuaternion rotation ) {
	((SceneNode*)pointer)->SetRotation( rotation );
}

void ruNodeHandle::SetGravity( ruVector3 gravity ) {
	((SceneNode*)pointer)->SetGravity( gravity );
}

ruVector3 ruNodeHandle::GetLookVector() {
	return ((SceneNode*)pointer)->GetLookVector();
}

ruVector3 ruNodeHandle::GetRightVector() {

}

ruVector3 ruNodeHandle::GetUpVector() {

}

ruVector3 ruNodeHandle::GetPosition() {

}

void ruNodeHandle::SetBodyMass( float mass ) {

}

ruQuaternion ruNodeHandle::GetLocalRotation() {

}

ruVector3 ruNodeHandle::GetLocalPosition() {

}

void ruNodeHandle::SetLocalPosition( ruVector3 pos ) {

}

void ruNodeHandle::SetLocalRotation( ruQuaternion rot ) {

}

void ruNodeHandle::SetName( const char * name ) {

}

ruVector3 ruNodeHandle::GetAABBMin( ruNodeHandle node ) {

}

ruVector3 ruNodeHandle::GetAABBMax( ruNodeHandle node ) {

}

int ruNodeHandle::IsInside( ruNodeHandle node2 ) {

}

ruNodeHandle ruNodeHandle::GetChild( int i ) {

}

int ruNodeHandle::GetCountChildren() {

}

bool ruNodeHandle::IsInFrustum() {

}

void ruNodeHandle::SetAlbedo( float albedo ) {

}