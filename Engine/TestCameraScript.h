#pragma once
#include "MonoBehaviour.h"

class TestCameraScript : public MonoBehaviour
{
public:
	TestCameraScript();
	virtual ~TestCameraScript();

	virtual void LateUpdate() override;

	void Recoil(float pitchAmount, float yawAmount);

	void setSensitivity(float sensitivity) { _sensitivity = sensitivity; }
	float getSensitivity() { return _sensitivity; }


private:
	float		_speed = 300.f;

	Vec2		_mousePos = { 0.f, 0.f };

	float _sensitivity = 30.f;		// ���콺 ����
	float _recoilPitch = 0.f;		// ���Ʒ� ��鸲
	float _recoilYaw = 0.f;			// �¿� ��鸲
	float _mousePitch = 0.f;		// ���콺 ��ġ (ȸ��)
	float _mouseYaw = 0.f;			// ���콺 �� (ȸ��)

};

