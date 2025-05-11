#include "pch.h"
#include "Timer.h"

void Timer::Init()
{
	::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&_frequency));
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&_prevCount)); // CPU 클럭
}

void Timer::Update()
{
	uint64 currentCount;
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));

	_deltaTime = (currentCount - _prevCount) / static_cast<float>(_frequency);
	_prevCount = currentCount;

	// 타임아웃 태스크 업데이트
	for (auto it = _timeoutTasks.begin(); it != _timeoutTasks.end(); )
	{
		it->timeLeft -= _deltaTime;
		if (it->timeLeft <= 0.f)
		{
			it->callback();          // 시간 만료 콜백 실행
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