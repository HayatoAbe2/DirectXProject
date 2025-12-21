#pragma once
#include "../Io/Logger.h"
#include "../Scene/DebugCamera.h"
#include "../Object/Material.h"
#include "../Object/Transform.h"
#include "../Object/TransformationMatrix.h"
#include "../Object/VertexData.h"
#include "../Object/LightManager.h"
#include "BlendMode.h"
#include "SRVManager.h"
#include "CameraForGPU.h"

#include <wrl.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <dxcapi.h>
#include <memory>

class Model;
class InstancedModel;
class Sprite;
class ParticleSystem;
class DirectXContext;
class Camera;

class Renderer {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(DirectXContext* dxContext);

	/// <summary>
	/// トランスフォーム更新
	/// </summary>
	void UpdateModelTransforms(Model* model, Camera* camera);

	void UpdateSpriteTransform(Sprite* sprite);

	/// <summary>
	/// モデル描画
	/// </summary>
	/// <param name="blendMode">ブレンドモード</param>
	void DrawModel(Model* model, Camera* camera, LightManager* lightManager, int blendMode);

	/// <summary>
	/// インスタンスモデル描画
	/// </summary>
	/// <param name="model">複数インスタンスを持つモデル</param>
	/// <param name="blendMode">ブレンドモード</param>
	void DrawModelInstance(InstancedModel* model, Camera* camera, LightManager* lightManager, int blendMode);

	void DrawParticles(ParticleSystem* particleSys, Camera* camera, int blendMode);

	void DrawSprite(Sprite* sprite, int blendMode);

	/// <summary>
	/// フレーム開始時の処理(描画開始時に行う)
	/// </summary>
	void BeginFrame();

	/// <summary>
	/// フレーム終了時の処理(描画終了時に行う)
	/// </summary>
	void EndFrame();

private:
	DirectXContext* dxContext_ = nullptr;

	// transform
	Microsoft::WRL::ComPtr<ID3D12Resource> transformBuffer_;
	UINT8* mappedTransformData_ = nullptr;

	// CBサイズ
	static constexpr UINT kCBSize = (sizeof(TransformationMatrix) + 255) & ~255;
	const UINT kMaxObjects = 4096; // 最大数。もし足りなかったら増やす

	// カメラ位置(GPU転送)
	CameraForGPU* cameraData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraBuffer_;

	LightsForGPU* dummyLight_;
	Microsoft::WRL::ComPtr<ID3D12Resource> dummyLightBuffer_;
};