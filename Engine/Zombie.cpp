#include "pch.h"
#include "Zombie.h"
#include "Input.h"
#include "Animator.h"

Zombie::Zombie()
{
}

Zombie::~Zombie()
{
}

void Zombie::Awake()
{
	shared_ptr<ZombieInfo> info = GAME_INFO->Get<ZombieInfo>(L"NormalZombie");
	_info = *info;
}

void Zombie::Update()
{
	if (INPUT->GetButtonDown(KEY_TYPE::KEY_1))
	{
		int32 index = static_cast<int32>(KEY_TYPE::KEY_1);
		GetAnimator()->Play(index);
	}

	if (INPUT->GetButtonDown(KEY_TYPE::KEY_2))
	{
		int32 index = static_cast<int32>(KEY_TYPE::KEY_2);
		GetAnimator()->Play(index);
	}

	if (INPUT->GetButtonDown(KEY_TYPE::KEY_3))
	{
		int32 index = static_cast<int32>(KEY_TYPE::KEY_3);
		GetAnimator()->Play(index);
	}

	if (INPUT->GetButtonDown(KEY_TYPE::KEY_4))
	{
		int32 index = static_cast<int32>(KEY_TYPE::KEY_4);
		GetAnimator()->Play(index);
	}
}

void Zombie::LateUpdate()
{
}
