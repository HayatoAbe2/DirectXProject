#pragma once

// ゲームシーンにデフォルトで入れておくインクルード
#include "MathUtils.h"
#include "Camera.h"
#include "../Object/Model.h"
#include "../Object/InstancedModel.h"
#include "../Object/Sprite.h"
#include "../Object/ParticleSystem.h"
#include "../Object/Lights/LightsForGPU.h"
#include "../Graphics/BlendMode.h"

#include <cstdint>
#include <string>
#include <memory>
#include <random>

class ResourceManager;
class LightManager;
class Input;
class Audio;
class Renderer;

class GameContext {
public:
	GameContext(Renderer* renderer, Audio* audio, Input* input, ResourceManager* resourceManager, LightManager* lightManager);

	///
	/// ウィンドウ情報
	///

	Vector2 GetWindowSize() const;

	///
	/// リソース管理系
	///

	std::unique_ptr<Model> LoadModel(const std::string& directoryPath, const std::string& filename, bool enableLighting = true);
	std::unique_ptr<InstancedModel> LoadInstancedModel(const std::string& directoryPath, const std::string& filename, const int num);
	std::unique_ptr<Sprite> LoadSprite(std::string texturePath);

	///
	/// ライトの操作
	///

	int AddPointLight();
	void RemovePointLight(int index);
	PointLight& GetPointLight(int index);
	int AddSpotLight();
	void RemoveSpotLight(int index);
	SpotLight& GetSpotLight(int index);
	void DrawLightImGui();

	///
	/// 描画系 
	///

	void DrawModel(Model* model, Camera* camera, BlendMode blendMode = BlendMode::Normal);
	void DrawSprite(Sprite* sprite, BlendMode blendMode = BlendMode::Normal);
	void DrawInstancedModel(InstancedModel* instancedModel, Camera* camera, BlendMode blendMode = BlendMode::Normal);
	void DrawParticle(ParticleSystem* particleSystem, Camera* camera, BlendMode blendMode = BlendMode::Normal);

	/// 
	/// 入力系
	/// 

	bool IsTrigger(uint8_t keyNumber);
	bool IsPress(uint8_t keyNumber);
	bool IsRelease(uint8_t keyNumber);
	bool IsClickLeft();
	bool IsClickRight();
	bool IsClickWheel();
	bool IsTriggerLeftClick();
	bool IsTriggerRightClick();
	bool IsTriggerMouseWheel();
	Vector3 GetMouseMove();
	Vector2 GetMousePosition();
	bool IsControllerPress(uint8_t buttonNumber);
	Vector2 GetLeftStick();
	Vector2 GetRightStick();

	///
	/// 音声
	/// 

	void SoundLoad(const wchar_t* filename);
	void SoundPlay(const wchar_t* filename, bool isLoop,float volume = 1.0f);
	void SoundUnload(const wchar_t* filename);


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
	LightManager* lightManager_ = nullptr;
	std::random_device randomDevice_;
	std::mt19937 randomEngine_;
};