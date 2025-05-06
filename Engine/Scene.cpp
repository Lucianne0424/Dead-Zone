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
#include "SphereCollider.h"
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
	float gravity = 9.8f;

	for (auto& obj : _players) {
		uint32_t id = obj->GetID();
		auto itJS = _jumpStates.find(id);
		if (itJS == _jumpStates.end() || !itJS->second.isJumping)
			continue;

		auto& js = itJS->second;
		Vec3 pos = obj->GetTransform()->GetLocalPosition();

		pos.y += js.verticalVel * DELTA_TIME;
		js.verticalVel -= gravity * DELTA_TIME;

		if (pos.y <= 10.0f) {
			pos.y = 10.0f;
			js.isJumping = false;
			js.verticalVel = 0.0f;
		}

		obj->GetTransform()->SetLocalPosition(pos);
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

void Scene::AddPlayer(sc_packet_login_ok* packet)
{
	Vec3 position = Vec3(packet->position.x, packet->position.y, packet->position.z);

	shared_ptr<GameObject> obj = make_shared<GameObject>();
	obj->SetID(static_cast<uint32_t>(packet->playerId));
	obj->SetName(L"OBJ");
	obj->AddComponent(make_shared<Transform>());
	obj->AddComponent(make_shared<SphereCollider>());
	obj->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
	obj->GetTransform()->SetLocalPosition(position);
	obj->SetStatic(false);
	shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
	{
		shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
		meshRenderer->SetMesh(sphereMesh);
	}
	{
		shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"GameObject");
		meshRenderer->SetMaterial(material->Clone());
	}
	dynamic_pointer_cast<SphereCollider>(obj->GetCollider())->SetRadius(0.5f);
	dynamic_pointer_cast<SphereCollider>(obj->GetCollider())->SetCenter(Vec3(0.f, 0.f, 0.f));
	obj->AddComponent(meshRenderer);
	AddGameObject(obj);

	_players.push_back(obj);
}

void Scene::RemovePlayer(sc_packet_player_leave* packet)
{
	uint32_t leftId = static_cast<uint32_t>(packet->playerId);

	for (auto it = _players.begin(); it != _players.end(); ++it)
	{
		if ((*it)->GetID() == leftId)
		{
			RemoveGameObject(*it);
			_players.erase(it);
			break;
		}
	}
}

void Scene::MovePlayer(sc_packet_move* packet)
{
	Vec3 position = Vec3(packet->position.x, packet->position.y, packet->position.z); 
	
	auto it = std::find_if(_players.begin(), _players.end(),
		[=](const shared_ptr<GameObject>& player) { return player->GetID() == static_cast<uint32_t>(packet->playerId);
		});

	if (it == _players.end()) {
			return;
	}

	(*it)->GetTransform()->SetLocalPosition(position);
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



void Scene::ApplySnapshot(sc_packet_snapshot* snap)
{
	for (int i = 0; i < snap->count; ++i) {
		auto& e = snap->entries[i];
		uint32_t id = static_cast<uint32_t>(e.playerId);

		if (id == GWindowInfo.local)
			continue;

		auto it = std::find_if(_players.begin(), _players.end(),
			[&](auto& p) { return p->GetID() == id; });
		if (it == _players.end())
			continue;

		auto obj = *it;

		auto itJS = _jumpStates.find(id);
		bool remoteJumping = (itJS != _jumpStates.end() && itJS->second.isJumping);

		Vec3 pos = obj->GetTransform()->GetLocalPosition();

		pos.x = e.position.x;
		pos.z = e.position.z;

		if (!remoteJumping) {
			pos.y = e.position.y;
		}
		obj->GetTransform()->SetLocalPosition(pos);

		// 3) 회전도 보정
		/*Vec3 rot = obj->GetTransform()->GetLocalRotation();
		rot.y = e.yaw;
		obj->GetTransform()->SetLocalRotation(rot);*/
	}
}

void Scene::ClearPlayers()
{
	for (auto& obj : _players)
		RemoveGameObject(obj);
	_players.clear();
}