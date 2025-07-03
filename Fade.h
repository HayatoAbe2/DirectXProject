#pragma once
#include "Sprite.h"
class Fade {
public:

	// 状態
	enum class Status {
		None,
		FadeIn,
		FadeOut,
	};

	
	~Fade();
	void Initialize(HWND hwnd, Graphics* graphics,Camera* camera);
	void Update();
	void Draw();

	void Start(Status status,float duration);
	void Stop();
	bool IsFinish()const;

private:

	Graphics* graphics_ = nullptr;
	Sprite* sprite_ = nullptr;

	// カメラ
	Camera* camera_;

	// デバッグカメラ
	bool isDebugCameraActive_ = false;
	DebugCamera* debugCamera_ = nullptr;

	Status status_ = Status::None;

	// かかる時間
	float duration_ = 0.0f;
	// 時間経過
	float counter_ = 0.0f;
};

