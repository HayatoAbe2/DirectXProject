#pragma once
#include <string>
#include <Windows.h>
#include <dxcapi.h>
#include <wrl.h>

class Logger;
class ShaderCompiler {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// シェーダーコンパイル
	/// </summary>
	/// <param name="filePath">hlslファイルパス</param>
	/// <param name="profile">ShaderProfile</param>
	/// <param name="logger">ログポインタ</param>
	/// <returns></returns>
	Microsoft::WRL::ComPtr<IDxcBlob> Compile(const std::wstring& filePath, const wchar_t* profile, Logger* logger);

private:
	// dxc
	IDxcUtils* dxcUtils_ = nullptr;
	IDxcCompiler3* dxcCompiler_ = nullptr;
	IDxcIncludeHandler* includeHandler_ = nullptr;
};

