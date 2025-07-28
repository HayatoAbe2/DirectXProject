#pragma once
#include <wrl.h>

/// <summary>
/// ダンプファイルの出力
/// </summary>
class DumpExporter {
public:
	
	static LONG __stdcall ExportDump(EXCEPTION_POINTERS* exception);
};

