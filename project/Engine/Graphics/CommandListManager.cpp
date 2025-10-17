#include "CommandListManager.h"
#include "DeviceManager.h"

#include <cassert>

void CommandListManager::Initialize(DeviceManager* deviceManager) {
	deviceManager_ = deviceManager;

	// フェンス生成
	InitializeFence();
	// コマンドキュー生成
	InitializeCommandQueue();
	// コマンドアロケータ生成
	InitializeCommandAllocator();
	// コマンドリスト生成
	InitializeCommandList();
}

void CommandListManager::InitializeFence() {
	HRESULT hr;
	// 初期値0でFenceを作る
	uint64_t fenceValue_ = 0;
	hr = deviceManager_->GetDevice()->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr));

	// FenceのSignalを待つためのイベントを作成する
	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent_ != nullptr);
}

void CommandListManager::InitializeCommandQueue() {
	HRESULT hr;
	// コマンドキューを生成する
	hr = deviceManager_->GetDevice()->CreateCommandQueue(&commandQueueDesc_, IID_PPV_ARGS(&commandQueue_));
	// コマンドキューの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));
}

void CommandListManager::InitializeCommandAllocator() {
	HRESULT hr;
	// コマンドアロケータを生成する
	hr = deviceManager_->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	// コマンドアロケータの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));
}

void CommandListManager::InitializeCommandList() {
	HRESULT hr;
	// コマンドリストを生成する
	hr = deviceManager_->GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_));
	// コマンドリストの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));
}

void CommandListManager::ExecuteAndWait() {
	assert(commandList_);
	assert(commandQueue_);
	assert(fence_);
	assert(commandAllocator_);

	// commandListをCloseし、commandQueue->ExecuteCommandListsを使いキックする
	commandList_->Close();
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(_countof(commandLists), commandLists);


	Wait();
	Reset();
}

void CommandListManager::Wait() {
	// 実行を待つ
	fenceValue_++;
	commandQueue_->Signal(fence_.Get(), fenceValue_); // シグナルを送る
	if (fence_->GetCompletedValue() < fenceValue_) {
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		WaitForSingleObject(fenceEvent_, INFINITE); // 待機
	}
}

void CommandListManager::Reset() {
	// 実行が完了したので、allocatorとcommandListをResetして次のコマンドを積めるようにする
	commandAllocator_->Reset();
	commandList_->Reset(commandAllocator_.Get(), nullptr);
}