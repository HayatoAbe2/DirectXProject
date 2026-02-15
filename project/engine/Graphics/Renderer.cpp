#include <Windows.h>
#include "Renderer.h"
#include "CommandListManager.h"
#include "PipelineStateManager.h"
#include "RootSignatureManager.h"
#include "DescriptorHeapManager.h"
#include "DeviceManager.h"
#include "Asset/Model/Model.h"
#include "Asset/Model/InstancedModel.h"
#include "Asset/Sprite.h"
#include "Object/ParticleSystem.h"
#include "Scene/Camera.h"
#include "Graphics/DirectXContext.h"
#include "Graphics/BufferManager.h"

#include <cassert>
#include <format>
#include <dxcapi.h>
#include <mfobjects.h>
#include <numbers>

#include "externals/DirectXTex/d3dx12.h"

void Renderer::Initialize(DirectXContext* dxContext) {
	dxContext_ = dxContext;

	// デバイス
	auto device = dxContext_->GetDeviceManager()->GetDevice();

	// カメラバッファ作成
	cameraBuffer_ = dxContext_->GetBufferManager()->CreateUploadBuffer(sizeof(CameraForGPU));
	cameraBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));

	// ライト非使用時のダミー
	dummyLightBuffer_ = dxContext_->GetBufferManager()->CreateUploadBuffer(sizeof(LightsForGPU));
	dummyLightBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&dummyLight_));
}

void Renderer::UpdateSpriteTransform(Sprite* sprite) {
	Transform transform{};
	Vector2 size = sprite->GetSize();
	Vector2 pos = sprite->GetPosition();
	float rot = sprite->GetRotation();
	Vector2 windowSize = { float(dxContext_->GetWindowWidth()),float(dxContext_->GetWindowHeight()) };

	transform.scale = { size.x, size.y, 1.0f };
	transform.translate = { pos.x,pos.y, 0.0f };
	transform.rotate = { 0.0f,0.0f,rot };
	// トランスフォーム
	TransformationMatrix data;
	data.World = MakeAffineMatrix(transform);
	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, windowSize.x, windowSize.y, 0.0f, 100.0f);
	data.WVP = Multiply(data.World, projectionMatrix);
	data.WorldInverseTranspose = Transpose(Inverse(data.World));
	auto ptr = dxContext_->GetConstantBufferManager()->GetTransformPtr(sprite->GetTransformCBHandle());
	memcpy(ptr, &data, sizeof(data));
}

void Renderer::DrawModel(Model* model, Camera* camera, LightManager* lightManager, int blendMode) {
	// GPUに渡すデータの更新
	cameraData_->position = camera->transform_.translate;
	if (lightManager) { lightManager->Update(); }

	auto cmdList = dxContext_->GetCommandListManager()->GetCommandList();
	auto pso = dxContext_->GetPipelineStateManager()->GetStandardPSO(blendMode);
	auto rootSig = dxContext_->GetRootSignatureManager()->GetStandardRootSignature().Get();

	// PSO設定
	cmdList->SetPipelineState(pso);
	// RootSignatureを設定
	cmdList->SetGraphicsRootSignature(rootSig);
	// トポロジを三角形に設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// カメラ
	cmdList->SetGraphicsRootConstantBufferView(3, cameraBuffer_->GetGPUVirtualAddress());
	// ライト
	cmdList->SetGraphicsRootConstantBufferView(4, lightManager->GetLightResource()->GetGPUVirtualAddress());

	DrawNode(model, camera, cmdList, model->GetRootNode(), MakeIdentity4x4());
}

