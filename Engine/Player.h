#pragma once
#include "MonoBehaviour.h"
#include "GameInfo.h"

enum PlayerState
{
	T_POSE,
	IDLE,
	RUN_FORWARD,
	RUN_BACKWARD,
	RUN_LEFT,
	RUN_RIGHT,
	FIRE,
};

class Player : public MonoBehaviour
{
public:
	Player();
	virtual ~Player();

	virtual void LateUpdate() override;

	virtual void SetState(PlayerState state) {}
	
protected:
	PlayerState _state;
	PlayerInfo _info;
};

