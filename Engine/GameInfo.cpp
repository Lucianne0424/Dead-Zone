#include "pch.h"
#include "GameInfo.h"

void GameInfo::Init()
{
	CreateDefaultPlayerInfo();
	CreateDefaultZombieInfo();
}

void GameInfo::CreateDefaultPlayerInfo()
{
	// Default Player Info
	{
		PlayerInfo info =
		{
			INFO_TYPE::PLAYER,
			L"Player",
		};

		shared_ptr<PlayerInfo> playerInfo = make_shared<PlayerInfo>();
		Add<PlayerInfo>(L"Player", playerInfo);
	}
}

void GameInfo::CreateDefaultZombieInfo()
{
	// Normal Zombie Info
	{
		ZombieInfo info =
		{
			INFO_TYPE::ZOMBIE,
			L"NormalZombie",
			ZOMBIE_TYPE::NORMAL,
		};

		shared_ptr<ZombieInfo> zombieInfo = make_shared<ZombieInfo>();
		Add<ZombieInfo>(L"NormalZombie", zombieInfo);
	}
}

