#include <Windows.h>
#include "Renderer.h"
#include "CommandListManager.h"
#include "PipelineStateManager.h"
#include "RootSignatureManager.h"
#include "DescriptorHeapManager.h"
#include "DeviceManager.h"
#include "../Object/Model.h"
#include "../Object/InstancedModel.h"
#include "../Object/Sprite.h"
#include "../Object/ParticleSystem.h"
#include "../Scene/Camera.h"
#include "../Graphics/DirectXContext.h"

#include <cassert>
#include <format>
#include <dxcapi.h>
#include <mfobjects.h>
#include <numbers>

#include "externals/DirectXTex/d3dx12.h"

void Renderer::Initialize(DirectXContext* dxContext) {
	dxContext_ = dxContext;

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

	// カメラバッファ作成
	cameraBuffer_ = dxContext_->CreateBufferResource(sizeof(CameraForGPU));
	cameraBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));

	// ライト非使用時のダミー
	dummyLightBuffer_ = dxContext_->CreateBufferResource(sizeof(LightsForGPU));
	dummyLightBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&dummyLight_));
}


void Renderer::UpdateModelTransforms(
	Model* model,
	Camera* camera) {

	// トランスフォーム更新
	TransformationMatrix data;
	data.World = MakeAffineMatrix(model->GetTransform());
	data.WVP = 
		model->GetData()->meshes[0]->rootNode.localMatrix *
		data.World
		* camera->viewMatrix_
		* camera->projectionMatrix_;
	data.WorldInverseTranspose = Transpose(Inverse(data.World));
	memcpy(mappedTransformData_ + kCBSize * model->GetTransformCBHandle(), &data, kCBSize);
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

	// コピー
	memcpy(mappedTransformData_ + kCBSize * sprite->GetTransformCBHandle(), &data, kCBSize);
}

void Renderer::DrawModel(Model* model, Camera* camera, LightManager* lightManager, int blendMode) {
	// GPUに渡すデータの更新
	UpdateModelTransforms(model,camera);
	cameraData_->position = camera->transform_.translate;
	lightManager->Update();

	auto cmdList = dxContext_->GetCommandListManager()->GetCommandList();
	auto pso = dxContext_->GetPipelineStateManager()->GetStandardPSO(blendMode);
	auto rootSig = dxContext_->GetRootSignatureManager()->GetStandardRootSignature().Get();

	// PSO設定
	cmdList->SetPipelineState(pso);
	// RootSignatureを設定
	cmdList->SetGraphicsRootSignature(rootSig);
	// トポロジを三角形に設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 各メッシュを描画
	for (auto& mesh : model->GetData()->meshes) {
		Material* material = model->GetMaterial(0); // 複数マテリアル未対応
		// マテリアル更新
		material->UpdateGPU();

		// マテリアルCBufferの場所を設定
		cmdList->SetGraphicsRootConstantBufferView(0, material->GetCBV()->GetGPUVirtualAddress());
		// メッシュVBV
		cmdList->IASetVertexBuffers(0, 1, &mesh->GetVBV());	// VBVを設定
		// トランスフォームCBV
		D3D12_GPU_VIRTUAL_ADDRESS cbAddress =
			transformBuffer_->GetGPUVirtualAddress() + kCBSize * model->GetTransformCBHandle();
		cmdList->SetGraphicsRootConstantBufferView(1, cbAddress);

		// SRVの設定
		cmdList->SetGraphicsRootDescriptorTable(2, material->GetTextureSRVHandle());
		// カメラ
		cmdList->SetGraphicsRootConstantBufferView(3, cameraBuffer_->GetGPUVirtualAddress());
		// ライト
		cmdList->SetGraphicsRootConstantBufferView(4, lightManager->GetLightResource()->GetGPUVirtualAddress());
		// ドローコール
		cmdList->DrawInstanced(UINT(mesh->GetVertices().size()), 1, 0, 0);
	}
}

void Renderer::DrawModelInstance(InstancedModel* model, Camera* camera, LightManager* lightManager, int blendMode) {
	// GPUに渡すデータの更新
	std::vector<Vector4> colors;
	colors.resize(model->GetNumInstance());
	for (auto& color : colors) {
		color = { 1,1,1,1 };
	}
	model->UpdateInstanceTransform(camera, model->GetTransforms(), colors);
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

	// 各メッシュを描画
	for (auto& mesh : model->GetData()->meshes) {
		Material* material = model->GetMaterial(0); // 複数マテリアル未対応
		// マテリアル更新
		material->UpdateGPU();

		// マテリアルCBufferの場所を設定
		cmdList->SetGraphicsRootConstantBufferView(0, material->GetCBV()->GetGPUVirtualAddress());
		// モデル描画
		cmdList->IASetVertexBuffers(0, 1, &mesh->GetVBV());	// VBVを設定
		// wvp用のCBufferの場所を設定
		cmdList->SetGraphicsRootConstantBufferView(1, model->GetInstanceCBV());
		// SRVの設定
		cmdList->SetGraphicsRootDescriptorTable(2, material->GetTextureSRVHandle());
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
		// ドローコール
		cmdList->DrawInstanced(UINT(mesh->GetVertices().size()), model->GetNumInstance(), 0, 0);
	}
}

void Renderer::DrawParticles(ParticleSystem* particleSys, Camera* camera,int blendMode) {
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
		Material* material = particleSys->GetInstancedModel()->GetMaterial(0); // 複数マテリアル未対応
		// マテリアル更新
		material->UpdateGPU();

		// マテリアルCBufferの場所を設定
		cmdList->SetGraphicsRootConstantBufferView(0, material->GetCBV()->GetGPUVirtualAddress());
		// モデル描画
		cmdList->IASetVertexBuffers(0, 1, &mesh->GetVBV());	// VBVを設定
		// wvp用のCBufferの場所を設定
		cmdList->SetGraphicsRootConstantBufferView(1, particleSys->GetInstancedModel()->GetInstanceCBV());
		// SRVの設定
		cmdList->SetGraphicsRootDescriptorTable(2, material->GetTextureSRVHandle());
		// インスタンス用SRVの設定
		cmdList->SetGraphicsRootDescriptorTable(3, particleSys->GetInstancedModel()->GetInstanceSRVHandle());
		// ドローコール
		cmdList->DrawInstanced(UINT(mesh->GetVertices().size()), particleSys->GetInstancedModel()->GetNumInstance(), 0, 0);
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
	// WVPのCBV
	D3D12_GPU_VIRTUAL_ADDRESS cbAddress =
		transformBuffer_->GetGPUVirtualAddress() + kCBSize * sprite->GetTransformCBHandle();
	cmdList->SetGraphicsRootConstantBufferView(1, cbAddress);
	// SRVの設定
	cmdList->SetGraphicsRootDescriptorTable(2, sprite->GetTextureSRVHandle());
	// 描画!(DrawCall/ドローコール)
	cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Renderer::BeginFrame() {
	dxContext_->BeginFrame();
}

void Renderer::EndFrame() {
	dxContext_->EndFrame();
}