#include "Audio.h"
#include <cassert>
#include <mfapi.h>
#include <mfidl.h>
#include <mfobjects.h>
#include <mfreadwrite.h>
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfplat.lib")
#include <vector>

void Audio::Initialize() {
	HRESULT hr;
	// XAudioエンジンのインスタンスを生成する
	hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));
	// マスターボイスを生成する
	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(hr));
	// MFの初期化
	hr = MFStartup(MF_VERSION,MFSTARTUP_NOSOCKET);
	assert(SUCCEEDED(hr));
}

void Audio::Finalize() {
	HRESULT result;

	StopAll();

	if (masterVoice_) {
		masterVoice_->DestroyVoice();
		masterVoice_ = nullptr;
	}

	xAudio2_.Reset();

	result = MFShutdown();
	assert(SUCCEEDED(result));
}

void Audio::SoundLoad(const wchar_t* filename) {
	// 読み込み済みなら何もしない
	if (soundMap_.contains(filename)) return;

	HRESULT hr;
	// ソースリーダー作成
	IMFSourceReader* pMFSourceReader = nullptr;
	hr = MFCreateSourceReaderFromURL(filename, NULL, &pMFSourceReader);
	assert(SUCCEEDED(hr));

	// メディアタイプの取得
	IMFMediaType* pMFMediaType = nullptr;
	hr = MFCreateMediaType(&pMFMediaType);
	assert(SUCCEEDED(hr));
	pMFMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	pMFMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	hr = pMFSourceReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pMFMediaType);
	assert(SUCCEEDED(hr));

	pMFMediaType->Release();
	pMFMediaType = nullptr;
	pMFSourceReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pMFMediaType);

	// データ形式
	WAVEFORMATEX* waveFormat = nullptr;
	hr = MFCreateWaveFormatExFromMFMediaType(pMFMediaType, &waveFormat, nullptr);
	assert(SUCCEEDED(hr));

	std::vector<BYTE> mediaData;
	while (true)
	{
		IMFSample* pMFSample{ nullptr };
		DWORD dwStreamFlags{ 0 };
		hr = pMFSourceReader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &dwStreamFlags, nullptr, &pMFSample);
		assert(SUCCEEDED(hr));

		if (dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM)
		{
			break;
		}

		IMFMediaBuffer* pMFMediaBuffer{ nullptr };
		pMFSample->ConvertToContiguousBuffer(&pMFMediaBuffer);

		BYTE* pBuffer{ nullptr };
		DWORD cbCurrentLength{ 0 };
		pMFMediaBuffer->Lock(&pBuffer, nullptr, &cbCurrentLength);

		mediaData.resize(mediaData.size() + cbCurrentLength);
		memcpy(mediaData.data() + mediaData.size() - cbCurrentLength, pBuffer, cbCurrentLength);

		pMFMediaBuffer->Unlock();

		pMFMediaBuffer->Release();
		pMFSample->Release();
	}
	pMFSourceReader->Release();

	// 格納する音声データ
	SoundData soundData;
	soundData.wfex = *waveFormat;
	soundData.bufferSize = static_cast<UINT>(mediaData.size());
	soundData.buffer = std::move(mediaData);

	// mapに格納
	soundMap_[filename] = std::move(soundData);

	CoTaskMemFree(waveFormat);
}

void Audio::SoundUnload(const wchar_t* filename) {
	soundMap_.erase(filename); // マップから削除
}

void Audio::SoundPlay(const wchar_t* filename,bool isLoop) {
	if (soundMap_.find(filename) == soundMap_.end()) {
		return; // 存在しない場合は何もしない
	}
	HRESULT hr;

	// 波形フォーマットを元にSourceVoiceの生成
	IXAudio2SourceVoice* sourceVoice = nullptr;
	hr = xAudio2_->CreateSourceVoice(&sourceVoice, &soundMap_[filename].wfex);
	assert(SUCCEEDED(hr));

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundMap_[filename].buffer.data();
	buf.AudioBytes = static_cast<UINT>(soundMap_[filename].bufferSize);
	buf.Flags = XAUDIO2_END_OF_STREAM; // 波形データの終端を示すフラグ
	if (isLoop) {
		buf.LoopBegin = 0; // ループ
		buf.LoopLength = 0;
		buf.LoopCount = XAUDIO2_LOOP_INFINITE;
	}
	// 波形データの再生
	hr = sourceVoice->SubmitSourceBuffer(&buf);
	assert(SUCCEEDED(hr));
	hr = sourceVoice->Start();
	assert(SUCCEEDED(hr));

	voices_.push_back(sourceVoice);
}

void Audio::StopAll() {
	for (auto* voice : voices_) {
		if (voice) {
			voice->Stop();
			voice->FlushSourceBuffers();
			voice->DestroyVoice();
		}
	}
	voices_.clear();
}

