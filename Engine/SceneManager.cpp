#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"

#include "Engine.h"
#include "Material.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Camera.h"
#include "Light.h"

#include "TestCameraScript.h"
#include "Resources.h"
#include "ParticleSystem.h"
#include "Terrain.h"
#include "SphereCollider.h"
#include "MeshData.h"
#include "TestDragon.h"

#include "Zombie.h"
#include "M4A1.h"
#include "AK47.h"

#include "Container.h"

// TODO: 나중에 삭제
#include "Timer.h"
#include <sstream>
#include "TestObjectScript.h"

void SceneManager::Update()
{
	if (_activeScene == nullptr)
		return;

	_activeScene->Update();
	_activeScene->LateUpdate();
	_activeScene->FinalUpdate();
}

// TEMP
void SceneManager::Render()
{
	if (_activeScene)
		_activeScene->Render();
}

void SceneManager::RenderUI()
{
	uint8 backbufferindex = GEngine->GetSwapChain()->GetBackBufferIndex();
	shared_ptr<D3D11On12Device> device = GEngine->GetD3D11on12Device();
	D2D1_SIZE_F rtSize = device->GetD3D11On12RT(backbufferindex)->GetSize();
	//D2D1_RECT_F textRect = D2D1::RectF(0, 0, rtSize.width, rtSize.height);

	// Acquire our wrapped render target resource for the current back buffer.
	device->GetD3D11on12Device()->AcquireWrappedResources(device->GetWrappedBackBuffer(backbufferindex).GetAddressOf(), 1);
	// Render text directly to the back buffer.
	device->GetD2DDeviceContext()->SetTarget(device->GetD3D11On12RT(backbufferindex).Get());
	device->GetSolidColorBrush()->SetColor(D2D1::ColorF(D2D1::ColorF::White));
	device->GetD2DDeviceContext()->BeginDraw();

	// TODO: UI 렌더링
	//if (_activeScene)
	//	_activeScene->RenderUI();

	static float elapsedTime = 0.f;
	if(DELTA_TIME < 1.f)
		elapsedTime += DELTA_TIME;
	if (_activeScene)
	{
		// 총알 UI
		Vec2 pivot = {
			static_cast<float>(GEngine->GetWindow().width - 100),
			static_cast<float>(GEngine->GetWindow().height - 50) };
		D2D1_RECT_F textRect = D2D1::RectF(pivot.x - 100, pivot.y - 100, pivot.x + 100, pivot.y + 100);
		int32 currentAmmo = 0;
		
		int32 gunType = static_pointer_cast<TestCameraScript>(_activeScene->FindGameObject(L"Main_Camera")->GetMonoBehaviour(L"MainCamera"))->getGunType();
		if (gunType == 0)
			currentAmmo = static_pointer_cast<M4A1>(_activeScene->FindGameObject(L"M4A1")->GetMonoBehaviour(L"M4A1"))->GetCurrentAmmo();
		else if (gunType == 1)
			currentAmmo = static_pointer_cast<M4A1>(_activeScene->FindGameObject(L"AK47")->GetMonoBehaviour(L"AK47"))->GetCurrentAmmo();
		
		std::wstringstream wss1;
		wss1 << std::fixed << std::setprecision(2) << currentAmmo;
		wstring text = L"탄창: ";
		text += wss1.str();
		device->GetD2DDeviceContext()->DrawTextW(
			text.c_str(),
			static_cast<uint32>(text.size()),
			device->GetTextFormat().Get(),
			&textRect,
			device->GetSolidColorBrush().Get());

		// 타이머 UI
		pivot = { static_cast<float>(GEngine->GetWindow().width / 2), 50.f };
		D2D1_RECT_F textRect2 = D2D1::RectF(pivot.x - 100, pivot.y - 100, pivot.x + 100, pivot.y + 100);

		wstring text2 = L"시간 : ";
		std::wstringstream wss;
		wss << std::fixed << std::setprecision(2) << elapsedTime;
		text2 += wss.str();
		device->GetD2DDeviceContext()->DrawTextW(
			text2.c_str(),
			static_cast<uint32>(text2.size()),
			device->GetTextFormat().Get(),
			&textRect2,
			device->GetSolidColorBrush().Get());

		// 체력 UI
		pivot = { 100.f, static_cast<float>(GEngine->GetWindow().height - 50) };
		D2D1_RECT_F textRect3 = D2D1::RectF(pivot.x - 100, pivot.y - 100, pivot.x + 100, pivot.y + 100);

		wstring text3 = L"HP : 100";
		device->GetD2DDeviceContext()->DrawTextW(
			text3.c_str(),
			static_cast<uint32>(text3.size()),
			device->GetTextFormat().Get(),
			&textRect3,
			device->GetSolidColorBrush().Get());

		pivot = { 100.f, static_cast<float>(GEngine->GetWindow().height / 2) };
		D2D1_RECT_F textRect4 = D2D1::RectF(pivot.x - 100, pivot.y - 200, pivot.x + 100, pivot.y + 200);

		wstring text4 = L"X : ";
		Vec3 mainCameraPos = _activeScene->GetMainCamera()->GetTransform()->GetWorldPosition();
		wss.str(L"");
		wss.clear();
		wss << std::fixed << std::setprecision(2) << mainCameraPos.x;
		text4 += wss.str();
		text4 += L"\nY : ";
		wss.str(L"");
		wss.clear();
		wss << std::fixed << std::setprecision(2) << mainCameraPos.y;
		text4 += wss.str();
		text4 += L"\nZ : ";
		wss.str(L"");
		wss.clear();
		wss << std::fixed << std::setprecision(2) << mainCameraPos.z;
		text4 += wss.str();
		device->GetD2DDeviceContext()->DrawTextW(
			text4.c_str(),
			static_cast<uint32>(text4.size()),
			device->GetTextFormat().Get(),
			&textRect4,
			device->GetSolidColorBrush().Get());
	}

	device->GetD2DDeviceContext()->EndDraw();
	// Release our wrapped render target resource. Releasing 
	// transitions the back buffer resource to the state specified
	// as the OutState when the wrapped resource was created.
	device->GetD3D11on12Device()->ReleaseWrappedResources(device->GetWrappedBackBuffer(backbufferindex).GetAddressOf(), 1);

	// Flush to submit the 11 command list to the shared command queue.
	device->GetD3D11DeviceContext()->Flush();
}

