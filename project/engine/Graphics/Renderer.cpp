#include <Windows.h>
#include "Renderer.h"
#include "DirectXContext.h"
#include "CommandListManager.h"
#include "PipelineStateManager.h"
#include "RootSignatureManager.h"
#include "DescriptorHeapManager.h"
#include "../Object/Model.h"
#include "../Object/Sprite.h"

#include <cassert>
#include <format>
#include <dxcapi.h>
#include <mfobjects.h>
#include <numbers>

#include "externals/DirectXTex/d3dx12.h"
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"


void Renderer::Initialize(int32_t clientWidth, int32_t clientHeight, HWND hwnd, Logger* logger) {
	dxContext_ = std::make_unique<DirectXContext>();
	dxContext_.get()->Initialize(clientWidth, clientHeight, hwnd, logger);
}

void Renderer::Finalize() {
	dxContext_.get()->Finalize();
}

void Renderer::DrawModel(Model& model,int blendMode) {
	CommandListManager* cmdListManager = dxContext_.get()->GetCommandListManager();
	PipelineStateManager* psoManager = dxContext_.get()->GetPipelineStateManager();
	RootSignatureManager* rootSignatureManager = dxContext_.get()->GetRootSignatureManager();
	DescriptorHeapManager* descHeapManager = dxContext_.get()->GetDescriptorHeapManager();

	model.UpdateMaterial();
	
	// PSO設定
	cmdListManager->GetCommandList()->SetPipelineState(psoManager->GetStandardPSO(blendMode));
	// RootSignatureを設定
	cmdListManager->GetCommandList()->SetGraphicsRootSignature(rootSignatureManager->GetStandardRootSignature().Get());
	// 描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { descHeapManager->GetSRVHeap().Get() };
	cmdListManager->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);

	// トポロジを三角形に設定
	cmdListManager->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// マテリアルCBufferの場所を設定
	cmdListManager->GetCommandList()->SetGraphicsRootConstantBufferView(0, model.GetMaterialCBV());
	// モデル描画
	cmdListManager->GetCommandList()->IASetVertexBuffers(0, 1, &model.GetVBV());	// VBVを設定
	// wvp用のCBufferの場所を設定
	cmdListManager->GetCommandList()->SetGraphicsRootConstantBufferView(1, model.GetCBV());
	// SRVの設定
	if (model.GetTextureSRVHandle().ptr != 0) {
		cmdListManager->GetCommandList()->SetGraphicsRootDescriptorTable(2, model.GetTextureSRVHandle());
	}
	// ライト
	cmdListManager->GetCommandList()->SetGraphicsRootConstantBufferView(3, dxContext_.get()->GetLightResource()->GetGPUVirtualAddress());
	// ドローコール
	cmdListManager->GetCommandList()->DrawInstanced(UINT(model.GetVertices().size()), 1, 0, 0);
}

void Renderer::DrawModelInstance(Model& model, int blendMode) {
	CommandListManager* cmdListManager = dxContext_.get()->GetCommandListManager();
	PipelineStateManager* psoManager = dxContext_.get()->GetPipelineStateManager();
	RootSignatureManager* rootSignatureManager = dxContext_.get()->GetRootSignatureManager();
	DescriptorHeapManager* descHeapManager = dxContext_.get()->GetDescriptorHeapManager();

	model.UpdateMaterial();
	
	// PSO設定
	cmdListManager->GetCommandList()->SetPipelineState(psoManager->GetInstancingPSO(blendMode));
	// RootSignatureを設定
	cmdListManager->GetCommandList()->SetGraphicsRootSignature(rootSignatureManager->GetInstancingRootSignature().Get());
	// 描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { descHeapManager->GetSRVHeap().Get() };
	cmdListManager->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);

	// トポロジを三角形に設定
	cmdListManager->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// マテリアルCBufferの場所を設定
	cmdListManager->GetCommandList()->SetGraphicsRootConstantBufferView(0, model.GetMaterialCBV());
	// モデル描画
	cmdListManager->GetCommandList()->IASetVertexBuffers(0, 1, &model.GetVBV());	// VBVを設定
	// wvp用のCBufferの場所を設定
	cmdListManager->GetCommandList()->SetGraphicsRootConstantBufferView(1, model.GetInstanceCBV());
	// SRVの設定
	if (model.GetTextureSRVHandle().ptr != 0) {
	cmdListManager->GetCommandList()->SetGraphicsRootDescriptorTable(2, model.GetTextureSRVHandle());
	}
	// インスタンス用SRVの設定
	if (model.IsInstancing()) {
		cmdListManager->GetCommandList()->SetGraphicsRootDescriptorTable(4, model.GetInstanceSRVHandle());
	}
	// ドローコール
	cmdListManager->GetCommandList()->DrawInstanced(UINT(model.GetVertices().size()), model.GetNumInstance(), 0, 0);
}

void Renderer::DrawSprite(Sprite& sprite,int blendMode) {
	CommandListManager* cmdListManager = dxContext_.get()->GetCommandListManager();
	PipelineStateManager* psoManager = dxContext_.get()->GetPipelineStateManager();
	RootSignatureManager* rootSignatureManager = dxContext_.get()->GetRootSignatureManager();
	DescriptorHeapManager* descHeapManager = dxContext_.get()->GetDescriptorHeapManager();

	// PSO設定
	cmdListManager->GetCommandList()->SetPipelineState(psoManager->GetStandardPSO(blendMode));
	// RootSignatureを設定
	cmdListManager->GetCommandList()->SetGraphicsRootSignature(rootSignatureManager->GetStandardRootSignature().Get());
	// トポロジを三角形に設定
	cmdListManager->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// マテリアルCBufferの場所を設定
	cmdListManager->GetCommandList()->SetGraphicsRootConstantBufferView(0, sprite.GetCBV());
	// Spriteの描画。変更が必要なものだけ変更する
	cmdListManager->GetCommandList()->IASetIndexBuffer(&sprite.GetIBV());	// IBVを設定
	cmdListManager->GetCommandList()->IASetVertexBuffers(0, 1, &sprite.GetVBV());	// VBVを設定
	// TransformationMatrixCBufferの場所を設定
	cmdListManager->GetCommandList()->SetGraphicsRootConstantBufferView(1, sprite.GetCBV());
	// SRVの設定
	cmdListManager->GetCommandList()->SetGraphicsRootDescriptorTable(2, sprite.GetTextureSRVHandle());
	// ライト
	cmdListManager->GetCommandList()->SetGraphicsRootConstantBufferView(3, dxContext_.get()->GetLightResource()->GetGPUVirtualAddress());
	// 描画!(DrawCall/ドローコール)
	cmdListManager->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Renderer::BeginFrame() {
	dxContext_.get()->BeginFrame();
}

void Renderer::EndFrame() {
	dxContext_.get()->EndFrame();
}