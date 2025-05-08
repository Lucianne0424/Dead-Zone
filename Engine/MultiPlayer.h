#pragma once
#include "Player.h"

class MultiPlayer : public Player
{
public:
	MultiPlayer();
	virtual ~MultiPlayer();

	virtual void LateUpdate() override;

	virtual void SetState(PlayerState playerState) override;

public:

};