void SceneManager::LoadScene(wstring sceneName)
{
	// TODO : 기존 Scene 정리
	// TODO : 파일에서 Scene 정보 로드

	_activeScene = LoadTestScene();

	_activeScene->Awake();
	_activeScene->Start();
}

void SceneManager::SetLayerName(uint8 index, const wstring& name)
{
	// 기존 데이터 삭제
	const wstring& prevName = _layerNames[index];
	_layerIndex.erase(prevName);

	_layerNames[index] = name;
	_layerIndex[name] = index;
}

uint8 SceneManager::LayerNameToIndex(const wstring& name)
{
	auto findIt = _layerIndex.find(name);
	if (findIt == _layerIndex.end())
		return 0;

	return findIt->second;
}

shared_ptr<GameObject> SceneManager::Pick(int32 screenX, int32 screenY)
{
	shared_ptr<Camera> camera = GetActiveScene()->GetMainCamera();

	float width = static_cast<float>(GEngine->GetWindow().width);
	float height = static_cast<float>(GEngine->GetWindow().height);

	Matrix projectionMatrix = camera->GetProjectionMatrix();

	// ViewSpace에서 Picking 진행
	float viewX = (+2.0f * screenX / width - 1.0f) / projectionMatrix(0, 0);
	float viewY = (-2.0f * screenY / height + 1.0f) / projectionMatrix(1, 1);

	Matrix viewMatrix = camera->GetViewMatrix();
	Matrix viewMatrixInv = viewMatrix.Invert();

	auto& gameObjects = GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObjects();

	float minDistance = FLT_MAX;
	shared_ptr<GameObject> picked;

	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetCollider() == nullptr)
			continue;

		// ViewSpace에서의 Ray 정의
		Vec4 rayOrigin = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
		Vec4 rayDir = Vec4(viewX, viewY, 1.0f, 0.0f);

		// WorldSpace에서의 Ray 정의
		rayOrigin = XMVector3TransformCoord(rayOrigin, viewMatrixInv);
		rayDir = XMVector3TransformNormal(rayDir, viewMatrixInv);
		rayDir.Normalize();

		// WorldSpace에서 연산
		float distance = 0.f;
		if (gameObject->GetCollider()->Intersects(rayOrigin, rayDir, OUT distance) == false)
			continue;

		if (distance < minDistance)
		{
			minDistance = distance;
			picked = gameObject;
		}
	}

	return picked;
}

