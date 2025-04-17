#pragma once

enum class ContainerType : uint8
{
	Container1 = 0,
	Container2,
	Container3,
	Container4,
	Container5,
	Container6,
	Container7,
	End,
};

constexpr int CONTAINER_TYPE_COUNT = static_cast<int>(ContainerType::End);

class Container
{
public:
	Container();
	
	void createContainer(shared_ptr<class Scene>& scene, uint8 type, Vec3 Position, Vec3 Scale, Vec3 Rotation);
	//vector<shared_ptr<class GameObject>>& getGameObjects() { return gameObjects; }
	
private:
	//vector<shared_ptr<class GameObject>> gameObjects;
	static bool initialized;
	static shared_ptr<class MeshData> meshData[CONTAINER_TYPE_COUNT];

	void meshLoad();
};

