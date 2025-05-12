#pragma once
#include "MonoBehaviour.h"

class TestCameraScript : public MonoBehaviour
{
public:
	TestCameraScript();
	virtual ~TestCameraScript();

	virtual void LateUpdate() override;

	void ProcessKeyInput();
	void ProcessMouseInput();

	void Recoil(float pitchAmount, float yawAmount);
	int32 getGunType() { return _GunType; } // 총기 타입

	void setSensitivity(float sensitivity) { _sensitivity = sensitivity; }
	float getSensitivity() { return _sensitivity; }

private:
	float		_speed = 300.f;

	Vec2		_mousePos = { 0.f, 0.f };
	Vec3		_moveDir = { 0.f, 0.f, 0.f };

	float _sensitivity = 30.f;		// 마우스 감도
	float _recoilPitch = 0.f;		// 위아래 흔들림
	float _recoilYaw = 0.f;			// 좌우 흔들림
	float _mousePitch = 0.f;		// 마우스 피치 (회전)
	float _mouseYaw = 0.f;			// 마우스 요 (회전)

	// 임시
	int32 _GunType = 1; // 총기 타입
	const int32 _MaxGunType = 2; // 총기 타입

};

