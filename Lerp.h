#pragma once

// 線形補間関数  
float Lerp(float a, float b, float t) {
	return a + t * (b - a);
}