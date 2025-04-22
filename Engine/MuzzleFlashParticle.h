#pragma once
#include "ParticleSystem.h"

class MuzzleFlashParticle : public ParticleSystem
{
public:
	MuzzleFlashParticle();
	virtual ~MuzzleFlashParticle();

	virtual void FinalUpdate() override;

	void EmitOnce();

private:
	bool _emitRequested = false;
	bool _autoDestruct = false;
};