void Renderer::DrawModelInstance(InstancedModel* model, Camera* camera, LightManager* lightManager, int blendMode) {
	// GPUに渡すデータの更新
	cameraData_->position = camera->transform_.translate;
	if (lightManager) { lightManager->Update(); }

	auto cmdList = dxContext_->GetCommandListManager()->GetCommandList();
	auto pso = dxContext_->GetPipelineStateManager()->GetInstancingPSO(blendMode);
	auto rootSig = dxContext_->GetRootSignatureManager()->GetInstancingRootSignature().Get();

	// PSO設定
	cmdList->SetPipelineState(pso);
	// RootSignatureを設定
	cmdList->SetGraphicsRootSignature(rootSig);
	// トポロジを三角形に設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// wvp用のCBufferの場所を設定
	cmdList->SetGraphicsRootConstantBufferView(1, model->GetInstanceCBV());
	// インスタンス用SRVの設定
	cmdList->SetGraphicsRootDescriptorTable(3, model->GetInstanceSRVHandle());
	// カメラ
	cmdList->SetGraphicsRootConstantBufferView(4, cameraBuffer_->GetGPUVirtualAddress());
	if (lightManager) {
		// ライト
		cmdList->SetGraphicsRootConstantBufferView(5, lightManager->GetLightResource()->GetGPUVirtualAddress());
	} else {
		cmdList->SetGraphicsRootConstantBufferView(5, dummyLightBuffer_->GetGPUVirtualAddress());
	}

	DrawNodeInstance(model, camera, cmdList, model->GetRootNode(), MakeIdentity4x4());
}

void Renderer::DrawParticles(ParticleSystem* particleSys, Camera* camera, int blendMode) {
	particleSys->PreDraw(camera);

	auto cmdList = dxContext_->GetCommandListManager()->GetCommandList();
	auto pso = dxContext_->GetPipelineStateManager()->GetParticlePSO(blendMode);
	auto rootSig = dxContext_->GetRootSignatureManager()->GetParticleRootSignature().Get();

	// PSO設定
	cmdList->SetPipelineState(pso);
	// RootSignatureを設定
	cmdList->SetGraphicsRootSignature(rootSig);
	// トポロジを三角形に設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 各メッシュを描画
	for (auto& mesh : particleSys->GetInstancedModel()->GetData()->meshes) {
		for (const auto& subMesh : mesh->GetPrimitives()) {

			Material* material = particleSys->GetInstancedModel()->GetMaterial(subMesh.materialIndex_);
			// マテリアル更新
			material->UpdateGPU();

			// マテリアルCBufferの場所を設定
			cmdList->SetGraphicsRootConstantBufferView(0, material->GetCBV()->GetGPUVirtualAddress());
			// モデル描画
			cmdList->IASetVertexBuffers(0, 1, &subMesh.vertexBufferView_);	// VBVを設定
			// wvp用のCBufferの場所を設定
			cmdList->SetGraphicsRootConstantBufferView(1, particleSys->GetInstancedModel()->GetInstanceCBV());
			// SRVの設定
			cmdList->SetGraphicsRootDescriptorTable(2, material->GetTextureSRVHandle());
			// インスタンス用SRVの設定
			cmdList->SetGraphicsRootDescriptorTable(3, particleSys->GetInstancedModel()->GetInstanceSRVHandle());
			// ドローコール
			cmdList->DrawInstanced(UINT(subMesh.vertices_.size()), particleSys->GetInstancedModel()->GetNumInstance(), 0, 0);
		}
	}
}

