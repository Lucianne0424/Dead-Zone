#pragma once
#include "Gun.h"

class M4A1 : public Gun
{
public:
	M4A1();
	virtual ~M4A1();

	virtual void Awake() override;
	//virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

private:
	static Vec3 _basePosition;
	static Vec3 _baseRotation;
	static Vec3 _baseScale;
};

