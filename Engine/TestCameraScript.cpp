#include "pch.h"
#include "TestCameraScript.h"
#include "Engine.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"
#include "MultiPlayer.h" 
#include "protocol.h"


template<typename T>
T Lerp(const T& a, const T& b, float t)
{
	return a * (1.0f - t) + b * t;
}

TestCameraScript::TestCameraScript()
{
	_name = L"MainCamera";
}

TestCameraScript::~TestCameraScript()
{
}

void TestCameraScript::LateUpdate()
{
	/*static bool bInitialized = false;
	if (!bInitialized) {
		Vec3 spawn{ 1185.0f, 140.0f, 473.0f };
		GetTransform()->SetLocalPosition(spawn);
		bInitialized = true;
	}*/

	constexpr float MAP_MIN_X = 237.0f;
	constexpr float MAP_MAX_X = 2030.0f;
	constexpr float MAP_MIN_Z = -3552.0f;
	constexpr float MAP_MAX_Z = 3535.0f;
	constexpr float MAP_MIN_Y = 140.0f;
	constexpr float MAP_MAX_Y =  960.0f;

	constexpr float PLAYER_RADIUS = 10.0f;

	static bool  localJumping = false;
	static float localVerticalVelocity = 0.0f;
	const float  gravity = 9.8f;

	Vec3 pos = GetTransform()->GetLocalPosition();
	_moveDir = { 0.f, 0.f, 0.f };

	// 입력 처리
	ProcessKeyInput();
	ProcessMouseInput();

	static bool wasMovingLastFrame = false;
	bool isMoving = (_moveDir.x != 0.f || _moveDir.y != 0.f || _moveDir.z != 0.f);

	if (_moveDir.x != 0.f || _moveDir.y != 0.f || _moveDir.z != 0.f)
	{
		_moveDir.Normalize();

		pos += _moveDir * _speed * DELTA_TIME;
		GetTransform()->SetLocalPosition(pos);

		cs_packet_move pkt;
		pkt.size = sizeof(cs_packet_move);
		pkt.type = C2S_P_MOVE;
		pkt.direction.x = _moveDir.x;
		pkt.direction.y = _moveDir.y;
		pkt.direction.z = _moveDir.z;
		pkt.look.x = GetTransform()->GetUp().x;
		pkt.look.y = GetTransform()->GetUp().y;
		pkt.look.z = GetTransform()->GetUp().z;

		send(GEngine->GetWindow().sock,
			reinterpret_cast<char*>(&pkt),
			sizeof(pkt), 0);
	}
	else if (wasMovingLastFrame)
	{
		cs_packet_move stopPkt{};
		stopPkt.size = sizeof(stopPkt);
		stopPkt.type = C2S_P_MOVE;
		stopPkt.direction.x = 0.f;
		stopPkt.direction.y = 0.f;
		stopPkt.direction.z = 0.f;
		stopPkt.look.x = GetTransform()->GetUp().x;
		stopPkt.look.y = GetTransform()->GetUp().y;
		stopPkt.look.z = GetTransform()->GetUp().z;
		
		send(GEngine->GetWindow().sock,
			reinterpret_cast<char*>(&stopPkt),
			sizeof(stopPkt), 0);
	}
	wasMovingLastFrame = isMoving;

	if (INPUT->GetButtonDown(KEY_TYPE::SPACE) && !localJumping) {
		localJumping = true;
		const float jumpSpeed = 12.0f;
		localVerticalVelocity = jumpSpeed;
		cs_packet_jump pkt{};
		pkt.size = sizeof(cs_packet_jump);
		pkt.type = C2S_P_JUMP;
		pkt.initVelocity = jumpSpeed;
		send(GEngine->GetWindow().sock,
			reinterpret_cast<char*>(&pkt),
			sizeof(pkt),
			0);
	}

	if (localJumping) {
		pos.y += localVerticalVelocity * DELTA_TIME;
		localVerticalVelocity -= gravity * DELTA_TIME;
		if (pos.y <= MAP_MIN_Y) {
			pos.y = MAP_MIN_Y;
			localJumping = false;
		}
	}

	if (pos.x - PLAYER_RADIUS < MAP_MIN_X)
		pos.x = MAP_MIN_X + PLAYER_RADIUS;
	else if (pos.x + PLAYER_RADIUS > MAP_MAX_X)
		pos.x = MAP_MAX_X - PLAYER_RADIUS;

	if (pos.z - PLAYER_RADIUS < MAP_MIN_Z)
		pos.z = MAP_MIN_Z + PLAYER_RADIUS;
	else if (pos.z + PLAYER_RADIUS > MAP_MAX_Z)
		pos.z = MAP_MAX_Z - PLAYER_RADIUS;

	if (pos.y < MAP_MIN_Y)
		pos.y = MAP_MIN_Y;
	// if (pos.y > MAP_MAX_Y) 
	//     pos.y = MAP_MAX_Y;

	uint8_t newState = static_cast<uint8_t>(PlayerState::IDLE);
	if (isMoving) {
		_moveDir.Normalize();
		Vec3 forward = GetTransform()->GetLook();
		Vec3 right = GetTransform()->GetRight();
		float fwd = _moveDir.x * forward.x + _moveDir.y * forward.y + _moveDir.z * forward.z;
		float rgt = _moveDir.x * right.x + _moveDir.y * right.y + _moveDir.z * right.z;
		if (fwd > 0.5f)             newState = static_cast<uint8_t>(PlayerState::RUN_FORWARD);
		else if (fwd < -0.5f)        newState = static_cast<uint8_t>(PlayerState::RUN_BACKWARD);
		else if (rgt > 0.5f)        newState = static_cast<uint8_t>(PlayerState::RUN_RIGHT);
		else if (rgt < -0.5f)        newState = static_cast<uint8_t>(PlayerState::RUN_LEFT);
		else                         newState = static_cast<uint8_t>(PlayerState::IDLE);
	}

	static uint8_t lastState = 255;
	if (newState != lastState) {
		cs_packet_state stPkt{};
		stPkt.size = sizeof(stPkt);
		stPkt.type = C2S_P_STATE;
		stPkt.state = newState;

		send(GEngine->GetWindow().sock,
			reinterpret_cast<char*>(&stPkt),
			stPkt.size, 0);

		lastState = newState;
	}

	GetTransform()->SetLocalPosition(pos);
}

