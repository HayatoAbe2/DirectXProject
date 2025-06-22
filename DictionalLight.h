#pragma once

struct DirectionalLight {
	Vector4 color;
	Vector3 direction; // 向き(単位ベクトル)
	float intensity; // 輝度
};