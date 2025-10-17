#pragma once
#include <d3d12.h>
#include <cstdint>
#include <wrl.h>

class DeviceManager;

/// <summary>
/// コマンドリスト・キュー・アロケータ、フェンスの管理
/// </summary>
class CommandListManager {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="deviceManager">デバイスマネージャー</param>
	void Initialize(DeviceManager* deviceManager);

	// 初期化内で呼ばれる関数----------

	/// <summary>
	/// Fence作成
	/// </summary>
	void InitializeFence();

	/// <summary>
	/// CommandQueue作成
	/// </summary>
	void InitializeCommandQueue();

	/// <summary>
	/// CommandAllocator作成
	/// </summary>
	void InitializeCommandAllocator();

	/// <summary>
	/// CommandList作成
	/// </summary>
	void InitializeCommandList();

	// アクセサ----------
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetCommandQueue() { return commandQueue_; }
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCommandList() { return commandList_; }
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> GetCommandAllocator() { return commandAllocator_; }
	Microsoft::WRL::ComPtr<ID3D12Fence> GetFence() { return fence_; }
	
	/// <summary>
	/// コマンドリストを実行して待機
	/// </summary>
	void ExecuteAndWait();

	void Wait();

	void Reset();

private:

	// フェンス
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_ = nullptr;
	HANDLE fenceEvent_ = nullptr;
	uint64_t fenceValue_ = 0;

	// コマンドキュー
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc_{};
	// コマンドアロケータ
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;
	// コマンドリスト
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;

	// デバイスマネージャー
	DeviceManager* deviceManager_ = nullptr;
};

