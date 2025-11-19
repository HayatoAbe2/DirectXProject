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
#include "../Object/InstancedModel.h"
#include "../Object/ParticleSystem.h"

#include <cassert>
#include <format>
#include <dxcapi.h>
#include <mfobjects.h>
#include <numbers>

#include "externals/DirectXTex/d3dx12.h"

void Renderer::Initialize(int32_t clientWidth, int32_t clientHeight, HWND hwnd, Logger* logger) {
	dxContext_ = std::make_unique<DirectXContext>();
	dxContext_->Initialize(clientWidth, clientHeight, hwnd, logger);

	// エンティティtransformバッファ初期化
	UINT bufferSize = kCBSize * kMaxObjects;

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
	HRESULT hr = transformBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mappedTransformData_));
	assert(SUCCEEDED(hr));
}

void Renderer::Finalize() {
	dxContext_->Finalize();
}

void Renderer::UpdateEntityTransforms(
	const Entity& entity,
	const Camera& camera) {

	// トランスフォーム更新
	TransformationMatrix data;
	data.World = MakeAffineMatrix(entity.GetTransform());
	data.WVP = data.World
		* camera.viewMatrix_
		* camera.projectionMatrix_;
	memcpy(mappedTransformData_ + kCBSize * entity.GetID(), &data, kCBSize);
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
	memcpy(mappedTransformData_ + kCBSize * entity.GetID(), &data, kCBSize);
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
		if (entity.GetInstancedModel()) {
			entity.GetInstancedModel()->UpdateInstanceTransform(camera, entity.GetInstanceTransforms());
			DrawModelInstance(&entity, blendMode);
		}
		/*if (entity->spriteInstance)
			DrawEntitySpriteInstanced(*entity);*/
		if (entity.GetParticleSystem()) {
			entity.GetParticleSystem()->PreDraw(camera);
			DrawModelInstance(&entity, blendMode);
		}
	}
}

void Renderer::DrawModel(Entity* entity, int blendMode) {
	auto cmdList = dxContext_->GetCommandListManager()->GetCommandList();
	auto pso = dxContext_->GetPipelineStateManager()->GetStandardPSO(blendMode);
	auto rootSig = dxContext_->GetRootSignatureManager()->GetStandardRootSignature().Get();

	// PSO設定
	cmdList->SetPipelineState(pso);
	// RootSignatureを設定
	cmdList->SetGraphicsRootSignature(rootSig);
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
			transformBuffer_->GetGPUVirtualAddress() + kCBSize * entity->GetID();
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

void Renderer::DrawModelInstance(Entity* entity, int blendMode) {
	auto cmdList = dxContext_->GetCommandListManager()->GetCommandList();
	auto pso = dxContext_->GetPipelineStateManager()->GetInstancingPSO(blendMode);
	auto rootSig = dxContext_->GetRootSignatureManager()->GetInstancingRootSignature().Get();

	// PSO設定
	cmdList->SetPipelineState(pso);
	// RootSignatureを設定
	cmdList->SetGraphicsRootSignature(rootSig);
	// トポロジを三角形に設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 各メッシュを描画
	if (entity->GetInstancedModel()) {
		for (auto& mesh : entity->GetInstancedModel()->GetMeshes()) {

			// マテリアル更新
			mesh->UpdateMaterial();
			// マテリアルCBufferの場所を設定
			cmdList->SetGraphicsRootConstantBufferView(0, mesh->GetMaterialCBV());
			// モデル描画
			cmdList->IASetVertexBuffers(0, 1, &mesh->GetVBV());	// VBVを設定
			// wvp用のCBufferの場所を設定
			cmdList->SetGraphicsRootConstantBufferView(1, entity->GetInstancedModel()->GetInstanceCBV());
			// SRVの設定
			if (mesh->GetTextureSRVHandle().ptr != 0) {
				cmdList->SetGraphicsRootDescriptorTable(2, mesh->GetTextureSRVHandle());
			}
			// インスタンス用SRVの設定
			cmdList->SetGraphicsRootDescriptorTable(4, entity->GetInstancedModel()->GetInstanceSRVHandle());
			// ドローコール
			cmdList->DrawInstanced(UINT(mesh->GetVertices().size()), entity->GetInstancedModel()->GetNumInstance(), 0, 0);
		}
	} else {
		for (auto& mesh : entity->GetParticleSystem()->GetInstancedModel_()->GetMeshes()) {

			// マテリアル更新
			mesh->UpdateMaterial();
			// マテリアルCBufferの場所を設定
			cmdList->SetGraphicsRootConstantBufferView(0, mesh->GetMaterialCBV());
			// モデル描画
			cmdList->IASetVertexBuffers(0, 1, &mesh->GetVBV());	// VBVを設定
			// wvp用のCBufferの場所を設定
			cmdList->SetGraphicsRootConstantBufferView(1, entity->GetParticleSystem()->GetInstancedModel_()->GetInstanceCBV());
			// SRVの設定
			if (mesh->GetTextureSRVHandle().ptr != 0) {
				cmdList->SetGraphicsRootDescriptorTable(2, mesh->GetTextureSRVHandle());
			}
			// インスタンス用SRVの設定
			cmdList->SetGraphicsRootDescriptorTable(4, entity->GetParticleSystem()->GetInstancedModel_()->GetInstanceSRVHandle());
			// ドローコール
			cmdList->DrawInstanced(UINT(mesh->GetVertices().size()), entity->GetParticleSystem()->GetInstancedModel_()->GetNumInstance(), 0, 0);
		}
	}
}

void Renderer::DrawSprite(Entity* entity, int blendMode) {
	auto cmdList = dxContext_->GetCommandListManager()->GetCommandList();
	auto pso = dxContext_->GetPipelineStateManager()->GetStandardPSO(blendMode);
	auto rootSig = dxContext_->GetRootSignatureManager()->GetStandardRootSignature().Get();

	entity->GetSprite()->UpdateMaterial();
	// PSO設定
	cmdList->SetPipelineState(pso);
	// RootSignatureを設定
	cmdList->SetGraphicsRootSignature(rootSig);
	// トポロジを三角形に設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// マテリアルCBufferの場所を設定
	cmdList->SetGraphicsRootConstantBufferView(0, entity->GetSprite()->GetMaterialCBV());
	// Spriteの描画。変更が必要なものだけ変更する
	cmdList->IASetIndexBuffer(&entity->GetSprite()->GetIBV());	// IBVを設定
	cmdList->IASetVertexBuffers(0, 1, &entity->GetSprite()->GetVBV());	// VBVを設定
	// WVPのCBV
	D3D12_GPU_VIRTUAL_ADDRESS cbAddress =
		transformBuffer_->GetGPUVirtualAddress() + kCBSize * entity->GetID();
	cmdList->SetGraphicsRootConstantBufferView(1, cbAddress);
	// SRVの設定
	cmdList->SetGraphicsRootDescriptorTable(2, entity->GetSprite()->GetTextureSRVHandle());
	// 描画!(DrawCall/ドローコール)
	cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Renderer::BeginFrame() {
	dxContext_.get()->BeginFrame();
}

void Renderer::EndFrame() {
	dxContext_.get()->EndFrame();
}