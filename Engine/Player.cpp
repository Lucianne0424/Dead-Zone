#include "pch.h"
#include "Player.h"

Player::Player()
{
	_name = L"Player";

	shared_ptr<PlayerInfo> info = GET_SINGLE(GameInfo)->Get<PlayerInfo>(L"Player");
	_info = *info;

	_state = PlayerState::T_POSE;
}

Player::~Player()
{
}

void Player::Awake()
{
}

void Player::Start()
{
}

void Player::Update()
{
}

void Player::LateUpdate()
{
}
