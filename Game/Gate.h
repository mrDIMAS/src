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
	State mState;
	shared_ptr<ISceneNode> mGate;
	shared_ptr<ISceneNode> mButtonOpen[2];
	shared_ptr<ISceneNode> mButtonClose[2];
	shared_ptr<ISound> mBeginSound;
	shared_ptr<ISound> mIdleSound;
	shared_ptr<ISound> mEndSound;
	shared_ptr<ISound> mButtonSound;

	Animation mOpenAnim;
	Animation mCloseAnim;
	Animation mButtonPushAnim[4];
public:
	bool mLocked;
	explicit Gate(shared_ptr<ISceneNode> gate, shared_ptr<ISceneNode> buttonOpen, shared_ptr<ISceneNode> buttonClose, shared_ptr<ISceneNode> buttonOpen2, shared_ptr<ISceneNode> buttonClose2);
	~Gate();
	void Update();
	void Open();
	void Close();
	shared_ptr<ISceneNode> GetNode() const;
	State GetState() const;
};