#pragma once
#include <wrl.h>
#include <d3d12.h>

class Logger;

/// <summary>
/// ルートシグネチャ管理
/// </summary>
class RootSignatureManager {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="logger">ログ出力</param>
	void Initialize(const Microsoft::WRL::ComPtr<ID3D12Device>& device, Logger* logger);

	/// <summary>
	/// RootSignatureを取得
	/// </summary>
	/// <returns></returns>
	Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature() { return rootSignature_.Get(); }

	/// <summary>
	/// errorBlobを取得
	/// </summary>
	/// <returns></returns>
	ID3D10Blob* GetErrorBlob() { return errorBlob_; }

private:

	ID3DBlob* signatureBlob_ = nullptr;
	ID3DBlob* errorBlob_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;

	// ログ出力
	Logger* logger_ = nullptr;
};

