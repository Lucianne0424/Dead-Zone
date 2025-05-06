#pragma once

class GameObject;

struct JumpState {
	bool  isJumping = false;
	float verticalVel = 0.0f;
};

class Scene
{
public:
	void Awake();
	void Start();
	void Update();
	void LateUpdate();
	void FinalUpdate();

	shared_ptr<class Camera> GetMainCamera();

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
	void AddPlayer(struct sc_packet_login_ok* packet);
	void RemovePlayer(struct sc_packet_player_leave* packet);
	//void AddZombie(struct sc_packet_login_ok* packet);

	void MovePlayer(struct sc_packet_move* packet);
	void JumpPlayer(struct sc_packet_jump* packet);
	void LandPlayer(struct sc_packet_land * packet);
	void ApplySnapshot(struct sc_packet_snapshot* snap);
	void ClearPlayers();
private:
	vector<shared_ptr<GameObject>>		_gameObjects;
	vector<shared_ptr<class Camera>>	_cameras;
	vector<shared_ptr<class Light>>		_lights;

	vector<shared_ptr<GameObject>>		_players;
	vector<shared_ptr<GameObject>>		_zombies;

	std::unordered_map<uint32_t, JumpState> _jumpStates;
};

