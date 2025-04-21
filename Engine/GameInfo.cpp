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
		shared_ptr<PlayerInfo> info = make_shared<PlayerInfo>(
			PlayerInfo
			{
				INFO_TYPE::PLAYER,
				L"Player",

				100,
				3.f,
				5.f,
			});
		Add<PlayerInfo>(L"Player", info);
	}
}

void GameInfo::CreateDefaultZombieInfo()
{
	// Normal Zombie Info
	{
		shared_ptr<ZombieInfo> info = make_shared<ZombieInfo>(
			ZombieInfo
			{
				INFO_TYPE::ZOMBIE,
				L"NormalZombie",

				ZOMBIE_TYPE::NORMAL,
				100,
				10,
				100.f,
				3.3f,
			});
		Add<ZombieInfo>(L"NormalZombie", info);
	}
}

void GameInfo::CreateDefaultGunInfo()
{
	// M91 Info
	{
		shared_ptr<GunInfo> info = make_shared<GunInfo>(
			GunInfo
			{
				INFO_TYPE::GUN,
				L"M91",
				GUN_TYPE::M91,

				1000,
				4.f,
				1000.f,
				2.f,
				5,
				5500
			});
		Add<GunInfo>(L"M91", info);
	}
}

