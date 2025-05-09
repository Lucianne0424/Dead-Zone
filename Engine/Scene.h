#pragma once

class GameObject;
class Camera;
class Light;


class Scene
{
public:
	void Awake();
	void Start();
	void Update();
	void LateUpdate();
	void FinalUpdate();

	shared_ptr<Camera> GetMainCamera();

	void Render();
	void RenderUI();

	void ClearRTV();

	void RenderShadow();
	void RenderDeferred();
	void RenderLights();
	void RenderFinal();

	void RenderForward();
	
private:
	void PushLightData();

public:
	void AddGameObject(shared_ptr<GameObject> gameObject);
	void RemoveGameObject(shared_ptr<GameObject> gameObject);

	const vector<shared_ptr<GameObject>>& GetGameObjects() { return _gameObjects; }


public:
	shared_ptr<GameObject> FindGameObject(const wstring& name);

private:
	vector<shared_ptr<GameObject>>		_gameObjects;
	vector<shared_ptr<Camera>>			_cameras;
	vector<shared_ptr<Light>>			_lights;
};

