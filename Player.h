#pragma once

class Model;
class Player {
public:
	Player(Model* model);
	~Player();

	void Update();
	void Draw();

private:

	Model* model_ = nullptr;
};

