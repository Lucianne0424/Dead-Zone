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

		shared_ptr<GunInfo> gunInfo = make_shared<GunInfo>();
		Add<GunInfo>(L"M91", gunInfo);
	}
}

