#include "RootSignatureManager.h"
#include "DeviceManager.h"
#include "Logger.h"

#include <cassert>
#include <dxcapi.h>

void RootSignatureManager::Initialize(const Microsoft::WRL::ComPtr<ID3D12Device>& device, Logger* logger) {
	logger_ = logger;
	device_ = device;

	CreateStandardRootSignature();
	CreateInstancingRootSignature();
}

void RootSignatureManager::CreateStandardRootSignature() {
	HRESULT hr;
	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成
	D3D12_ROOT_PARAMETER rootParameters[5] = {};

	// 0(マテリアルCBV)
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;		// PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;						// レジスタ番号0を使う

	// 1(トランスフォームCBV)
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;	// VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0;						// レジスタ番号0を使う

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;	// 0から始まる
	descriptorRange[0].NumDescriptors = 1;		// 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	// SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;	// Offsetを自動計算

	// 2(テクスチャSRV)
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;					// DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;								// PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;							// Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);				// Tableで利用する数

	// 3(カメラ)
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;		// PixelShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 1;						// レジスタ番号1を使う

	// 4(Light)
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;		// PixelShaderで使う
	rootParameters[4].Descriptor.ShaderRegister = 2;						// レジスタ番号2を使う

	// -------------------------

	descriptionRootSignature.pParameters = rootParameters;				// ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);		// 配列の長さ

	// Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0~1の範囲をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; // LODの最大値
	staticSamplers[0].ShaderRegister = 0; // レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
	if (FAILED(hr)) {
		logger_->Log(logger_->GetStream(), reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
		assert(false);
	}
	// バイナリを元に生成
	standardRootSignature_ = nullptr;
	hr = device_->CreateRootSignature(0,
		signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(),
		IID_PPV_ARGS(&standardRootSignature_));
	assert(SUCCEEDED(hr));

}

void RootSignatureManager::CreateInstancingRootSignature() {
	D3D12_ROOT_PARAMETER rootParameters[5] = {};

	// 0: Material CBV (PixelShader) - スロットは通常と同じ
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0;

	// 1: Transform CBV (VertexShader)
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 0;

	// 2: Texture SRV (PixelShader)
	D3D12_DESCRIPTOR_RANGE textureRange[1]{};
	textureRange[0].BaseShaderRegister = 0;
	textureRange[0].NumDescriptors = 1;
	textureRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	textureRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[2].DescriptorTable.pDescriptorRanges = textureRange;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(textureRange);

	// 3: Optional CBV (PixelShader)
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[3].Descriptor.ShaderRegister = 1;

	// 4: Instance SRV (VertexShader)
	D3D12_DESCRIPTOR_RANGE instanceRange[1]{};
	instanceRange[0].BaseShaderRegister = 1;
	instanceRange[0].NumDescriptors = 1;
	instanceRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	instanceRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[4].DescriptorTable.pDescriptorRanges = instanceRange;
	rootParameters[4].DescriptorTable.NumDescriptorRanges = _countof(instanceRange);

	D3D12_ROOT_SIGNATURE_DESC desc{};
	desc.NumParameters = _countof(rootParameters);
	desc.pParameters = rootParameters;
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// Sampler
	D3D12_STATIC_SAMPLER_DESC sampler[1]{};
	sampler[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	sampler[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler[0].ShaderRegister = 0;
	sampler[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	sampler[0].MaxLOD = D3D12_FLOAT32_MAX;

	desc.NumStaticSamplers = _countof(sampler);
	desc.pStaticSamplers = sampler;


	HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlobInstancing_, &errorBlobInstancing_);
	if (FAILED(hr)) {
		logger_->Log(logger_->GetStream(), reinterpret_cast<char*>(errorBlobInstancing_->GetBufferPointer()));
		assert(false);
	}

	hr = device_->CreateRootSignature(0, signatureBlobInstancing_->GetBufferPointer(), signatureBlobInstancing_->GetBufferSize(), IID_PPV_ARGS(&instancingRootSignature_));
	assert(SUCCEEDED(hr));
}