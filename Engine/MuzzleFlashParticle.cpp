#include "pch.h"
#include "MuzzleFlashParticle.h"
#include "Resources.h"
#include "Timer.h"
#include "StructuredBuffer.h"

MuzzleFlashParticle::MuzzleFlashParticle()
{
	_particleBuffer = make_shared<StructuredBuffer>();
	_particleBuffer->Init(sizeof(ParticleInfo), _maxParticle);

	_computeSharedBuffer = make_shared<StructuredBuffer>();
	_computeSharedBuffer->Init(sizeof(ComputeSharedInfo), 1);

	// 파라미터 설정
	SetMaxParticle(1);
	SetLifeTime(0.05f, 0.15f);
	SetSpeed(0.f, 0.f);
	SetScale(5.f, 3.f);
	//SetlifeTime(1.0f);
	//SetCreateInterval(0.001f);

	// 텍스처 설정
	shared_ptr<Texture> tex = GET_SINGLE(Resources)->Load<Texture>(
		L"Fire", L"..\\Resources\\Texture\\Particle\\Fire.png");
	SetTexture(tex);
}

void MuzzleFlashParticle::FinalUpdate()
{
	_accTime += DELTA_TIME;
	int32 add = 0;
	if (_isActive)
	{
		add = 1;
		_isActive = false;
	}
	
	_particleBuffer->PushComputeUAVData(UAV_REGISTER::u0);
	_computeSharedBuffer->PushComputeUAVData(UAV_REGISTER::u1);

	_computeMaterial->SetInt(0, _maxParticle);
	_computeMaterial->SetInt(1, add);

	_computeMaterial->SetVec2(1, Vec2(DELTA_TIME, _accTime));
	_computeMaterial->SetVec4(0, Vec4(_minLifeTime, _maxLifeTime, _minSpeed, _maxSpeed));

	_computeMaterial->Dispatch(1, 1, 1);
}