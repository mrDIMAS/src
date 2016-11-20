#include "Precompiled.h"
#include "Button.h"
#include "Player.h"
#include "Utils.h"
#include "Level.h"

void Button::Update()
{
	auto & player = Game::Instance()->GetLevel()->GetPlayer();
	if(player->mNearestPickedNode == mNode) {
		player->GetHUD()->SetAction(player->mKeyUse, mText);
		if(player->IsUseButtonHit() && !mPushAnimation.IsEnabled()) {
			mPushAnimation.SetEnabled(true);
		}
	}

	mPushAnimation.Update();
}

Button::Button(const shared_ptr<ruSceneNode> & node, const string & text, const ruDelegate & onPush) :
	mNode(node),
	mText(text)
{
	mPushSound = ruSound::Load3D("data/sounds/button.ogg");
	mPushSound->Attach(mNode);

	mPushAnimation = ruAnimation(0, mNode->GetTotalAnimationFrameCount() - 1, 0.8f, false);
	mPushAnimation.AddFrameListener(mNode->GetTotalAnimationFrameCount() / 2, [this] { OnPush(); mPushSound->Play(); });

	mNode->SetAnimation(&mPushAnimation);

	OnPush += onPush;
}
