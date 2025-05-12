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

				60,					// ���ݷ�
				10.f,				// �߻� �ӵ� (��/s)	
				500.f,				// ��Ÿ� (m)	
				2.5f,				// ������ �ӵ� (s)
				30,					// ��ź��
				2880,				// ���� (g)
				2.f,				// �ݵ�
				60.f,				// ������ �� �þ߰�
				1.f,				// ������ �ӵ� (s)
			});
		Add<GunInfo>(L"M4A1", info);
	}
}

