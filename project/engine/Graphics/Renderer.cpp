#include <Windows.h>
#include "Renderer.h"
#include "DirectXContext.h"
#include "CommandListManager.h"
#include "PipelineStateManager.h"
#include "RootSignatureManager.h"
#include "DescriptorHeapManager.h"
#include "DeviceManager.h"
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
	dxContext_->Initialize(clientWidth, clientHeight, hwnd, logger);

	// エンティティtransformバッファ初期化
	UINT bufferSize = sizeof(TransformationMatrix) * kMaxObjects;

	// 定数バッファのアライメント（256の倍数）
	bufferSize = (bufferSize + 255) & ~255;

	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	dxContext_->GetDeviceManager()->GetDevice()->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&transformBuffer_)
	);

	// 一度だけMapして保持
	transformBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mappedTransformData_));

	InitializeImGui(hwnd);
}

void Renderer::Finalize() {
	dxContext_->Finalize();

	// ImGuiの終了処理
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Renderer::UpdateEntityTransforms(
	const Entity& entity,
	const Camera& camera) {
	TransformationMatrix data;
	data.World = MakeAffineMatrix(entity.GetTransform());
	data.WVP = data.World
		* camera.viewMatrix_
		* camera.projectionMatrix_;
	memcpy(mappedTransformData_ + sizeof(TransformationMatrix) * entity.GetID(), &data, sizeof(TransformationMatrix));
}

void Renderer::UpdateSpriteTransform(Entity& entity) {
	Transform transform{};
	Vector2 size = entity.GetSprite()->GetSize();
	Vector2 pos = entity.GetSprite()->GetPosition();
	float rot = entity.GetSprite()->GetRotation();
	Vector2 windowSize = { float(dxContext_->GetWindowWidth()),float(dxContext_->GetWindowHeight()) };

	transform.scale = { size.x, size.y, 1.0f };
	transform.translate = { pos.x,pos.y, 0.0f };
	transform.rotate = { 0.0f,0.0f,rot };
	// トランスフォーム
	TransformationMatrix data;
	data.World = MakeAffineMatrix(transform);
	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, windowSize.x, windowSize.y, 0.0f, 100.0f);
	data.WVP = Multiply(data.World, projectionMatrix);
	
	// コピー
	memcpy(mappedTransformData_ + sizeof(TransformationMatrix) * entity.GetID(), &data, sizeof(TransformationMatrix));
}

void Renderer::DrawEntity(Entity& entity, const Camera& camera, int blendMode) {
	if (entity.IsRenderable()) {
		// 各描画対象があれば描画する
		if (entity.GetModel()) {
			UpdateEntityTransforms(entity, camera);
			DrawModel(&entity, blendMode);
		}
		if (entity.GetSprite()) {
			UpdateSpriteTransform(entity);
			DrawSprite(&entity, blendMode);
		}
		/*if (entity->modelInstance)
			DrawEntityModelInstanced(*entity);
		if (entity->spriteInstance)
			DrawEntitySpriteInstanced(*entity);*/
	}
}

void Renderer::DrawModel(Entity* entity, int blendMode) {
	auto cmdList = dxContext_->GetCommandListManager()->GetCommandList();
	auto pso = dxContext_->GetPipelineStateManager()->GetStandardPSO(blendMode);
	auto rootSig = dxContext_->GetRootSignatureManager()->GetStandardRootSignature().Get();
	auto descHeap = dxContext_->GetDescriptorHeapManager()->GetSRVHeap().Get();

	// PSO設定
	cmdList->SetPipelineState(pso);
	// RootSignatureを設定
	cmdList->SetGraphicsRootSignature(rootSig);
	// 描画用のDescriptorHeapの設定
	cmdList->SetDescriptorHeaps(1, &descHeap);
	// トポロジを三角形に設定a
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 各メッシュを描画
	for (auto& mesh : entity->GetModel()->GetMeshes()) {

		// マテリアル更新
		mesh->UpdateMaterial();
		// マテリアルCBufferの場所を設定
		cmdList->SetGraphicsRootConstantBufferView(0, mesh->GetMaterialCBV());
		// モデル描画
		cmdList->IASetVertexBuffers(0, 1, &mesh->GetVBV());	// VBVを設定
		// WVPのCBV
		D3D12_GPU_VIRTUAL_ADDRESS cbAddress =
			transformBuffer_->GetGPUVirtualAddress() + sizeof(TransformationMatrix) * entity->GetID();
		cmdList->SetGraphicsRootConstantBufferView(1, cbAddress);

		// SRVの設定
		if (mesh->GetTextureSRVHandle().ptr != 0) {
			cmdList->SetGraphicsRootDescriptorTable(2, mesh->GetTextureSRVHandle());
		}
		// ライト
		cmdList->SetGraphicsRootConstantBufferView(3, dxContext_.get()->GetLightResource()->GetGPUVirtualAddress());
		// ドローコール
		cmdList->DrawInstanced(UINT(mesh->GetVertices().size()), 1, 0, 0);
	}
}

