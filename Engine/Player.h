#pragma once
#include "MonoBehaviour.h"

struct PlayerInfo
{
	uint32 id;
	string name;
	uint32 level;
	uint32 hp;
	uint32 attack;
	uint32 defense;
};

class Player : public MonoBehaviour
{
public:
	Player();
	virtual ~Player();

	virtual void Awake() override;
	//virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	
private:
	PlayerInfo info;
};

