#pragma once
#include "MathUtils.h"

#include <cstdint>
#include <string>

class ResourceManager;
class Input;
class Audio;
class Model;
class Sprite;
class Renderer;
class GameContext {
public:
	GameContext(Renderer* renderer, Audio* audio, Input* input,ResourceManager* resourceManager);

	///
	/// ウィンドウ情報
	///

	Vector2 GetWindowSize() const;

	///
	/// リソース管理系
	///

	Model* LoadModel(const std::string& directoryPath, const std::string& filename);
	Sprite* LoadSprite(std::string texturePath, Vector2 size);

	///
	/// 描画系 
	///

	void DrawModel(class Model& model, bool useAlphaBlend = 0);
	void DrawSprite(class Sprite& sprite);

	/// 
	/// 入力系
	/// 

	bool IsTrigger(uint8_t keyNumber);
	bool IsPress(uint8_t keyNumber);
	bool IsRelease(uint8_t keyNumber);
	bool isClickLeft();
	bool isClickRight();
	bool isClickWheel();
	bool isTriggerLeft();
	bool isTriggerRight();
	bool isTriggerWheel();
	Vector3 GetMouseMove();
	Vector2 GetMousePosition();

private:
	Renderer* renderer_ = nullptr;
	Audio* audio_ = nullptr;
	Input* input_ = nullptr;
	ResourceManager* resourceManager_ = nullptr;
};