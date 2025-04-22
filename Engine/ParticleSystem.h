#pragma once
#include "Component.h"
#include "Material.h"

class Material;
class Mesh;
class StructuredBuffer;
class Texture;

struct ParticleInfo
{
	Vec3	worldPos;
	float	curTime;
	Vec3	worldDir;
	float	lifeTime;
	int32	alive;
	int32	padding[3];
};

struct ComputeSharedInfo
{
	int32 addCount;
	int32 padding[3];
};

class ParticleSystem : public Component
{
public:
	ParticleSystem();
	virtual ~ParticleSystem();

public:
	virtual void FinalUpdate();
	void Render();

public:
	virtual void Load(const wstring& path) override { }
	virtual void Save(const wstring& path) override { }

	void SetMaxParticle(uint32 max) { _maxParticle = max; }
	void SetLifeTime(float min, float max) { _minLifeTime = min; _maxLifeTime = max; }
	void SetSpeed(float min, float max) { _minSpeed = min; _maxSpeed = max; }
	void SetScale(float start, float end) { _startScale = start; _endScale = end; }
	void SetCreateInterval(float interval) { _createInterval = interval; }
	void SetTexture(shared_ptr<Texture> tex) { _material->SetTexture(0, tex); }

	void EmitOnce();

protected:
	shared_ptr<StructuredBuffer>	_particleBuffer;
	shared_ptr<StructuredBuffer>	_computeSharedBuffer;
	uint32							_maxParticle = 1000;

	shared_ptr<Material>		_computeMaterial;
	shared_ptr<Material>		_material;
	shared_ptr<Mesh>			_mesh;

	float				_createInterval = 0.005f;
	float				_accTime = 0.f;

	float				_minLifeTime = 0.5f;
	float				_maxLifeTime = 1.f;
	float				_minSpeed = 100;
	float				_maxSpeed = 50;
	float				_startScale = 10.f;
	float				_endScale = 5.f;

	bool _emitRequested = false;
	bool _autoDestruct = false;


};
