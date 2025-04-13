#pragma once
#include "MonoBehaviour.h"

enum class ZOMBIE_TYPE : uint8
{
	NORMAL,

	END
};

struct ZombieInfo
{
	uint32 level;
	uint32 hp;
	uint32 attack;
	uint32 defense;
};

class Zombie : public MonoBehaviour
{
public:
	Zombie();
	virtual ~Zombie();

	virtual void Awake() override;
	//virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

private:

};

