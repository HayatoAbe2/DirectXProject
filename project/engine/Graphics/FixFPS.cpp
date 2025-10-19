#include "FixFPS.h"
#include <thread>

void FixFPS::Initialize() {
	// 時間の記録
	reference_ = std::chrono::steady_clock::now();
}

void FixFPS::Update() {
	// 1/60秒
	const std::chrono::microseconds kMinTime(uint64_t(1000000) / 60);
	// 1/60秒より短い時間
	const std::chrono::microseconds kMinCheckTime(uint64_t(1000000) / 65);

	// 現在時間
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	// 経過時間
	std::chrono::microseconds elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	if (elapsed < kMinCheckTime) {
		while (std::chrono::steady_clock::now() - reference_ < kMinTime) {
			// 1/60秒になるまで待機
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}

	// 時間の記録
	reference_ = std::chrono::steady_clock::now();
}
