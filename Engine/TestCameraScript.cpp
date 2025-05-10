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


template<typename T>
T Lerp(const T& a, const T& b, float t)
{
	return a * (1.0f - t) + b * t;
}


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

	if(INPUT->GetButton(KEY_TYPE::KEY_F5))
		SET_DEBUG_MODE(!DEBUG_MODE);

	if (INPUT->GetButton(KEY_TYPE::KEY_F4))
		INPUT->LockCursor(!INPUT->IsCursorLocked());

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
		pkt.look.x = GetTransform()->GetUp().x;
		pkt.look.y = GetTransform()->GetUp().y;
		pkt.look.z = GetTransform()->GetUp().z;

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
		stopPkt.look.x = GetTransform()->GetUp().x;
		stopPkt.look.y = GetTransform()->GetUp().y;
		stopPkt.look.z = GetTransform()->GetUp().z;
		
		send(GWindowInfo.sock,
			reinterpret_cast<char*>(&stopPkt),
			sizeof(stopPkt), 0);
	}
	wasMovingLastFrame = isMoving;

	if (INPUT->GetButton(KEY_TYPE::S))
		pos -= GetTransform()->GetLook() * _speed * DELTA_TIME;

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

	POINT deltaPos = INPUT->GetDeltaPos();

	_mouseYaw += deltaPos.x * DELTA_TIME * _sensitivity;
	_mousePitch += deltaPos.y * DELTA_TIME * _sensitivity;

	// 피치 제한 (위아래)
	// _mousePitch = std::clamp(_mousePitch, -89.f, 89.f);

	// 반동 감쇠
	_recoilPitch = Lerp(_recoilPitch, 0.f, 2.f * DELTA_TIME);
	_recoilYaw = Lerp(_recoilYaw, 0.f, 2.f * DELTA_TIME);

	// 최종 회전 = 마우스 + 반동
	float finalPitch = _mousePitch - _recoilPitch;
	float finalYaw = _mouseYaw - _recoilYaw;

	GetTransform()->SetLocalRotation(Vec3(finalPitch, finalYaw, 0.f));

	GetTransform()->SetLocalPosition(pos);
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
void TestCameraScript::Recoil(float pitchAmount, float yawAmount)
{
	// 카메라 반동 설정
	_recoilPitch += pitchAmount;	// 수직
	_recoilYaw += yawAmount;		// 수평
}