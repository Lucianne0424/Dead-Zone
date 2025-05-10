#include "pch.h"
#include "MultiPlayer.h"
#include "Animator.h"

MultiPlayer::MultiPlayer()
{
	_name = L"MultiPlayer";

	shared_ptr<PlayerInfo> info = GET_SINGLE(GameInfo)->Get<PlayerInfo>(L"Player");
	_info = *info;

	_state = PLAYER_STATE::T_POSE;
}

MultiPlayer::~MultiPlayer()
{
}

void MultiPlayer::LateUpdate()
{
}

void MultiPlayer::SetState(PLAYER_STATE playerState)
{
	// 현재 상태와 패킷의 상태가 같으면 아무것도 하지 않음
	if (_state == playerState)
		return;

	_state = playerState;
	switch (playerState)
	{
	case PLAYER_STATE::T_POSE:
	{
		uint32 index = static_cast<uint32>(PLAYER_ANIMATION_TYPE::T_POSE);
		GetAnimator()->Play(index);
		break;
	}
	case PLAYER_STATE::IDLE:
	{
		uint32 index = static_cast<uint32>(PLAYER_ANIMATION_TYPE::IDLE);
		GetAnimator()->Play(index);
		break;
	}
	case PLAYER_STATE::RUN_FORWARD:
	{
		uint32 index = static_cast<uint32>(PLAYER_ANIMATION_TYPE::RUN_FORWARD);
		GetAnimator()->Play(index);
		break;
	}
	case PLAYER_STATE::RUN_BACKWARD:
	{
		uint32 index = static_cast<uint32>(PLAYER_ANIMATION_TYPE::RUN_BACKWARD);
		GetAnimator()->Play(index);
		break;
	}
	case PLAYER_STATE::RUN_LEFT:
	{
		uint32 index = static_cast<uint32>(PLAYER_ANIMATION_TYPE::RUN_LEFT);
		GetAnimator()->Play(index);
		break;
	}
	case PLAYER_STATE::RUN_RIGHT:
	{
		uint32 index = static_cast<uint32>(PLAYER_ANIMATION_TYPE::RUN_RIGHT);
		GetAnimator()->Play(index);
		break;
	}
	case PLAYER_STATE::FIRE:
	{
		uint32 index = static_cast<uint32>(PLAYER_ANIMATION_TYPE::FIRE);
		GetAnimator()->Play(index);
		break;
	}
	default:
		break;
	}
}
