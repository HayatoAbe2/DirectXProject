#pragma once
#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl.h>

class Logger;

/// <summary>
/// デバイス初期化、アダプタ決定、デバッグフィルター
/// </summary>
class DeviceManager {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="dxgiFactory">DXGIファクトリー</param>
	/// <param name="logger">ログ出力</param>
	void Initialize(IDXGIFactory7* dxgiFactory, Logger* logger);

	// 初期化内で呼ばれる関数----------

	void CreateD3D12Device();
	void SelectAdapter();
	void DebugFilter();

	// Getter----------
	Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() { return device_; }

private:
	// DXGIファクトリー
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;

	// 使用するアダプタ用の変数
	IDXGIAdapter4* useAdapter_ = nullptr;

	// D3D12デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;

	// ログ出力
	Logger* logger_ = nullptr;
};