void TestCameraScript::ProcessKeyInput()
{
	// 콜라이더 출력 여부
	if (INPUT->GetButton(KEY_TYPE::KEY_F5))
		SET_DEBUG_MODE(!DEBUG_MODE);

	// 커서 출력 여부
	if (INPUT->GetButton(KEY_TYPE::KEY_F4))
		INPUT->LockCursor(!INPUT->IsCursorLocked());

	if (INPUT->GetButton(KEY_TYPE::W))
	{
		_moveDir += GetTransform()->GetLook();
		_moveDir.y = 0.f;
	}
	if (INPUT->GetButton(KEY_TYPE::S))
	{
		_moveDir -= GetTransform()->GetLook();
		_moveDir.y = 0.f;
	}
	if (INPUT->GetButton(KEY_TYPE::A))
	{
		_moveDir -= GetTransform()->GetRight();
		_moveDir.y = 0.f;
	}
	if (INPUT->GetButton(KEY_TYPE::D))
	{
		_moveDir += GetTransform()->GetRight();
		_moveDir.y = 0.f;
	}
}

void TestCameraScript::ProcessMouseInput()
{
	if (INPUT->GetButtonDown(MOUSE_TYPE::RBUTTON))
	{
		const POINT& pos = INPUT->GetMousePos();
		GET_SINGLE(SceneManager)->Pick(pos.x, pos.y);
	}

	POINT deltaPos = INPUT->GetDeltaPos();

	_mouseYaw += deltaPos.x * DELTA_TIME * _sensitivity;
	_mousePitch += deltaPos.y * DELTA_TIME * _sensitivity;

	// 피치 제한 (위아래)
	_mousePitch = std::clamp(_mousePitch, -89.f, 89.f);

	// 반동 감쇠
	_recoilPitch = Lerp(_recoilPitch, 0.f, 2.f * DELTA_TIME);
	_recoilYaw = Lerp(_recoilYaw, 0.f, 2.f * DELTA_TIME);

	// 최종 회전 = 마우스 + 반동
	float finalPitch = _mousePitch - _recoilPitch;
	float finalYaw = _mouseYaw - _recoilYaw;

	GetTransform()->SetLocalRotation(Vec3(finalPitch, finalYaw, 0.f));
}

void TestCameraScript::Recoil(float pitchAmount, float yawAmount)
{
	// 카메라 반동 설정
	_recoilPitch += pitchAmount;	// 수직
	_recoilYaw += yawAmount;		// 수평
}