shared_ptr<Scene> SceneManager::LoadTestScene()
{
#pragma region LayerMask
	SetLayerName(0, L"Default");
	SetLayerName(1, L"Gun"); // 총 UI 별도 처리
	SetLayerName(2, L"UI");
#pragma endregion

#pragma region ComputeShader
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ComputeShader");

		// UAV 용 Texture 생성
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->CreateTexture(L"UAVTexture",
			DXGI_FORMAT_R8G8B8A8_UNORM, 1024, 1024,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"ComputeShader");
		material->SetShader(shader);
		material->SetInt(0, 1);
		GEngine->GetComputeDescHeap()->SetUAV(texture->GetUAVHandle(), UAV_REGISTER::u0);

		// 쓰레드 그룹 (1 * 1024 * 1)
		material->Dispatch(1, 1024, 1);
	}
#pragma endregion

	shared_ptr<Scene> scene = make_shared<Scene>();

#pragma region Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Main_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, FOV=45도
		camera->AddComponent(make_shared<TestCameraScript>());
		camera->GetCamera()->SetFar(10000.f);
		camera->GetCamera()->SetFOV(90.f);
		camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));

		uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI는 안 찍음

		layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"Gun");
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // Gun은 안 찍음

		scene->AddGameObject(camera);
	}
#pragma endregion

#pragma region UI_Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Orthographic_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, 800*600
		camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
		camera->GetCamera()->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);
		uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskAll(); // 다 끄고
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, false); // UI만 찍음
		scene->AddGameObject(camera);
	}
#pragma endregion

#pragma region GunCamera
	{
		shared_ptr<GameObject> gunCamera = make_shared<GameObject>();
		gunCamera->SetName(L"Gun_Camera");

		gunCamera->AddComponent(make_shared<Transform>());
		gunCamera->AddComponent(make_shared<Camera>());
		
		gunCamera->GetCamera()->SetFOV(60.f);
		gunCamera->GetCamera()->SetFar(1000.f);

		gunCamera->GetCamera()->SetCullingMaskAll(); // 다 끄고
		gunCamera->GetCamera()->SetCullingMaskLayerOnOff(GET_SINGLE(SceneManager)->LayerNameToIndex(L"Gun"), false); // Gun만 찍음

		// Main_Camera의 Transform을 따라가도록 설정
		shared_ptr<GameObject> mainCam = scene->FindGameObject(L"Main_Camera");
		gunCamera->GetTransform()->SetParent(mainCam->GetTransform());

		scene->AddGameObject(gunCamera);
	}
#pragma endregion

