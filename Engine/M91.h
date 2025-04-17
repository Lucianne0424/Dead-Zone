#pragma once
#include "Gun.h"

class M91 : public Gun
{
public:
	M91();
	virtual ~M91();

	virtual void Awake() override;
	//virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

private:
	static Vec3 _basePosition;
	static Vec3 _baseRotation;
	static Vec3 _baseScale;
};

