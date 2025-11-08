#pragma once
#include "../Io/Logger.h"
#include "../Scene/DebugCamera.h"
#include "../Object/Material.h"
#include "../Object/Model.h"
#include "../Object/Entity.h"
#include "../Object/Transform.h"
#include "../Object/TransformationMatrix.h"
#include "../Object/VertexData.h"
#include "../Scene/DirectionalLight.h"
#include "BlendMode.h"
#include "DirectXContext.h"

#include <wrl.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <dxcapi.h>
#include <memory>

class Renderer {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(int32_t clientWidth, int32_t clientHeight, HWND hwnd, Logger* logger);

	/// <summary>
	/// 解放処理(ループ終了後に行う)
	/// </summary>
	void Finalize();

	/// <summary>
	/// トランスフォーム更新
	/// </summary>
	/// <param name="entity">エンティティ</param>
	/// <param name="camera">カメラ</param>
	void UpdateEntityTransforms(const Entity& entity, const Camera& camera);

	void UpdateSpriteTransform(Entity& entity);

	void DrawEntity(Entity& entity, const Camera& camera, int blendMode);

	void DrawModel(Entity* entity, int blendMode);
	
	/// <summary>
	/// モデル描画
	/// </summary>
	/// <param name="blendMode">ブレンドモード</param>

	/// <summary>
	/// インスタンスモデル描画
	/// </summary>
	/// <param name="model">複数インスタンスを持つモデル</param>
	/// <param name="blendMode">ブレンドモード</param>
	void DrawModelInstance(Model& model, int blendMode);

	void DrawSprite(Entity* entity, int blendMode);

	/// <summary>
	/// フレーム開始時の処理(描画開始時に行う)
	/// </summary>
	void BeginFrame();

	/// <summary>
	/// フレーム終了時の処理(描画終了時に行う)
	/// </summary>
	void EndFrame();

	/// <summary>
	/// ImGui初期化
	/// </summary>
	/// <param name="hwnd">HWND</param>
	void InitializeImGui(HWND hwnd) { dxContext_.get()->InitializeImGui(hwnd); };


	// getter
	int32_t GetWindowWidth() { return dxContext_.get()->GetWindowWidth(); }
	int32_t GetWindowHeight() { return dxContext_.get()->GetWindowHeight(); }
	DeviceManager* GetDeviceManager() { return dxContext_.get()->GetDeviceManager(); }
	CommandListManager* GetCommandListManager() { return dxContext_.get()->GetCommandListManager(); }
	DescriptorHeapManager* GetDescriptorHeapManager() { return dxContext_.get()->GetDescriptorHeapManager(); }


private:
	std::unique_ptr<DirectXContext> dxContext_ = nullptr;
	void* mappedCBV_ = nullptr;

	// transform
	Microsoft::WRL::ComPtr<ID3D12Resource> transformBuffer_;
	UINT8* mappedTransformData_ = nullptr;

	// CBサイズ
	static constexpr UINT kCBSize = (sizeof(Matrix4x4) + 255) & ~255;
	const UINT kMaxObjects = 4096; // 最大数。もし足りなかったら増やす
};