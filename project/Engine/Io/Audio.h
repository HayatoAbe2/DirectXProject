#pragma once
#include <xaudio2.h>
#include <wrl.h>
#include <string>
#include <unordered_map>
class Audio {
	// 音声データ
	struct SoundData {
		// 波形フォーマット
		WAVEFORMATEX wfex;
		// バッファ
		std::vector<BYTE> buffer;
		// バッファのサイズ
		unsigned int bufferSize;
	};

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 終了時に呼び出す
	/// </summary>
	void Finalize();

	/// <summary>
	/// 音声の読み込み
	/// </summary>
	/// <param name="filename">ファイルパス</param>
	void SoundLoad(const wchar_t* filename);

	/// <summary>
	/// メモリ解放
	/// </summary>
	/// <param name="filename">ファイルパス</param>
	void SoundUnload(const wchar_t* filename);

	/// <summary>
	/// 再生
	/// </summary>
	/// <param name="filename">ファイルパス</param>
	void SoundPlay(const wchar_t* filename,bool isLoop = false,float volume = 1.0f);

	/// <summary>
	/// 止める
	/// </summary>
	void StopAll();


private:

	// ロードした音声データを格納
	std::unordered_map<std::wstring, SoundData> soundMap_;

	// XAudio2のインスタンス
	Microsoft::WRL::ComPtr<IXAudio2>xAudio2_;

	// マスターボイス
	IXAudio2MasteringVoice* masterVoice_;

	std::vector<IXAudio2SourceVoice*> voices_;

};

