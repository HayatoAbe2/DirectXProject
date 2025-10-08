#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <string>

class Texture {
public:
	const std::string& GetMtlPath()const { return mtlFilePath; }
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetSRVHandle() const { return srvHandle_; }
	void SetSRVHandle(D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU) { srvHandle_ = srvHandleGPU; }
	void SetResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource) {
		resource_ = resource;
	}

	/// <summary>
	/// mtlファイルパス設定
	/// </summary>
	/// <param name="mtlPath">mtlファイルのパス</param>
	void SetMtlFilePath(const std::string& mtlPath) { mtlFilePath = mtlPath; }

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandle_ = {};
	std::string mtlFilePath;
};