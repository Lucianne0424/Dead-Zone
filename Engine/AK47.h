#pragma once
#include "Gun.h"

class AK47 : public Gun
{
public:
	AK47();
	virtual ~AK47();

	virtual void Awake() override;
	//virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

	Vec3 GetNomalParticlePos() { return _ParticlePosition; }

private:
	static Vec3 _basePosition;
	static Vec3 _baseRotation;
	static Vec3 _baseScale;
	static Vec3 _aimingPosition;
	static Vec3 _ParticlePosition;
};

