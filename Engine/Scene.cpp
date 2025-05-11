#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Camera.h"
#include "Engine.h"
#include "ConstantBuffer.h"
#include "Light.h"
#include "Resources.h"
#include "Timer.h"

#include "Transform.h"
#include "MeshRenderer.h"
#include "MeshData.h"
#include "SphereCollider.h"
#include "MultiPlayer.h"
#include "Zombie.h"
#include "Animator.h"

#include "..//echoserver//protocol.h"

extern WindowInfo GWindowInfo;

void Scene::Awake()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->Awake();
	}
}

void Scene::Start()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->Start();
	}
}

void Scene::Update()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->Update();
	}
}

void Scene::LateUpdate()
{
	const float gravity = 9.8f;

	for (auto& group : _players) {
		auto & root = group[0];
		uint32_t id = root->GetID();
		auto itJS = _jumpStates.find(id);
		if (itJS == _jumpStates.end() || !itJS->second.isJumping)
			 continue;
		auto & js = itJS->second;
		Vec3 pos = root->GetTransform()->GetLocalPosition();
		pos.y += js.verticalVel * DELTA_TIME;
		js.verticalVel -= gravity * DELTA_TIME;
		if (pos.y <= 0.0f) {
			pos.y = 0.0f;
			js.isJumping = false;
			js.verticalVel = 0.0f;
		}
		root->GetTransform()->SetLocalPosition(pos);
	}

	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->LateUpdate();
	}
}

void Scene::FinalUpdate()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->FinalUpdate();
	}
}

shared_ptr<class Camera> Scene::GetMainCamera()
{
	if (_cameras.empty())
		return nullptr;

	return _cameras[0];
}

void Scene::Render()
{
	PushLightData();

	ClearRTV();

	RenderShadow();

	RenderDeferred();

	RenderLights();

	RenderFinal();

	RenderForward();
}

void Scene::RenderUI()
{

}

void Scene::ClearRTV()
{
	// SwapChain Group 초기화
	int8 backIndex = GEngine->GetSwapChain()->GetBackBufferIndex();
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->ClearRenderTargetView(backIndex);
	// Shadow Group 초기화
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->ClearRenderTargetView();
	// Deferred Group 초기화
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->ClearRenderTargetView();
	// Lighting Group 초기화
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->ClearRenderTargetView();
}

void Scene::RenderShadow()
{
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->OMSetRenderTargets();

	for (auto& light : _lights)
	{
		if (light->GetLightType() != LIGHT_TYPE::DIRECTIONAL_LIGHT)
			continue;

		light->RenderShadow();
	}

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->WaitTargetToResource();
}

void Scene::RenderDeferred()
{
	// Deferred OMSet
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->OMSetRenderTargets();

	shared_ptr<Camera> mainCamera = _cameras[0];
	mainCamera->SortGameObject();
	mainCamera->Render_Deferred();

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->WaitTargetToResource();
}

void Scene::RenderLights()
{
	shared_ptr<Camera> mainCamera = _cameras[0];
	Camera::S_MatView = mainCamera->GetViewMatrix();
	Camera::S_MatProjection = mainCamera->GetProjectionMatrix();

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->OMSetRenderTargets();

	// 광원을 그린다.
	for (auto& light : _lights)
	{
		light->Render();
	}

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->WaitTargetToResource();
}

void Scene::RenderFinal()
{
	// Swapchain OMSet
	int8 backIndex = GEngine->GetSwapChain()->GetBackBufferIndex();
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->OMSetRenderTargets(1, backIndex);

	GET_SINGLE(Resources)->Get<Material>(L"Final")->PushGraphicsData();
	GET_SINGLE(Resources)->Get<Mesh>(L"Rectangle")->Render();
}

void Scene::RenderForward()
{
	shared_ptr<Camera> mainCamera = _cameras[0];
	mainCamera->Render_Forward();

	for (auto& camera : _cameras)
	{
		if (camera == mainCamera)
			continue;

		camera->SortGameObject();
		camera->Render_Forward();
	}
}

void Scene::PushLightData()
{
	LightParams lightParams = {};

	for (auto& light : _lights)
	{
		const LightInfo& lightInfo = light->GetLightInfo();

		light->SetLightIndex(lightParams.lightCount);

		lightParams.lights[lightParams.lightCount] = lightInfo;
		lightParams.lightCount++;
	}

	CONST_BUFFER(CONSTANT_BUFFER_TYPE::GLOBAL)->SetGraphicsGlobalData(&lightParams, sizeof(lightParams));
}

void Scene::AddGameObject(shared_ptr<GameObject> gameObject)
{
	if (gameObject->GetCamera() != nullptr)
	{
		_cameras.push_back(gameObject->GetCamera());
	}
	else if (gameObject->GetLight() != nullptr)
	{
		_lights.push_back(gameObject->GetLight());
	}

	_gameObjects.push_back(gameObject);
}

