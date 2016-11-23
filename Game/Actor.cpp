#include "Precompiled.h"

#include "Actor.h"

void Actor::Move(Vector3 direction, float speed) {
	mBody->Move(direction * speed);
}



Actor::Actor(unique_ptr<Game> & game, float height, float width) :
	mGame(game),
	mBodyHeight(height),
	mBodyWidth(width),
	mSpringLength(1.0f),
	mCrouch(false),
	mCrouchMultiplier(1.0f),
	mVerticalSpeed(0.0f),
	mLastVerticalPosition(0.0f),
	mMaxHealth(100),
	mHealth(100) {
	mBody = mGame->GetEngine()->GetSceneFactory()->CreateSceneNode();
	mBody->SetCapsuleBody(mBodyHeight, mBodyWidth);
	mBody->SetAngularFactor(Vector3(0, 0, 0));
	mBody->SetFriction(0);
	mBody->SetAnisotropicFriction(Vector3(1, 1, 1));
	mBody->SetDamping(0, 0);
	mBody->SetMass(2);
	mBody->SetGravity(Vector3(0, 0, 0));
}

void Actor::SetPosition(Vector3 position) {
	mBody->SetPosition(position);
}

Actor::~Actor() {

}

char Actor::IsInsideZone(const shared_ptr<ISceneNode> & zone) {
	return mBody->IsInsideNode(zone);
}

Vector3 Actor::GetCurrentPosition() {
	return mBody->GetPosition();
}

void Actor::StopInstantly() {
	mBody->Move(Vector3(0.0f, 0.0f, 0.0f));
}

void Actor::Unfreeze() {
	mBody->Unfreeze();
	mBody->SetCollisionEnabled(true);
}

void Actor::Freeze() {
	mBody->Freeze();
	mBody->SetCollisionEnabled(false);
}

Vector3 Actor::GetLookDirection() {
	return mBody->GetLookVector();
}

void Actor::SetBodyVisible(bool state) {
	if(state) {
		mBody->Show();
	} else {
		mBody->Hide();
	}
}

bool Actor::IsVisibleFromPoint(Vector3 begin) {
	return mGame->GetEngine()->GetPhysics()->CastRay(begin, GetCurrentPosition(), nullptr) == mBody;
}

bool Actor::IsCrouch() {
	return mCrouch;
}

void Actor::Damage(float dmg) {
	mHealth -= fabsf(dmg);
	if(mHealth < 0.0f) {
		mHealth = 0.0f;
	}
}

void Actor::Heal(float howMuch) {
	mHealth += fabsf(howMuch);
	if(mHealth > mMaxHealth) {
		mHealth = mMaxHealth;
	}
}
