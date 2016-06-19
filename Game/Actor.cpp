#include "Precompiled.h"

#include "Actor.h"

void Actor::Move(ruVector3 direction, float speed) {
	mBody->Move(direction * speed);
}



Actor::Actor(float height, float width) :
	mBodyHeight(height),
	mBodyWidth(width),
	mSpringLength(1.0f),
	mCrouch(false),
	mCrouchMultiplier(1.0f),
	mVerticalSpeed(0.0f),
	mLastVerticalPosition(0.0f),
	mMaxHealth(100),
	mHealth(100) {
	mBody = ruSceneNode::Create();
	mBody->SetCapsuleBody(mBodyHeight, mBodyWidth);
	mBody->SetAngularFactor(ruVector3(0, 0, 0));
	mBody->SetFriction(0);
	mBody->SetAnisotropicFriction(ruVector3(1, 1, 1));
	mBody->SetDamping(0, 0);
	mBody->SetMass(2);
	mBody->SetGravity(ruVector3(0, 0, 0));
}

void Actor::SetPosition(ruVector3 position) {
	mBody->SetPosition(position);
}

Actor::~Actor() {

}

char Actor::IsInsideZone(const shared_ptr<ruSceneNode> & zone) {
	return mBody->IsInsideNode(zone);
}

ruVector3 Actor::GetCurrentPosition() {
	return mBody->GetPosition();
}

void Actor::StopInstant() {
	mBody->Move(ruVector3(0.0f, 0.0f, 0.0f));
}

void Actor::Unfreeze() {
	mBody->Unfreeze();
	mBody->SetCollisionEnabled(true);
}

void Actor::Freeze() {
	mBody->Freeze();
	mBody->SetCollisionEnabled(false);
}

ruVector3 Actor::GetLookDirection() {
	return mBody->GetLookVector();
}

void Actor::SetBodyVisible(bool state) {
	if (state) {
		mBody->Show();
	} else {
		mBody->Hide();
	}
}

bool Actor::IsVisibleFromPoint(ruVector3 begin) {
	return ruPhysics::CastRay(begin, GetCurrentPosition(), nullptr) == mBody;
}

void Actor::Crouch(bool state) {
	mCrouch = state;

	// stand up only if we can
	ruVector3 pickPoint;
	ruVector3 rayBegin = mBody->GetPosition() + ruVector3(0, mBodyHeight * mCrouchMultiplier * 1.025f, 0);
	ruVector3 rayEnd = mBody->GetPosition() + ruVector3(0, mBodyHeight * 1.05f, 0);
	shared_ptr<ruSceneNode> upCast = ruPhysics::CastRay(rayBegin, rayEnd, &pickPoint);
	if (upCast) {
		if (!mCrouch) {
			mCrouch = true;
		}
	}
}

void Actor::UpdateCrouch() {
	if (mCrouch) {
		mCrouchMultiplier -= 0.025f;
		if (mCrouchMultiplier < 0.5f) {
			mCrouchMultiplier = 0.5f;
		}
	} else {
		mCrouchMultiplier += 0.025f;
		if (mCrouchMultiplier > 1.0f) {
			mCrouchMultiplier = 1.0f;
		}
	}

	mBody->SetLocalScale(ruVector3(1.0f, mCrouchMultiplier, 1.0f));
}

bool Actor::IsCrouch() {
	return mCrouch;
}

void Actor::Damage(float dmg) {
	mHealth -= fabsf(dmg);
	if (mHealth < 0.0f) {
		mHealth = 0.0f;
	}
}

void Actor::Heal(float howMuch) {
	mHealth += fabsf(howMuch);
	if (mHealth > mMaxHealth) {
		mHealth = mMaxHealth;
	}
}
