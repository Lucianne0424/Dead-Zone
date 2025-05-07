#include "pch.h"
#include "MultiPlayer.h"
#include "Animator.h"

MultiPlayer::MultiPlayer()
{
	_name = L"MultiPlayer";

	shared_ptr<PlayerInfo> info = GET_SINGLE(GameInfo)->Get<PlayerInfo>(L"Player");
	_info = *info;

	_state = PlayerState::T_POSE;
}

MultiPlayer::~MultiPlayer()
{
}

void MultiPlayer::LateUpdate()
{
}

void MultiPlayer::SetState(PlayerState playerState)
{
	// 현재 상태와 패킷의 상태가 같으면 아무것도 하지 않음
	if (_state == playerState)
		return;

	_state = playerState;
	switch (playerState)
	{
	case PlayerState::T_POSE:
		uint32 index = static_cast<uint32>(PlayerState::T_POSE);
		GetAnimator()->Play(index);
		break;
	case PlayerState::IDLE:
		uint32 index = static_cast<uint32>(PlayerState::IDLE);
		GetAnimator()->Play(index);
		break;
	case PlayerState::RUN_FORWARD:
		uint32 index = static_cast<uint32>(PlayerState::RUN_FORWARD);
		GetAnimator()->Play(index);
		break;
	case PlayerState::RUN_BACKWARD:
		uint32 index = static_cast<uint32>(PlayerState::RUN_BACKWARD);
		GetAnimator()->Play(index);
		break;
	case PlayerState::RUN_LEFT:
		uint32 index = static_cast<uint32>(PlayerState::RUN_LEFT);
		GetAnimator()->Play(index);
		break;
	case PlayerState::RUN_RIGHT:
		uint32 index = static_cast<uint32>(PlayerState::RUN_RIGHT);
		GetAnimator()->Play(index);
		break;
	case PlayerState::FIRE:
		uint32 index = static_cast<uint32>(PlayerState::FIRE);
		GetAnimator()->Play(index);
		break;
	default:
		break;
	}
}
