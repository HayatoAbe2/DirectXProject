#pragma once

class Model;
class GameContext;
class Camera;

class Player {
public:
	~Player();

	void Initialize(Model* playerModel);
	void Update();
	void Draw(GameContext* context, Camera* camera);

private:

	Model* model_ = nullptr;
};

