#pragma once
#include "ParticleSystem.h"

class BloodParticle : public ParticleSystem
{
public:
	BloodParticle();
	virtual ~BloodParticle() = default;

	virtual void FinalUpdate() override;
};

