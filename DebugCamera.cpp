//#include "DebugCamera.h"
//
//void DebugCamera::Initialize(){
//	
//}
//
//void DebugCamera::Update(const BYTE* key,const DIMOUSESTATE* mouse) {
//	if (key[DIK_RSHIFT]) {
//		// デバッグカメラの切り替え
//		isEnable_ = !isEnable_;
//	}
//
//	if (isEnable_) {
//		ControlCamera(key);
//		UpdateView();
//	}
//}
//
//void DebugCamera::ControlCamera(const BYTE* key){
//	if (key[DIK_UP]) {
//		Vector3 move = { 0,0,kMoveSpeed_ };
//		move = TransformVector(move, rotateMatrix_);
//		translation_ = Add(translation_, move);
//	}
//
//	if (key[DIK_LEFT]) {
//		Vector3 move = { kMoveSpeed_,0,0 };
//		move = TransformVector(move, rotateMatrix_);
//		translation_ = Add(translation_, move);
//	}
//}
//
//void DebugCamera::UpdateView(){
//	// 角度から回転行列を計算
//	//rotateMatrix_ = Multiply(Multiply(
//	//	MakeRotateXMatrix(rotation_.x),
//	//	MakeRotateYMatrix(rotation_.y)),
//	//	MakeRotateZMatrix(rotation_.z));
//
//	//// 座標から平行移動行列を計算する
//	//Matrix4x4 translateMatrix = MakeTranslateMatrix(translation_);
//
//	//// ワールド行列を計算
//	//Matrix4x4 worldMatrix = Multiply(rotateMatrix_, translateMatrix);
//
//	Matrix4x4 worldMatrix = MakeAffineMatrix({ {1,1,1},rotation_,translation_ });
//
//	// ワールド行列の逆行列をビュー行列に代入
//	viewMatrix_ = Inverse(worldMatrix);
//}
//
//
//
//
//
