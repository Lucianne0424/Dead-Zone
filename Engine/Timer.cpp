#include "pch.h"
#include "Timer.h"

void Timer::Init()
{
	::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&_frequency));
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&_prevCount)); // CPU Ŭ��
}

void Timer::Update()
{
	uint64 currentCount;
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));

	_deltaTime = (currentCount - _prevCount) / static_cast<float>(_frequency);
	_prevCount = currentCount;

	// Ÿ�Ӿƿ� �½�ũ ������Ʈ
	for (auto it = _timeoutTasks.begin(); it != _timeoutTasks.end(); )
	{
		it->timeLeft -= _deltaTime;
		if (it->timeLeft <= 0.f)
		{
			it->callback();          // �ð� ���� �ݹ� ����
			it = _timeoutTasks.erase(it);
		}
		else
		{
			++it;
		}
	}

	_frameCount++;
	_frameTime += _deltaTime;

	if (_frameTime > 1.f)
	{
		_fps = static_cast<uint32>(_frameCount / _frameTime);

		_frameTime = 0.f;
		_frameCount = 0;
	}
}

void Timer::SetTimeout(const std::function<void()>& callback, float delay)
{
	_timeoutTasks.push_back(TimeoutTask{ callback, delay });
}