#include "pch.h"
#include "GameInfo.h"

void GameInfo::Init()
{
	CreateDefaultPlayerInfo();
	CreateDefaultZombieInfo();
	CreateDefaultGunInfo();
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
				1.f,
				100.f,
				3.3f,
			});
		Add<ZombieInfo>(L"NormalZombie", info);
	}
}

void GameInfo::CreateDefaultGunInfo()
{
	// M4A1 Info
	{
		shared_ptr<GunInfo> info = make_shared<GunInfo>(
			GunInfo
			{
				INFO_TYPE::GUN,
				L"M4A1",
				GUN_TYPE::M4A1,

				60,					// 공격력
				10.f,				// 발사 속도 (발/s)	
				500.f,				// 사거리 (m)	
				2.5f,				// 재장전 속도 (s)
				30,					// 장탄수
				2880,				// 무게 (g)
				2.f,				// 반동
				60.f,				// 정조준 시 시야각
				1.f,				// 정조준 속도 (s)
			});
		Add<GunInfo>(L"M4A1", info);
	}
}

