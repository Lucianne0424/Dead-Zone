#pragma once
#include "ParticleSystem.h"

class MuzzleFlashParticle : public ParticleSystem
{
public:
	MuzzleFlashParticle();
	virtual ~MuzzleFlashParticle() = default;

	virtual void FinalUpdate() override;

private:
};