#pragma region SkyBox
	{
		shared_ptr<GameObject> skybox = make_shared<GameObject>();
		skybox->AddComponent(make_shared<Transform>());
		skybox->SetCheckFrustum(false);
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
			meshRenderer->SetMesh(sphereMesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Skybox");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Sky02", L"..\\Resources\\Texture\\Sky02.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		skybox->AddComponent(meshRenderer);
		scene->AddGameObject(skybox);
	}
#pragma endregion

#pragma region Object
	//{
	//	shared_ptr<GameObject> obj = make_shared<GameObject>();
	//	obj->SetName(L"OBJ");
	//	obj->AddComponent(make_shared<Transform>());
	//	obj->AddComponent(make_shared<SphereCollider>());
	//	obj->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
	//	obj->GetTransform()->SetLocalRotation(Vec3(0.f, 0.f, 0.f));
	//	obj->GetTransform()->SetLocalPosition(Vec3(0, 0.f, 500.f));
	//	obj->SetStatic(false);
	//	shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
	//	{
	//		shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
	//		meshRenderer->SetMesh(sphereMesh);
	//	}
	//	{
	//		shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"GameObject");
	//		meshRenderer->SetMaterial(material->Clone());
	//	}
	//	dynamic_pointer_cast<SphereCollider>(obj->GetCollider())->SetRadius(0.5f);
	//	dynamic_pointer_cast<SphereCollider>(obj->GetCollider())->SetCenter(Vec3(0.f, 0.f, 0.f));
	//	obj->AddComponent(meshRenderer);
	//	obj->AddComponent(make_shared<TestObjectScript>());
	//	scene->AddGameObject(obj);
	//}
#pragma endregion

//#pragma region Terrain
//	{
//		shared_ptr<GameObject> obj = make_shared<GameObject>();
//		obj->AddComponent(make_shared<Transform>());
//		obj->AddComponent(make_shared<Terrain>());
//		obj->AddComponent(make_shared<MeshRenderer>());
//
//		obj->GetTransform()->SetLocalScale(Vec3(300.f, 300.f, 300.f));
//		obj->GetTransform()->SetLocalPosition(Vec3(0.0f, 0.0f, 0.0f));
//		obj->SetStatic(true);
//		obj->GetTerrain()->Init(64, 64);
//		obj->SetCheckFrustum(false);
//
//		scene->AddGameObject(obj);
//	}
//#pragma endregion

#pragma region UI_Test
	for (int32 i = 0; i < 6; i++)
	{
		shared_ptr<GameObject> obj = make_shared<GameObject>();
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
		obj->AddComponent(make_shared<Transform>());
		obj->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
		obj->GetTransform()->SetLocalPosition(Vec3(-350.f + (i * 120), 250.f, 500.f));
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Texture");

			shared_ptr<Texture> texture;
			if (i < 3)
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->GetRTTexture(i);
			else if (i < 5)
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->GetRTTexture(i - 3);
			else
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->GetRTTexture(0);

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		obj->AddComponent(meshRenderer);
		scene->AddGameObject(obj);
	}
#pragma endregion

#pragma region Crosshair
	{
		shared_ptr<GameObject> crosshair = make_shared<GameObject>();
		crosshair->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		crosshair->AddComponent(make_shared<Transform>());
		crosshair->GetTransform()->SetLocalScale(Vec3(50.f, 50.f, 50.f));
		crosshair->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 300.f));

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"AlphaTexture");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Crosshair", L"..\\Resources\\Texture\\Crosshair\\crosshair01.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		crosshair->SetName(L"Crosshair");
		crosshair->AddComponent(meshRenderer);
		scene->AddGameObject(crosshair);
	}
#pragma endregion

#pragma region M4A1
	{
		shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\M4A1.fbx");

		vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();
		uint8 gunLayer = GET_SINGLE(SceneManager)->LayerNameToIndex(L"Gun");

		for (auto& gameObject : gameObjects)
		{
			if (gameObject->GetMeshRenderer())
			{
				auto mat = gameObject->GetMeshRenderer()->GetMaterial()->Clone(); // 기존 머티리얼 복사
				auto shader = GET_SINGLE(Resources)->Get<Shader>(L"Forward"); // FORWARD 타입 셰이더

				mat->SetShader(shader);
				gameObject->GetMeshRenderer()->SetMaterial(mat);
			}
			gameObject->SetLayerIndex(gunLayer);
			gameObject->SetCheckFrustum(false);
			gameObject->SetActive(false);
			
			scene->AddGameObject(gameObject);
			gameObject->AddComponent(make_shared<M4A1>());
		}

		gameObjects[0]->SetName(L"M4A1");
	}
#pragma endregion

#pragma region AK47
	{
		shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\AK74.fbx");

		vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();
		uint8 gunLayer = GET_SINGLE(SceneManager)->LayerNameToIndex(L"Gun");

		for (auto& gameObject : gameObjects)
		{
			if (gameObject->GetMeshRenderer())
			{
				auto mat = gameObject->GetMeshRenderer()->GetMaterial()->Clone(); // 기존 머티리얼 복사
				auto shader = GET_SINGLE(Resources)->Get<Shader>(L"Forward"); // FORWARD 타입 셰이더

				mat->SetShader(shader);
				gameObject->GetMeshRenderer()->SetMaterial(mat);
			}
			gameObject->SetLayerIndex(gunLayer);
			gameObject->SetCheckFrustum(false);
			gameObject->SetActive(true);

			scene->AddGameObject(gameObject);
			gameObject->AddComponent(make_shared<AK47>());
		}

		gameObjects[0]->SetName(L"AK47");
	}
#pragma endregion

