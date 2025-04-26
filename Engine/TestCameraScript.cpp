#include "pch.h"
#include "TestCameraScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"
#include "protocol.h""
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
	Vec3 moveDirection = { 0.f, 0.f, 0.f };
	if (INPUT->GetButton(KEY_TYPE::W)) {
		pos += GetTransform()->GetLook() * _speed * DELTA_TIME;
		moveDirection += GetTransform()->GetLook() * _speed * DELTA_TIME;
		// ¿¹½ÃEXAMPLE
		//Vec3 _dir = GetTransform()->GetLook();
		//movePacket.direction.x = _dir.x;
		//movePacket.direction.y = _dir.y;
		//movePacket.direcition.z = _dir.z;
	}
	if (INPUT->GetButton(KEY_TYPE::S)) {
		pos += GetTransform()->GetLook() * _speed * DELTA_TIME;
		moveDirection += GetTransform()->GetLook() * _speed * DELTA_TIME;
	}
	if (INPUT->GetButton(KEY_TYPE::A)) {
		pos -= GetTransform()->GetRight() * _speed * DELTA_TIME;
		moveDirection+= GetTransform()->GetRight() * _speed * DELTA_TIME;
	}
	if (INPUT->GetButton(KEY_TYPE::D)) {
		pos += GetTransform()->GetRight() * _speed * DELTA_TIME;
		moveDirection += GetTransform()->GetRight() * _speed * DELTA_TIME;
	}
	if (moveDirection.x != 0.f || moveDirection.y != 0.f || moveDirection.z != 0.f)
	{
		cs_packet_move movePacket;
		movePacket.size = sizeof(cs_packet_move);
		movePacket.type = C2S_P_MOVE;
		movePacket.direction.x = pos.x;
		movePacket.direction.y = pos.y;
		movePacket.direction.z = pos.z;
		movePacket.yaw = 0.0f;  

		int sendResult = send(
			GWindowInfo.sock,
			reinterpret_cast<char*>(&movePacket),
			sizeof(movePacket),
			0
		);
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