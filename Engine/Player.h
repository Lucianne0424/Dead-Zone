#pragma once
#include "MonoBehaviour.h"
#include "GameInfo.h"

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

