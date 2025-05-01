#pragma once

class GameObject;

class ParticlePool
{
	DECLARE_SINGLE(ParticlePool);
public:
	void Init(uint32 maxCount);
	shared_ptr<GameObject> Get();
	void Return(shared_ptr<GameObject> obj);
	shared_ptr<GameObject> GetFromPool();

private:
	vector<shared_ptr<GameObject>> _pool;
	uint32 _curIndex = 0;
};
