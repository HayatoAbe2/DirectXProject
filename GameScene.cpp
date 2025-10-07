#include "GameScene.h"
#include "Graphics.h"
#include "Input.h"
#include "Model.h"
#include "Camera.h"
#include "DebugCamera.h"
#include "MathUtils.h"
#include "Audio.h"
#include <numbers>

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"


GameScene::~GameScene() {
}

void GameScene::Initialize(Graphics* graphics) {
	
}

void GameScene::Update(Input* input, Audio* audio) {

	

	debugCamera_->Update(input);
}

void GameScene::Draw(Graphics* graphics) {
	ImGui::Begin("Settings");
	ImGui::End();
}