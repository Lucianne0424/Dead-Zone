#pragma once

class GameObject;

struct PlayerInfo
{
	uint32 id;
	string name;
	uint32 level;
	uint32 exp;
	uint32 hp;
	uint32 mp;
	uint32 attack;
	uint32 defense;
};

class Player
{
public:
	Player();
	virtual ~Player();
	
private:
	PlayerInfo info;

	shared_ptr<GameObject> _player;
	shared_ptr<GameObject> _gun;
};

