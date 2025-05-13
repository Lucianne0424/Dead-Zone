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
#include "Scene.h"
#include "protocol.h"
#include "Scene.h"
#include "ParticleSystem.h"
#include "Gun.h"
#include "M4A1.h"
#include "AK47.h"
#include "Zombie.h"

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
	POINT deltapos = INPUT->GetDeltaPos();
	bool isMoving = (_moveDir.x != 0.f || _moveDir.y != 0.f || _moveDir.z != 0.f);
	bool isAttacking = INPUT->GetButton(MOUSE_TYPE::LBUTTON);

	if (isMoving || deltapos.x != 0.f || deltapos.y != 0.f)
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
		pkt.look.x = GetTransform()->GetLook().x;
		pkt.look.y = GetTransform()->GetLook().y;
		pkt.look.z = GetTransform()->GetLook().z;

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
		stopPkt.look.x = GetTransform()->GetLook().x;
		stopPkt.look.y = GetTransform()->GetLook().y;
		stopPkt.look.z = GetTransform()->GetLook().z;
		
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

	uint8_t newState = static_cast<uint8_t>(PLAYER_STATE::IDLE);
	if (isAttacking && !isMoving) {
		newState = static_cast<uint8_t>(PLAYER_STATE::FIRE);
	}
	else if (isMoving) {
			_moveDir.Normalize();
			Vec3 forward = GetTransform()->GetLook();
			Vec3 right = GetTransform()->GetRight();
			float fwd = _moveDir.x * forward.x + _moveDir.y * forward.y + _moveDir.z * forward.z;
			float rgt = _moveDir.x * right.x + _moveDir.y * right.y + _moveDir.z * right.z;
			if (fwd > 0.5f)             newState = static_cast<uint8_t>(PLAYER_STATE::RUN_FORWARD);
			else if (fwd < -0.5f)        newState = static_cast<uint8_t>(PLAYER_STATE::RUN_BACKWARD);
			else if (rgt > 0.5f)        newState = static_cast<uint8_t>(PLAYER_STATE::RUN_RIGHT);
			else if (rgt < -0.5f)        newState = static_cast<uint8_t>(PLAYER_STATE::RUN_LEFT);
			else                         newState = static_cast<uint8_t>(PLAYER_STATE::IDLE);
	}
	else {
		newState = static_cast<uint8_t>(PLAYER_STATE::IDLE);
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
	if (INPUT->GetButton(MOUSE_TYPE::LBUTTON))
	{
		shared_ptr<GameObject> obj = GET_SINGLE(SceneManager)->PickZombie(GEngine->GetWindow().width / 2, GEngine->GetWindow().height / 2);

		if (obj) {
			cs_packet_attack atkPkt{};
			atkPkt.size = sizeof(atkPkt);
			atkPkt.type = C2S_P_ATTACK;
			atkPkt.zombieId = obj ? static_cast<long long>(obj->GetID()) : -1;
			send(GEngine->GetWindow().sock,
				reinterpret_cast<char*>(&atkPkt),
				sizeof(atkPkt),
				0);
		}
	}

	if (INPUT->GetButton(KEY_TYPE::C))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.y -= DELTA_TIME * 15.0f;
		GetTransform()->SetLocalRotation(rotation);
	}

	if (INPUT->GetButtonDown(KEY_TYPE::F))
	{
		// 테스트용 임시로 대충 만듦
		_GunType = (_GunType + 1) % _MaxGunType;
		if (_GunType == 0)
		{
			GET_SINGLE(SceneManager)->GetActiveScene()->FindGameObject(L"M4A1")->SetActive(true);
			GET_SINGLE(SceneManager)->GetActiveScene()->FindGameObject(L"AK47")->SetActive(false);

			auto gun = GET_SINGLE(SceneManager)->GetActiveScene()->FindGameObject(L"M4A1");
			gun->SetActive(true);
			Vec3 pos = static_pointer_cast<M4A1>(gun->GetMonoBehaviour(L"M4A1"))->GetNomalParticlePos();
			static_pointer_cast<M4A1>(gun->GetMonoBehaviour(L"M4A1"))->setParticlePos(pos);

		}
		else if (_GunType == 1)
		{
			GET_SINGLE(SceneManager)->GetActiveScene()->FindGameObject(L"M4A1")->SetActive(false);
			auto gun = GET_SINGLE(SceneManager)->GetActiveScene()->FindGameObject(L"AK47");
			gun->SetActive(true);
			Vec3 pos = static_pointer_cast<AK47>(gun->GetMonoBehaviour(L"AK47"))->GetNomalParticlePos();
			static_pointer_cast<AK47>(gun->GetMonoBehaviour(L"AK47"))->setParticlePos(pos);
		}
	}

	if (INPUT->GetButtonDown(MOUSE_TYPE::LBUTTON))
	{
		shared_ptr<GameObject> obj = GET_SINGLE(SceneManager)->Pick(GEngine->GetWindow().width / 2, GEngine->GetWindow().height / 2);
		
		
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