void Scene::RemoveGameObject(shared_ptr<GameObject> gameObject)
{
	if (gameObject->GetCamera())
	{
		auto findIt = std::find(_cameras.begin(), _cameras.end(), gameObject->GetCamera());
		if (findIt != _cameras.end())
			_cameras.erase(findIt);
	}
	else if (gameObject->GetLight())
	{
		auto findIt = std::find(_lights.begin(), _lights.end(), gameObject->GetLight());
		if (findIt != _lights.end())
			_lights.erase(findIt);
	}

	auto findIt = std::find(_gameObjects.begin(), _gameObjects.end(), gameObject);
	if (findIt != _gameObjects.end())
		_gameObjects.erase(findIt);
}

shared_ptr<GameObject> Scene::FindGameObject(const wstring& name)
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		if (gameObject->GetName() == name)
			return gameObject;
	}

	return nullptr;
}

void Scene::AddPlayer(sc_packet_player_info* packet)
{
	//if (_players.size() >= 2)
		//return;

	Vec3 position = Vec3(packet->position.x, packet->position.y, packet->position.z);

	shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Soldado.fbx");

	vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate(ColliderType::OBB);

	for (auto& gameObject : gameObjects)
	{
		gameObject->SetName(L"Player");
		gameObject->AddComponent(make_shared<MultiPlayer>());
		shared_ptr<MultiPlayer> playerScript = static_pointer_cast<MultiPlayer>(gameObject->GetMonoBehaviour(L"MultiPlayer"));
		playerScript->SetState(PLAYER_STATE::IDLE);
		AddGameObject(gameObject);
	}

	gameObjects[0]->SetID(static_cast<uint32_t>(packet->playerId));
	gameObjects[0]->GetTransform()->SetLocalPosition(position);
	gameObjects[0]->GetTransform()->SetLocalRotation(Vec3(-90.0f, 180.f, 0.0f));

	for (int i = 1; i < gameObjects.size(); i++)
	{
		gameObjects[i]->GetTransform()->SetParent(gameObjects[0]->GetTransform());
	}
	_players.push_back(gameObjects);
}

void Scene::RemovePlayer(sc_packet_player_leave* packet)
{
	uint32_t leftId = static_cast<uint32_t>(packet->playerId);

	for (auto it = _players.begin(); it != _players.end(); ++it) {
		auto & root = (*it)[0];
		if (root->GetID() == leftId) {
			for (auto& part : *it)
				 RemoveGameObject(part);
			_players.erase(it);
			break;
		}
	}
}

void Scene::AnimatePlayer(sc_packet_state* packet)
{
	PLAYER_STATE state = static_cast<PLAYER_STATE>(packet->state);
	for (auto& group : _players) {
		auto& root = group[0];
		if (root->GetID() == packet->playerId) {
			for (auto& part : group) {
				auto mp = static_pointer_cast<MultiPlayer>(part->GetMonoBehaviour(L"MultiPlayer"));
				if (mp)
					mp->SetState(state);
			}
			return;
		}
	}
}

void Scene::AnimateZombie(sc_packet_zombie_state* packet)
{
	ZOMBIE_STATE state = static_cast<ZOMBIE_STATE>(packet->state);
	for (auto& group : _zombies) {
		auto& root = group[0];
		if (root->GetID() == packet->zombieId) {
			for (auto& part : group) {
				auto mp = static_pointer_cast<Zombie>(part->GetMonoBehaviour(L"Zombie"));
				if (mp)
					mp->SetState(state);
			}
			return;
		}
	}
}

void Scene::MovePlayer(sc_packet_move* packet)
{
	Vec3 position = Vec3(packet->position.x, packet->position.y, packet->position.z); 
	Vec3 look = Vec3(packet->look.x, packet->look.y, packet->look.z);

	for (auto& group : _players) {
		auto & root = group[0];
		if (root->GetID() == packet->playerId) {
			shared_ptr<Transform> rootTransform = root->GetTransform();
			rootTransform->SetLocalPosition(position);
			rootTransform->LookAt(look);

			Vec3 rotation = rootTransform->GetLocalRotation();
			rotation.x = -90.f;
			rotation.y += 180.f;
			rootTransform->SetLocalRotation(rotation);
			return;
		}
	}
}

void Scene::MoveZombie(sc_packet_zombie_move* packet)
{
	Vec3 position = Vec3(packet->position.x, packet->position.y, packet->position.z);
	Vec3 look = Vec3(packet->dx, 0.f, packet->dz);
	uint32_t zid = static_cast<uint32_t>(packet->zombieId);

	for (auto& group : _zombies) {
		auto& root = group[0];
		if (root->GetID() == zid) {
			shared_ptr<Transform> rootTransform = root->GetTransform();
			rootTransform->SetLocalPosition(position);
			rootTransform->LookAt(look);

			Vec3 rotation = rootTransform->GetLocalRotation();
			rotation.x = -90.f;
			rotation.y += 180.f;
			rootTransform->SetLocalRotation(rotation);
			return;
		}
	}
}

void Scene::JumpPlayer(sc_packet_jump* packet)
{
	uint32_t id = static_cast<uint32_t>(packet->playerId);
	auto& js = _jumpStates[id];
	js.isJumping = true;
	js.verticalVel = packet->initVelocity;
}