void Renderer::DrawModelInstance(Model& model, int blendMode) {
	auto cmdList = dxContext_->GetCommandListManager()->GetCommandList();
	auto pso = dxContext_->GetPipelineStateManager()->GetInstancingPSO(blendMode);
	auto rootSig = dxContext_->GetRootSignatureManager()->GetInstancingRootSignature().Get();
	auto descHeap = dxContext_->GetDescriptorHeapManager()->GetSRVHeap().Get();

	// PSO設定
	cmdList->SetPipelineState(pso);
	// RootSignatureを設定
	cmdList->SetGraphicsRootSignature(rootSig);
	// 描画用のDescriptorHeapの設定
	cmdList->SetDescriptorHeaps(1, &descHeap);
	// トポロジを三角形に設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 各メッシュを描画
	for (auto& mesh : model.GetMeshes()) {

		// マテリアル更新
		mesh->UpdateMaterial();
		// マテリアルCBufferの場所を設定
		cmdList->SetGraphicsRootConstantBufferView(0, mesh->GetMaterialCBV());
		// モデル描画
		cmdList->IASetVertexBuffers(0, 1, &mesh->GetVBV());	// VBVを設定
		// wvp用のCBufferの場所を設定
		cmdList->SetGraphicsRootConstantBufferView(1, model.GetInstanceCBV());
		// SRVの設定
		if (mesh->GetTextureSRVHandle().ptr != 0) {
			cmdList->SetGraphicsRootDescriptorTable(2, mesh->GetTextureSRVHandle());
		}
		// インスタンス用SRVの設定
		if (model.IsInstancing()) {
			cmdList->SetGraphicsRootDescriptorTable(4, model.GetInstanceSRVHandle());
		}
		// ドローコール
		cmdList->DrawInstanced(UINT(mesh->GetVertices().size()), model.GetNumInstance(), 0, 0);
	}
}

void Renderer::DrawSprite(Entity* entity, int blendMode) {
	auto cmdList = dxContext_->GetCommandListManager()->GetCommandList();
	auto pso = dxContext_->GetPipelineStateManager()->GetStandardPSO(blendMode);
	auto rootSig = dxContext_->GetRootSignatureManager()->GetStandardRootSignature().Get();
	auto descHeap = dxContext_->GetDescriptorHeapManager()->GetSRVHeap().Get();

	entity->GetSprite()->UpdateMaterial();
	// PSO設定
	cmdList->SetPipelineState(pso);
	// RootSignatureを設定
	cmdList->SetGraphicsRootSignature(rootSig);
	// 描画用のDescriptorHeapの設定
	cmdList->SetDescriptorHeaps(1, &descHeap);
	// トポロジを三角形に設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// マテリアルCBufferの場所を設定
	cmdList->SetGraphicsRootConstantBufferView(0, entity->GetSprite()->GetMaterialCBV());
	// Spriteの描画。変更が必要なものだけ変更する
	cmdList->IASetIndexBuffer(&entity->GetSprite()->GetIBV());	// IBVを設定
	cmdList->IASetVertexBuffers(0, 1, &entity->GetSprite()->GetVBV());	// VBVを設定
	// WVPのCBV
	D3D12_GPU_VIRTUAL_ADDRESS cbAddress =
		transformBuffer_->GetGPUVirtualAddress() + sizeof(TransformationMatrix) * entity->GetID();
	cmdList->SetGraphicsRootConstantBufferView(1, cbAddress);
	// SRVの設定
	cmdList->SetGraphicsRootDescriptorTable(2, entity->GetSprite()->GetTextureSRVHandle());
	// 描画!(DrawCall/ドローコール)
	cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Renderer::BeginFrame() {
	dxContext_.get()->BeginFrame();

	// ImGuiフレーム
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void Renderer::EndFrame() {
	// ImGuiの内部コマンドを生成する
	ImGui::Render();

	// 実際のcommandListのImGuiの描画コマンドを読む
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxContext_.get()->GetCommandListManager()->GetCommandList().Get());

	dxContext_.get()->EndFrame();
}