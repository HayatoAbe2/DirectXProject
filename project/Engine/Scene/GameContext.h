#pragma once
#include "MathUtils.h"
#include "../Graphics/BlendMode.h"

#include <cstdint>
#include <string>
#include <memory>
#include <random>

class ResourceManager;
class Input;
class Audio;
class Entity;
class Model;
class Sprite;
class InstancedModel;
class Camera;
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

	std::shared_ptr<Model> LoadModel(const std::string& directoryPath, const std::string& filename, bool enableLighting = true);
	std::shared_ptr<InstancedModel> LoadInstancedModel(const std::string& directoryPath, const std::string& filename, const int num);
	std::shared_ptr<Sprite> LoadSprite(std::string texturePath);

	///
	/// トランスフォーム更新
	///



	///
	/// 描画系 
	///

	void DrawEntity(Entity& entity, Camera& camera,BlendMode blendMode = BlendMode::Normal);

	/// 
	/// 入力系
	/// 

	bool IsTrigger(uint8_t keyNumber);
	bool IsPress(uint8_t keyNumber);
	bool IsRelease(uint8_t keyNumber);
	bool IsClickLeft();
	bool IsClickRight();
	bool IsClickWheel();
	bool IsTriggerLeft();
	bool IsTriggerRight();
	bool IsTriggerWheel();
	Vector3 GetMouseMove();
	Vector2 GetMousePosition();
	bool IsControllerPress(uint8_t buttonNumber);
	Vector2 GetLeftStick();
	Vector2 GetRightStick();

	/// 
	/// 乱数
	///

	int RandomInt(int min, int max);
	float RandomFloat(float min, float max);

private:
	Renderer* renderer_ = nullptr;
	Audio* audio_ = nullptr;
	Input* input_ = nullptr;
	ResourceManager* resourceManager_ = nullptr;
	std::random_device randomDevice_;
	std::mt19937 randomEngine_;
};