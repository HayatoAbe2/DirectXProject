#pragma once
#include "Input.h"
#include "Graphics.h"
#include "Model.h"
class TitleScene {
public:

	~TitleScene();
	void Initialize(Input* input, HWND hwnd);
	void Update();
	void Draw();

	bool isFinished() const { return finished_; };
private:
	// シーンの終了
	bool finished_ = false;

	// 描画
	Graphics graphics_;

	// 入力
	Input* input_;
};