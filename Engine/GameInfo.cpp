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

		Add<PlayerInfo>(L"Player", info);
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

		Add<ZombieInfo>(L"NormalZombie", info);
	}
}

void GameInfo::CreateDefaultGunInfo()
{
	// M91 Info
	{
		GunInfo info =
		{
			INFO_TYPE::GUN,
			L"M91",
			GUN_TYPE::M91,
		};

		Add<GunInfo>(L"M91", info);
	}
}

