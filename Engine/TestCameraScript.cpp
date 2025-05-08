#include "pch.h"
#include "TestCameraScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"
#include "MultiPlayer.h" 
#include "protocol.h"

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
	static bool bInitialized = false;
	if (!bInitialized) {
		Vec3 spawn{ 1185.0f, 192.0f, 473.0f };
		GetTransform()->SetLocalPosition(spawn);
		bInitialized = true;
	}

	constexpr float MAP_MIN_X = 237.0f;
	constexpr float MAP_MAX_X = 2030.0f;
	constexpr float MAP_MIN_Z = -3552.0f;
	constexpr float MAP_MAX_Z = 3535.0f;
	constexpr float MAP_MIN_Y = 10.0f;
	constexpr float MAP_MAX_Y = 960.0f;

	constexpr float PLAYER_RADIUS = 0.5f;

	if (INPUT->GetButton(KEY_TYPE::KEY_F5))
		SET_DEBUG_MODE(!DEBUG_MODE);

	static bool   localJumping = false;
	static float localVerticalVelocity = 0.0f;
	const float  gravity = 9.8f;

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

	static bool wasMovingLastFrame = false;
	bool isMoving = (moveDir.x != 0.f || moveDir.y != 0.f || moveDir.z != 0.f);

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
	else if (wasMovingLastFrame)
	{
		cs_packet_move stopPkt{};
		stopPkt.size = sizeof(stopPkt);
		stopPkt.type = C2S_P_MOVE;
		stopPkt.direction.x = 0.f;
		stopPkt.direction.y = 0.f;
		stopPkt.direction.z = 0.f;
		stopPkt.yaw = GetTransform()->GetLocalRotation().y;
		send(GWindowInfo.sock,
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
		send(GWindowInfo.sock,
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
		Vec3 dirNorm = Normalize(moveDir);
		Vec3 forward = Normalize(GetTransform()->GetLook());
		Vec3 right = Normalize(GetTransform()->GetRight());
		float fwd = dirNorm.x * forward.x + dirNorm.y * forward.y + dirNorm.z * forward.z;
		float rgt = dirNorm.x * right.x + dirNorm.y * right.y + dirNorm.z * right.z;
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
		stPkt.playerId = GWindowInfo.local;
		stPkt.state = newState;
		send(GWindowInfo.sock,
			reinterpret_cast<char*>(&stPkt),
			stPkt.size, 0);
		lastState = newState;
	}

	GetTransform()->SetLocalPosition(pos);
}