void Renderer::DrawSprite(Sprite* sprite, int blendMode) {
	UpdateSpriteTransform(sprite);

	auto cmdList = dxContext_->GetCommandListManager()->GetCommandList();
	auto pso = dxContext_->GetPipelineStateManager()->GetStandardPSO(blendMode);
	auto rootSig = dxContext_->GetRootSignatureManager()->GetStandardRootSignature().Get();

	sprite->UpdateMaterial();
	// PSO設定
	cmdList->SetPipelineState(pso);
	// RootSignatureを設定
	cmdList->SetGraphicsRootSignature(rootSig);
	// トポロジを三角形に設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// マテリアルCBufferの場所を設定
	cmdList->SetGraphicsRootConstantBufferView(0, sprite->GetMaterialCBV());
	// Spriteの描画。変更が必要なものだけ変更する
	cmdList->IASetIndexBuffer(&sprite->GetIBV());	// IBVを設定
	cmdList->IASetVertexBuffers(0, 1, &sprite->GetVBV());	// VBVを設定
	// トランスフォームCBV
	auto cbAddress = dxContext_->GetConstantBufferManager()->GetTransformCBAddress(sprite->GetTransformCBHandle());
	cmdList->SetGraphicsRootConstantBufferView(1, cbAddress);
	// SRVの設定
	cmdList->SetGraphicsRootDescriptorTable(2, sprite->GetTextureSRVHandle());
	// 描画!(DrawCall/ドローコール)
	cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Renderer::DrawNode(Model* model, Camera* camera, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList, ModelNode* node, const Matrix4x4& parentWorld) {
	Matrix4x4 modelWorld = MakeAffineMatrix(model->GetTransform());
	Matrix4x4 nodeWorld = parentWorld * node->localMatrix;

	// トランスフォーム更新
	TransformationMatrix data;
	data.World = modelWorld * nodeWorld;
	data.WVP = data.World
		* camera->viewMatrix_
		* camera->projectionMatrix_;
	data.WorldInverseTranspose = Transpose(Inverse(data.World));
	auto ptr = dxContext_->GetConstantBufferManager()->GetTransformPtr(model->GetTransformCBHandle());
	memcpy(ptr, &data, sizeof(data));

	// cbアドレス
	auto cbAddress = dxContext_->GetConstantBufferManager()->GetTransformCBAddress(model->GetTransformCBHandle());
	cmdList->SetGraphicsRootConstantBufferView(1, cbAddress);

	// メッシュを描画
	for (uint32_t meshIndex : node->meshIndices) {
		DrawMesh(model, model->GetData()->meshes[meshIndex].get());
	}

	// 子ノード
	for (auto& child : node->children) {
		DrawNode(model, camera, cmdList, child.get(), nodeWorld);
	}
}

void Renderer::DrawMesh(Model* model, Mesh* mesh) {
	auto cmdList = dxContext_->GetCommandListManager()->GetCommandList();

	for (const auto& subMesh : mesh->GetPrimitives()) {
		Material* material = model->GetMaterial(subMesh.materialIndex_);

		// マテリアル更新
		material->UpdateGPU();

		// マテリアルCBufferの場所を設定
		cmdList->SetGraphicsRootConstantBufferView(0, material->GetCBV()->GetGPUVirtualAddress());
		// メッシュVBV
		cmdList->IASetVertexBuffers(0, 1, &subMesh.vertexBufferView_);	// VBVを設定
		// IBV
		cmdList->IASetIndexBuffer(&subMesh.ibv_);
		// SRVの設定
		cmdList->SetGraphicsRootDescriptorTable(2, material->GetTextureSRVHandle());
		// ドローコール
		cmdList->DrawIndexedInstanced(UINT(subMesh.indices_.size()), 1, 0, 0, 0);
	}
}

void Renderer::DrawNodeInstance(InstancedModel* model, Camera* camera, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList, ModelNode* node, const Matrix4x4& parentWorld) {
	Matrix4x4 nodeWorld = parentWorld * node->localMatrix;

	// トランスフォーム更新
	std::vector<Vector4> colors;
	colors.resize(model->GetNumInstance());
	for (auto& color : colors) {
		color = { 1,1,1,1 };
	}
	model->UpdateInstanceTransformWithNode(camera, nodeWorld, colors);

	// メッシュを描画
	for (uint32_t meshIndex : node->meshIndices) {
		DrawMeshInstance(model, model->GetData()->meshes[meshIndex].get());
	}

	// 子ノード
	for (auto& child : node->children) {
		DrawNodeInstance(model, camera, cmdList, child.get(), nodeWorld);
	}
}

void Renderer::DrawMeshInstance(InstancedModel* model, Mesh* mesh) {
	auto cmdList = dxContext_->GetCommandListManager()->GetCommandList();

	// 各メッシュを描画
	for (const auto& subMesh : mesh->GetPrimitives()) {
		Material* material = model->GetMaterial(subMesh.materialIndex_);

		// マテリアル更新
		material->UpdateGPU();

		// マテリアルCBufferの場所を設定
		cmdList->SetGraphicsRootConstantBufferView(0, material->GetCBV()->GetGPUVirtualAddress());
		// モデル描画
		cmdList->IASetVertexBuffers(0, 1, &subMesh.vertexBufferView_);	// VBVを設定
		// SRVの設定
		cmdList->SetGraphicsRootDescriptorTable(2, material->GetTextureSRVHandle());
		// ドローコール
		cmdList->DrawInstanced(UINT(subMesh.vertices_.size()), model->GetNumInstance(), 0, 0);
	}
}

void Renderer::BeginFrame() {
	dxContext_->BeginFrame();
}

void Renderer::EndFrame() {
	dxContext_->EndFrame();
}