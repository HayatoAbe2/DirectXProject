#pragma once

struct EnemyStatus {
	int hp = 10; // hp
	float radius = 0.5f; 	// 半径
	float moveSpeed = 0.1f;	// 移動の速さ
	float defaultSearchRadius = 8.0f; // 検知範囲
	float loseSightRadius = 15.0f; // 見失い距離
	int loseSightTime = 120; 	// 見失いカウント
	int moveTime = 0; // 動く時間
	int stopTime = 0; // その後止まる時間
	int stunResist = 0; // スタン時間を減らす量
	bool canFly = false; // 飛行する
};