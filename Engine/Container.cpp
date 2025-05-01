#include "pch.h"
#include "Container.h"
#include "EnginePch.h"
#include "Resources.h"
#include "MeshData.h"
#include "GameObject.h"
#include "Transform.h"
#include "Scene.h"
#include "SceneManager.h"

shared_ptr<MeshData> Container::meshData[CONTAINER_TYPE_COUNT];
bool Container::initialized = false;


Container::Container()
{
	if (!initialized)
	{
		initialized = true;
		meshLoad();
	}
}	

void Container::meshLoad()
{
	meshData[0] = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Container\\Container.fbx");
	meshData[1] = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Container\\Container20-2Doors.fbx");
	/*meshData[2] = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Container\\Container20-2DoorsShell.fbx");
	meshData[3] = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Container\\Container20-4Doors.fbx");
	meshData[4] = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Container\\Container20-4DoorsShell.fbx");
	meshData[5] = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Container\\Container20-LeftDoor.fbx");
	meshData[6] = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Container\\Container20-RightDoor.fbx");*/
	
	
}

void Container::createContainer(shared_ptr<class Scene>& scene, uint8 type, Vec3 Position, Vec3 Scale, Vec3 Rotation)
{
	vector<shared_ptr<GameObject>> gameObjects = meshData[type]->Instantiate();

	for (auto& gameObject : gameObjects)
	{
		gameObject->SetName(L"Container");
		gameObject->SetCheckFrustum(false);
		gameObject->GetTransform()->SetLocalPosition(Position);
		gameObject->GetTransform()->SetLocalScale(Scale);
		gameObject->GetTransform()->SetLocalRotation(Rotation);
		scene->AddGameObject(gameObject);
	}
}
