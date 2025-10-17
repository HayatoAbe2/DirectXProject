#pragma once
#include "MathUtils.h"
#include "../Graphics/BlendMode.h"

#include <cstdint>
#include <string>

class ResourceManager;
class Input;
class Audio;
class Model;
class Sprite;
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

	Model* LoadModel(const std::string& directoryPath, const std::string& filename);
	Model* LoadModel(const std::string& directoryPath, const std::string& filename, const int num);
	Sprite* LoadSprite(std::string texturePath, Vector2 size);

	///
	/// トランスフォーム更新
	///
	
	void UpdateInstanceTransform(Model* model, Camera* camera, const Transform* transforms, int numInstance);

	///
	/// 描画系 
	///

	void DrawModel(class Model& model, BlendMode blendMode = BlendMode::Normal);
	void DrawModelInstance(Model& model, BlendMode blendMode = BlendMode::Normal);
	void DrawSprite(class Sprite& sprite, BlendMode blendMode = BlendMode::Normal);

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

private:
	Renderer* renderer_ = nullptr;
	Audio* audio_ = nullptr;
	Input* input_ = nullptr;
	ResourceManager* resourceManager_ = nullptr;
};