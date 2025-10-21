#pragma once
#include "../Io/Logger.h"
#include "../Scene/DebugCamera.h"
#include "../Object/Material.h"
#include "../Object/Model.h"
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
	/// モデル描画
	/// </summary>
	/// <param name="blendMode">ブレンドモード</param>
	void DrawModel(Model& model, int blendMode);

	/// <summary>
	/// インスタンスモデル描画
	/// </summary>
	/// <param name="model">複数インスタンスを持つモデル</param>
	/// <param name="blendMode">ブレンドモード</param>
	void DrawModelInstance(Model& model, int blendMode);

	/// <summary>
	/// スプライト描画
	/// </summary>
	/// <param name="blendMode">ブレンドモード</param>
	void DrawSprite(Sprite& sprite,int blendMode);

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
};