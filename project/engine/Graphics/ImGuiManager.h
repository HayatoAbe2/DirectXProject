#pragma once
#include <format>
#include <d3d12.h>
#include <dxcapi.h>
#include <mfobjects.h>

#ifdef USE_IMGUI
#include "externals/DirectXTex/d3dx12.h"
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#endif

class ImGuiManager {
public:
	void Initialize(HWND* hwnd, ID3D12Device* device, int bufferCount, DXGI_FORMAT format, ID3D12DescriptorHeap* srvHeap, D3D12_CPU_DESCRIPTOR_HANDLE srvCPUHandle, D3D12_GPU_DESCRIPTOR_HANDLE srvGPUHandle);
	void Finalize();

	void BeginFrame();
	void EndFrame(ID3D12GraphicsCommandList* cmdList);
};