#pragma region Directional Light
	{
		shared_ptr<GameObject> light = make_shared<GameObject>();
		light->AddComponent(make_shared<Transform>());
		light->GetTransform()->SetLocalPosition(Vec3(0, 1000, 500));
		light->AddComponent(make_shared<Light>());
		light->GetLight()->SetLightDirection(Vec3(0, -1, 1.f));
		light->GetLight()->SetLightType(LIGHT_TYPE::DIRECTIONAL_LIGHT);
		light->GetLight()->SetDiffuse(Vec3(1.f, 1.f, 1.f));
		light->GetLight()->SetAmbient(Vec3(0.1f, 0.1f, 0.1f));
		light->GetLight()->SetSpecular(Vec3(0.1f, 0.1f, 0.1f));

		scene->AddGameObject(light);
	}
#pragma endregion


//#pragma region MAP
//	// 임시 맵 제작 ( 나중에 수정할 예정 )
//	shared_ptr<Container> container = make_shared<Container>();
//
//	const float cx = 806.f; // 컨테이너 크기
//	const float cy = 273.f;
//	const float cz = 298.f;
//
//	for (int y = 0; y < 2; ++y)
//	{
//		for (int x = 0; x < 1; ++x)
//		{
//			for (int z = -10; z < 20; ++z)
//			{
//				container->createContainer(scene, static_cast<uint8>(ContainerType::Container1),
//					Vec3(2000.f + cx*x, cy/2 +cy*y, cz*z), Vec3(100.f, 100.f, 100.f), Vec3(-90.f, 0.f, 0.f));
//
//				container->createContainer(scene, static_cast<uint8>(ContainerType::Container1),
//					Vec3(-2000.f + cx * x, cy / 2 + cy * y, cz* z), Vec3(100.f, 100.f, 100.f), Vec3(-90.f, 180.f, 0.f));
//			}
//		}
//	}
//
//	for (int x = 1; x < 5; ++x)
//	{
//		container->createContainer(scene, static_cast<uint8>(ContainerType::Container1),
//			Vec3(-2000.f + cx * x , cy / 2, cz * -10), Vec3(100.f, 100.f, 100.f), Vec3(-90.f, 0.f, 0.f));
//
//		container->createContainer(scene, static_cast<uint8>(ContainerType::Container1),
//			Vec3(-2000.f + cx * x, cy / 2 + cy, cz * -10), Vec3(100.f, 100.f, 100.f), Vec3(-90.f, 0.f, 0.f));
//	}
//	
//	container->createContainer(scene, static_cast<uint8>(ContainerType::Container1),
//		Vec3(-500, cy / 2, 2300), Vec3(100.f, 100.f, 100.f), Vec3(-90.f, -120.f, 0.f));
//
//	container->createContainer(scene, static_cast<uint8>(ContainerType::Container1),
//		Vec3(-500, cy / 2, -1800), Vec3(100.f, 100.f, 100.f), Vec3(-90.f, -120.f, 0.f));
//
//	container->createContainer(scene, static_cast<uint8>(ContainerType::Container1),
//		Vec3(500, cy / 2, 5300), Vec3(100.f, 100.f, 100.f), Vec3(-90.f, 120.f, 0.f));
//
//#pragma endregion

#pragma region Zombie
	//{
	//	shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\NormalZombie.fbx");

	//	for (int i = 0; i < 10; i++)
	//	{
	//		vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();

	//		for (auto& gameObject : gameObjects)
	//		{
	//			gameObject->SetName(L"Zombie");
	//			gameObject->SetCheckFrustum(false);
	//			gameObject->GetTransform()->SetLocalPosition(Vec3(-800.f + (160.f * i), 70.f, 2000.f));
	//			gameObject->GetTransform()->SetLocalScale(Vec3(2.f, 2.f, 2.f));
	//			gameObject->GetTransform()->SetLocalRotation(Vec3(-90.f, 0.f, 0.f));
	//			scene->AddGameObject(gameObject);
	//			gameObject->AddComponent(make_shared<Zombie>());
	//		}
	//	}
	//}
#pragma endregion

#pragma region TestFBX
	{
		shared_ptr<GameObject> t = make_shared<GameObject>();
		t->AddComponent(make_shared<Transform>());
		t->GetTransform()->SetLocalRotation(Vec3(-90.f, 180.f, 0.f));
		scene->AddGameObject(t);

		shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Factory1Items.fbx");
	
		vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate(ColliderType::OBB);
		
		for (auto& gameObject : gameObjects)
		{
			gameObject->SetName(L"Map");
			gameObject->SetStatic(true);
			gameObject->GetTransform()->SetParent(t->GetTransform());
			scene->AddGameObject(gameObject);
		}
	}
#pragma endregion


	return scene;
}