void Scene::LandPlayer(sc_packet_land* packet)
{
	uint32_t id = static_cast<uint32_t>(packet->playerId);
	auto& js = _jumpStates[id];
	js.isJumping = false;
	js.verticalVel = 0.0f;
}

void Scene::ApplySnapshot(sc_packet_snapshot* packet)
{
	for (int i = 0; i < packet->count; ++i) {
		auto& e = packet->entries[i];
		uint32_t id = static_cast<uint32_t>(e.playerId);

		if (id == GEngine->GetWindow().local) {
			auto cam = GetMainCamera();
			auto camGO = cam->GetGameObject();
			auto camTrans = camGO->GetTransform();

			Vec3 predicted = camTrans->GetLocalPosition();
			Vec3 serverPos{ e.position.x, e.position.y+140.f, e.position.z };

			Vec3 delta = serverPos - predicted;
			const float snapThreshold = 1.0f;   
			const float lerpRatio = 0.1f;   

			if (delta.Length() > snapThreshold) {
				camTrans->SetLocalPosition(serverPos);
			}
			else {
				camTrans->SetLocalPosition(predicted + delta * lerpRatio);
			}
			continue;  
		}

		shared_ptr<GameObject> rootObj;
		for (auto& group : _players) {
			if (group[0]->GetID() == id) {
				rootObj = group[0];
				break;
			}
		}
		if (!rootObj) continue;

		auto itJS = _jumpStates.find(id);
		bool remoteJumping = (itJS != _jumpStates.end() && itJS->second.isJumping);

		Vec3 pos = rootObj->GetTransform()->GetLocalPosition();

		pos.x = e.position.x;
		pos.z = e.position.z;

		if (!remoteJumping) {
			pos.y = e.position.y;
		}
		rootObj->GetTransform()->SetLocalPosition(pos);

		// 3) 회전도 보정
		/*Vec3 rot = obj->GetTransform()->GetLocalRotation();
		rot.y = e.yaw;
		obj->GetTransform()->SetLocalRotation(rot);*/
	}
}

void Scene::ClearPlayers()
{
	for (auto& group : _players) {
		for (auto& obj : group) {
			RemoveGameObject(obj);
		}
	}
	_players.clear();
}

void Scene::AddZombie(sc_packet_spawn_zombie* packet)
{
	Vec3 position = Vec3(packet->position.x, packet->position.y, packet->position.z);

	shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\NormalZombie.fbx");

	vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate(ColliderType::OBB);

	for (auto& gameObject : gameObjects)
	{
		gameObject->SetName(L"Zombie");
		gameObject->AddComponent(make_shared<Zombie>());
		shared_ptr<Zombie> playerScript = static_pointer_cast<Zombie>(gameObject->GetMonoBehaviour(L"Zombie"));
		playerScript->SetState(ZOMBIE_STATE::IDLE);
		AddGameObject(gameObject);
	}

	gameObjects[0]->SetID(static_cast<uint32_t>(packet->zombieId));
	gameObjects[0]->GetTransform()->SetLocalPosition(position);
	gameObjects[0]->GetTransform()->SetLocalRotation(Vec3(-90.0f, 180.f, 0.0f));

	for (int i = 1; i < gameObjects.size(); i++)
	{
		gameObjects[i]->GetTransform()->SetParent(gameObjects[0]->GetTransform());
	}
	_zombies.push_back(gameObjects);
}

void Scene::DieZombie(sc_packet_zombie_die* pkt)
{
	uint32_t id = static_cast<uint32_t>(pkt->zombieId);

	// _zombies: vector< vector< shared_ptr<GameObject> > >
	for (auto& group : _zombies) {
		if (group.empty() || group[0]->GetID() != id)
			continue;

		// 1) 각 파트에 대해 DIE 상태로 전환 → 애니메이션 재생
		for (auto& part : group) {
			auto zComp = static_pointer_cast<Zombie>(part->GetMonoBehaviour(L"Zombie"));
			if (zComp) {
				zComp->SetState(ZOMBIE_STATE::DIE);
			}
		}

		auto anim = group[0]->GetAnimator();
		int32 idx = anim->GetCurrentClipIndex();
		float deathAnimDuration = anim->GetAnimDuration(idx);
		GET_SINGLE(Timer)->SetTimeout([this, id]() {
			RemoveZombieById(id);
			}, deathAnimDuration);

		// 한 번 처리했으면 루프 탈출
		break;
	}
}

// 기존 RemoveZombieById는 그대로 사용
void Scene::RemoveZombieById(uint32_t zombieId)
{
	for (auto it = _zombies.begin(); it != _zombies.end(); ++it) {
		auto& group = *it;
		if (group.empty() || group[0]->GetID() != zombieId)
			continue;

		// 씬 오브젝트에서 제거
		for (auto& part : group) {
			RemoveGameObject(part);
		}
		// 벡터에서 그룹 삭제
		_zombies.erase(it);
		return;
	}
}