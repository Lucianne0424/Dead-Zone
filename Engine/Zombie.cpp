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
	_info = GAME_INFO->Get<ZombieInfo>(L"NormalZombie");
}

void Zombie::Update()
{
	if (INPUT->GetButtonDown(KEY_TYPE::KEY_1))
	{
		int32 count = GetAnimator()->GetAnimCount();
		int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

		int32 index = (currentIndex + 1) % count;
		GetAnimator()->Play(index);
	}

	if (INPUT->GetButtonDown(KEY_TYPE::KEY_2))
	{
		int32 count = GetAnimator()->GetAnimCount();
		int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

		int32 index = (currentIndex - 1 + count) % count;
		GetAnimator()->Play(index);
	}
}

void Zombie::LateUpdate()
{
}
