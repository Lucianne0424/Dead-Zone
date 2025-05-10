#include "pch.h"
#include "BloodParticle.h"
#include "Resources.h"
#include "Timer.h"
#include "StructuredBuffer.h"

BloodParticle::BloodParticle()
{
	_particleBuffer = make_shared<StructuredBuffer>();
	_particleBuffer->Init(sizeof(ParticleInfo), _maxParticle);

	_computeSharedBuffer = make_shared<StructuredBuffer>();
	_computeSharedBuffer->Init(sizeof(ComputeSharedInfo), 1);

	// 파라미터 설정
	SetMaxParticle(20);
	SetLifeTime(0.2f, 0.5f);
	SetSpeed(20.0f, 50.f);
	SetScale(5.f, 2.5f);
	SetlifeTime(0.1f);
	SetCreateInterval(0.005f);
	SetParticleType(PARTICLE_TYPE::BLOOD);
	

	// 텍스처 설정
	shared_ptr<Texture> tex = GET_SINGLE(Resources)->Load<Texture>(
		L"Fire", L"..\\Resources\\Texture\\Particle\\Blood.png");
	SetTexture(tex);
}

void BloodParticle::FinalUpdate()
{
	_elapsedTime += DELTA_TIME;

	int32 add = 0;
	if (_isActive && _lifeTime > 0.0f && _elapsedTime >= _lifeTime)
	{
		_isActive = false;
	}

	if (_isActive)
	{
		_accTime += DELTA_TIME;

		if (_createInterval < _accTime)
		{
			_accTime -= _createInterval;
			add = 5;
		}
	}

	_particleBuffer->PushComputeUAVData(UAV_REGISTER::u0);
	_computeSharedBuffer->PushComputeUAVData(UAV_REGISTER::u1);

	_computeMaterial->SetInt(0, _maxParticle);
	_computeMaterial->SetInt(1, add);
	_computeMaterial->SetInt(2, _type);

	_computeMaterial->SetVec2(1, Vec2(DELTA_TIME, _accTime));
	_computeMaterial->SetVec4(0, Vec4(_minLifeTime, _maxLifeTime, _minSpeed, _maxSpeed));


	_computeMaterial->Dispatch(1, 1, 1);
}
