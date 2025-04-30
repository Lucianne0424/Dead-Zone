#include "pch.h"
#include "TestCameraScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"
#include "protocol.h""

static Vec3 Normalize(const Vec3& v)
{
	float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	if (len > 1e-6f)
		return Vec3{ v.x / len, v.y / len, v.z / len };
	else
		return Vec3{ 0.f, 0.f, 0.f };
}

extern WindowInfo GWindowInfo;
TestCameraScript::TestCameraScript()
{
}

TestCameraScript::~TestCameraScript()
{
}

void TestCameraScript::LateUpdate()
{
	Vec3 pos = GetTransform()->GetLocalPosition();
	Vec3 moveDir = { 0.f, 0.f, 0.f };

	if (INPUT->GetButton(KEY_TYPE::W))
		moveDir += GetTransform()->GetLook();
	if (INPUT->GetButton(KEY_TYPE::S))
		moveDir -= GetTransform()->GetLook();
	if (INPUT->GetButton(KEY_TYPE::A))
		moveDir -= GetTransform()->GetRight();
	if (INPUT->GetButton(KEY_TYPE::D))
		moveDir += GetTransform()->GetRight();

	if (moveDir.x != 0.f || moveDir.y != 0.f || moveDir.z != 0.f)
	{
		Vec3 dirNorm = Normalize(moveDir);

		pos += dirNorm * _speed * DELTA_TIME;
		GetTransform()->SetLocalPosition(pos);

		cs_packet_move pkt;
		pkt.size = sizeof(cs_packet_move);
		pkt.type = C2S_P_MOVE;
		pkt.direction.x = dirNorm.x;
		pkt.direction.y = dirNorm.y;
		pkt.direction.z = dirNorm.z;
		pkt.yaw = GetTransform()->GetLocalRotation().y;

		send(GWindowInfo.sock,
			reinterpret_cast<char*>(&pkt),
			sizeof(pkt), 0);
	}

	if (INPUT->GetButton(KEY_TYPE::Q))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.x += DELTA_TIME * 15.0f;
		GetTransform()->SetLocalRotation(rotation);
	}

	if (INPUT->GetButton(KEY_TYPE::E))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.x -= DELTA_TIME * 15.0f;
		GetTransform()->SetLocalRotation(rotation);
	}

	if (INPUT->GetButton(KEY_TYPE::Z))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.y += DELTA_TIME * 15.0f;
		GetTransform()->SetLocalRotation(rotation);
	}

	if (INPUT->GetButton(KEY_TYPE::C))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.y -= DELTA_TIME * 15.0f;
		GetTransform()->SetLocalRotation(rotation);
	}

	if (INPUT->GetButtonDown(MOUSE_TYPE::RBUTTON))
	{
		const POINT& pos = INPUT->GetMousePos();
		GET_SINGLE(SceneManager)->Pick(pos.x, pos.y);
	}

	if (INPUT->GetButtonDown(MOUSE_TYPE::LBUTTON))
	{
		const POINT& pos = INPUT->GetMousePos();
		_mousePos.x = pos.x;
		_mousePos.y = pos.y;
	}

	if (INPUT->GetButton(MOUSE_TYPE::LBUTTON))
	{
		const POINT& pos = INPUT->GetMousePos();
		Vec3 rotation = GetTransform()->GetLocalRotation();

		float dx = (pos.x - _mousePos.x);
		float dy = (pos.y - _mousePos.y);

		rotation.y += dx * DELTA_TIME * 30.f;
		rotation.x += dy * DELTA_TIME * 30.f;

		_mousePos.x = pos.x;
		_mousePos.y = pos.y;

		GetTransform()->SetLocalRotation(rotation);
	}

	GetTransform()->SetLocalPosition(pos);
}