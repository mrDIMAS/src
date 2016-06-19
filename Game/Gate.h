#pragma once

#include "Game.h"
#include "Player.h"

class Gate {
public:
	enum class State : int {
		Opening,
		Closing,
		Opened,
		Closed,
	};
private:
	friend class Level;
	explicit Gate(shared_ptr<ruSceneNode> gate, shared_ptr<ruSceneNode> buttonOpen, shared_ptr<ruSceneNode> buttonClose, shared_ptr<ruSceneNode> buttonOpen2, shared_ptr<ruSceneNode> buttonClose2);

	State mState;
	shared_ptr<ruSceneNode> mGate;
	shared_ptr<ruSceneNode> mButtonOpen[2];
	shared_ptr<ruSceneNode> mButtonClose[2];
	shared_ptr<ruSound> mBeginSound;
	shared_ptr<ruSound> mIdleSound;
	shared_ptr<ruSound> mEndSound;
	shared_ptr<ruSound> mButtonSound;

	ruAnimation mOpenAnim;
	ruAnimation mCloseAnim;
	ruAnimation mButtonPushAnim[4];

	void Proxy_ButtonPush();
	void Proxy_BeginGateClosing();
	void Proxy_BeginGateOpening();
	void Proxy_Opening();
	void Proxy_Opened();
	void Proxy_Idle();
	void Proxy_Closed();
	void Proxy_Closing();
public:
	~Gate();
	void Update();
	void Open();
	shared_ptr<ruSceneNode> GetNode() const;
	State GetState() const;
};