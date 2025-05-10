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
	void AddPlayer(struct sc_packet_player_info* packet);
	void RemovePlayer(struct sc_packet_player_leave* packet);
	//void AddZombie(struct sc_packet_login_ok* packet);

	void MovePlayer(struct sc_packet_move* packet);
	void JumpPlayer(struct sc_packet_jump* packet);
	void LandPlayer(struct sc_packet_land * packet);
	void ApplySnapshot(struct sc_packet_snapshot* packet);
	void AnimatePlayer(struct sc_packet_state* packet);
	void ClearPlayers();

	void AddZombie(struct sc_packet_spawn_zombie* packet);
private:
	vector<shared_ptr<GameObject>>		_gameObjects;
	vector<shared_ptr<class Camera>>	_cameras;
	vector<shared_ptr<class Light>>		_lights;

	vector<vector<shared_ptr<GameObject>>>	_players;
	vector<shared_ptr<GameObject>>		_zombies;

	// array<플레이어를 이루는 게임오브젝트들, 플레이어 수> _players
	// 어차피 부모는 게임오브젝트들의 0번일꺼니까
	// MovePacket으로 적용시킬때, 탐색하는 경우, _player

	std::unordered_map<uint32_t, JumpState> _jumpStates;
};

