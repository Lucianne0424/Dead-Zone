#include "pch.h"
#include "MeshData.h"
#include "FBXLoader.h"
#include "Mesh.h"
#include "Material.h"
#include "Resources.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Animator.h"

#include "SphereCollider.h"
#include "OrientedBoxCollider.h"

MeshData::MeshData() : Object(OBJECT_TYPE::MESH_DATA)
{
}

MeshData::~MeshData()
{
}

shared_ptr<MeshData> MeshData::LoadFromFBX(const wstring& path)
{
	FBXLoader loader;
	loader.LoadFbx(path);

	shared_ptr<MeshData> meshData = make_shared<MeshData>();

	for (int32 i = 0; i < loader.GetMeshCount(); i++)
	{
		shared_ptr<Mesh> mesh = Mesh::CreateFromFBX(&loader.GetMesh(i), loader);

		GET_SINGLE(Resources)->Add<Mesh>(mesh->GetName(), mesh);

		// Material 찾아서 연동
		vector<shared_ptr<Material>> materials;
		for (size_t j = 0; j < loader.GetMesh(i).materials.size(); j++)
		{
			shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(loader.GetMesh(i).materials[j].name);
			materials.push_back(material);
		}

		MeshRenderInfo info = {};
		info.mesh = mesh;
		info.materials = materials;
		
		// 정점들로 바운딩 박스 만들기
		BoundingBox boundingBox;
		FbxMeshInfo meshInfo = loader.GetMesh(i);
		BoundingBox::CreateFromPoints(
			boundingBox,
			meshInfo.vertices.size(),
			&meshInfo.vertices[0].pos,
			sizeof(Vertex));

		info.center = boundingBox.Center;
		info.extents = boundingBox.Extents;

		info.position = meshInfo.position;
		info.rotation = meshInfo.rotation;
		info.scale = meshInfo.scale;

		meshData->_meshRenders.push_back(info);
	}

	return meshData;
}

void MeshData::Load(const wstring& _strFilePath)
{
	// TODO
}

void MeshData::Save(const wstring& _strFilePath)
{
	// TODO
}

vector<shared_ptr<GameObject>> MeshData::Instantiate(ColliderType colliderType)
{
	vector<shared_ptr<GameObject>> v;

	for (MeshRenderInfo& info : _meshRenders)
	{
		shared_ptr<GameObject> gameObject = make_shared<GameObject>();
		gameObject->AddComponent(make_shared<Transform>());
		gameObject->AddComponent(make_shared<MeshRenderer>());
		gameObject->GetMeshRenderer()->SetMesh(info.mesh);

		for (uint32 i = 0; i < info.materials.size(); i++)
			gameObject->GetMeshRenderer()->SetMaterial(info.materials[i]->Clone(), i);

		if (info.mesh->IsAnimMesh())
		{
			shared_ptr<Animator> animator = make_shared<Animator>();
			gameObject->AddComponent(animator);
			animator->SetBones(info.mesh->GetBones());
			animator->SetAnimClip(info.mesh->GetAnimClip());
		}

#pragma region Add Collider
		switch (colliderType)
		{
		case ColliderType::NONE:
			break;
		case ColliderType::SPHERE:
		{
			shared_ptr<SphereCollider> sphere = make_shared<SphereCollider>();
			sphere->SetCenter(info.center);
			sphere->SetRadius(max(max(info.extents.x, info.extents.y), info.extents.z));
			gameObject->AddComponent(sphere);
			break;
		}
		case ColliderType::OBB:
		{
			shared_ptr<OrientedBoxCollider> obb = make_shared<OrientedBoxCollider>();
			obb->SetCenter(info.center);
			obb->SetExtents(info.extents);
			gameObject->AddComponent(obb);
			break;
		}
		}
#pragma endregion

#pragma region Set Transform
		gameObject->GetTransform()->SetLocalPosition(info.position);
		gameObject->GetTransform()->SetLocalRotation(info.rotation);
		gameObject->GetTransform()->SetLocalScale(info.scale);

#pragma endregion

		v.push_back(gameObject);
	}


	return v;
}

