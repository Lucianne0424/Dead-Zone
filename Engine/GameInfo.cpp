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
		PlayerInfo playerinfo =
		{
			INFO_TYPE::PLAYER,
			L"Player",
		};

		shared_ptr<PlayerInfo> info = make_shared<PlayerInfo>(playerinfo);
		Add<PlayerInfo>(L"Player", info);
	}
}

void GameInfo::CreateDefaultZombieInfo()
{
	// Normal Zombie Info
	{
		ZombieInfo zombieInfo =
		{
			INFO_TYPE::ZOMBIE,
			L"NormalZombie",
			ZOMBIE_TYPE::NORMAL,
		};

		shared_ptr<ZombieInfo> info = make_shared<ZombieInfo>(zombieInfo);
		Add<ZombieInfo>(L"NormalZombie", info);
	}
}

void GameInfo::CreateDefaultGunInfo()
{
	// M91 Info
	{
		GunInfo gunInfo =
		{
			INFO_TYPE::GUN,
			L"M91",
			GUN_TYPE::M91,
		};

		shared_ptr<GunInfo> info = make_shared<GunInfo>(gunInfo);
		Add<GunInfo>(L"M91", info);
	}
}

