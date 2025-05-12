#pragma once

enum class INFO_TYPE : uint8
{
	NONE,
	PLAYER,
	ZOMBIE,
	GUN,

	END
};

enum class ZOMBIE_TYPE : uint8
{
	NORMAL,

	END
};

enum class GUN_TYPE : uint8
{
	M4A1,
	AK47,

	END
};

enum
{
	INFO_TYPE_COUNT = static_cast<uint8>(INFO_TYPE::END),

	ZOMBIE_TYPE_COUNT = static_cast<uint8>(ZOMBIE_TYPE::END),
	GUN_TYPE_COUNT = static_cast<uint8>(GUN_TYPE::END)
};

struct BaseInfo
{
	INFO_TYPE		type = INFO_TYPE::NONE;
	wstring			name = L"";
};

struct PlayerInfo : BaseInfo
{
	uint32			hp;
	float			walkSppeed;	// �ȱ� (m/s)
	float			runSpeed;	// �ٱ� (m/s)
};

struct ZombieInfo : BaseInfo
{
	ZOMBIE_TYPE		zombieType;
	uint32			hp;
	uint32			attackDamage;
	float			attackSpeed;	// ���� �ӵ� (m/s)
	float			walkSpeed;		// �ȱ� (m/s)
	float			runSpeed;		// �ٱ� (m/s)
};

struct GunInfo : BaseInfo
{
	GUN_TYPE		gunType;
	int32			damage;			// ���ݷ�
	float			fireRate;		// �߻� �ӵ� (��/s)
	float			range;			// ��Ÿ� (m)
	float			reloadSpeed;	// ������ �ӵ� (s)
	int32			ammoCapacity;	// ��ź��
	int32			weight;			// ���� (g)
	float			recoil;			// �ݵ�
	float			fov;			// ������ �� �þ߰�
	float 			aimSpeed;		// ������ �ӵ� (s)
};


class GameInfo
{
	DECLARE_SINGLE(GameInfo);

public:
	void Init();

	template<typename T>
	bool Add(const wstring& key, shared_ptr<T> info);

	template<typename T>
	shared_ptr<T> Get(const wstring& key);

	template<typename T>
	INFO_TYPE GetInfoType();

private:
	void CreateDefaultPlayerInfo();
	void CreateDefaultZombieInfo();
	void CreateDefaultGunInfo();

private:
	using KeyInfoMap = std::map<wstring/*key*/, shared_ptr<BaseInfo>>;
	array<KeyInfoMap, INFO_TYPE_COUNT> _info;
};

template<typename T>
inline bool GameInfo::Add(const wstring& key, shared_ptr<T> info)
{
	INFO_TYPE infoType = GetInfoType<T>();
	KeyInfoMap& keyInfoMap = _info[static_cast<uint8>(infoType)];

	auto findIt = keyInfoMap.find(key);
	if (findIt != keyInfoMap.end())
		return false;

	keyInfoMap[key] = info;

	return true;
}

template<typename T>
inline shared_ptr<T> GameInfo::Get(const wstring& key)
{
	INFO_TYPE infoType = GetInfoType<T>();
	KeyInfoMap& keyInfoMap = _info[static_cast<uint8>(infoType)];

	auto findIt = keyInfoMap.find(key);
	if (findIt != keyInfoMap.end())
		return static_pointer_cast<T>(findIt->second);

	return nullptr;
}

template<typename T>
inline INFO_TYPE GameInfo::GetInfoType()
{
	if (std::is_same_v<T, PlayerInfo>)
		return INFO_TYPE::PLAYER;
	else if (std::is_same_v<T, ZombieInfo>)
		return INFO_TYPE::ZOMBIE;
	else
		return INFO_TYPE::NONE;
}
