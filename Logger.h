#pragma once
#include <ostream>
#include <fstream>
class Logger{
public:

	/// <summary>
	/// ログファイルの初期化
	/// </summary>
	Logger();

	~Logger();

	/// <summary>
	/// ログ出力
	/// </summary>
	/// <param name="os"></param>
	/// <param name="message">出力内容</param>
	void Log(std::ostream& os, const std::string& message);

	/// <summary>
	/// string->wstringへの変換
	/// </summary>
	/// <param name="str">string文字列</param>
	/// <returns>wstring文字列</returns>
	std::wstring ConvertString(const std::string& str);

	/// <summary>
	/// wstring->stringへの変換
	/// </summary>
	/// <param name="str">wstring文字列</param>
	/// <returns>string文字列</returns>
	std::string ConvertString(const std::wstring& str);

	std::ofstream& GetStream() {
		return logStream_;
	}

private:
	std::ofstream logStream_; // ログファイルのストリーム
};

