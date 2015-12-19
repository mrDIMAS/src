#pragma once

#include "Game.h"
#include "SaveFile.h"

class Door {
public:
    enum class State {
        Opened,
        Closed,
        Closing,
        Opening,
    };
	enum class TurnDirection {
		Clockwise,
		Counterclockwise,
	};
public:
    shared_ptr<ruSceneNode> mDoorNode;
    float mMaxAngle;
    float mOffsetAngle;
    float mCurrentAngle;
	TurnDirection mTurnDirection;
    State mState;
	bool mLocked;
    ruSound mOpenSound;
    ruSound mCloseSound;
public:
    static vector< Door* > msDoorList;
    explicit Door( shared_ptr<ruSceneNode> hDoor, float fMaxAngle );
    virtual ~Door();
    bool IsPickedByPlayer();
    void SwitchState();
    State GetState();
	void SetTurnDirection( TurnDirection direction );
	bool IsLocked();
	void SetLocked( bool state );
    void Open( );
    void Close();
    virtual void DoInteraction();	
	void Serialize( SaveFile & out );
	void Deserialize( SaveFile & in );
	static Door * GetByName( const string & name );
};