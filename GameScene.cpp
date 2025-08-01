#include "GameScene.h"
#include "Graphics.h"
#include "Input.h"
#include "Model.h"
#include "Camera.h"
#include "DebugCamera.h"
#include "Math.h"
#include "Audio.h"
#include <numbers>

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"


GameScene::~GameScene() {
	delete planeModel_;
	delete camera_;
	delete debugCamera_;
	delete checkerSprite_;
	delete multiMeshModel_;
	delete teapotModel_;
	delete bunnyModel_;
	delete suzanneModel_;
}

void GameScene::Initialize(Graphics* graphics) {
	// plane
	planeModel_ = Model::LoadObjFile("Resources", "plane.obj", *graphics);
	transformPlane_.rotate = { 0,float(std::numbers::pi),0 };
	transformPlane_.translate = { 0,0,0 };

	// スプライト
	checkerSprite_ = Sprite::Initialize(graphics, "Resources/uvChecker.png", { 256,256 });

	// 球
	transformSphere_.translate = { 3,0,0 };

	// MultiMesh
	multiMeshModel_ = Model::LoadObjFile("Resources", "multiMesh.obj", *graphics);
	transformMultiMesh_.rotate = { 0,float(std::numbers::pi),0 };

	// teapot
	teapotModel_ = Model::LoadObjFile("Resources", "teapot.obj", *graphics);
	transformTeapot_.translate = { -2,0,0 };
	transformTeapot_.rotate = { 0,float(std::numbers::pi),0 };
	
	// bunny
	bunnyModel_ = Model::LoadObjFile("Resources", "bunny.obj", *graphics);
	transformBunny_.translate = { 2,0,0 };
	transformBunny_.rotate = { 0,float(std::numbers::pi),0 };

	// suzanne
	suzanneModel_ = Model::LoadObjFile("Resources", "suzanne.obj", *graphics);
	transformSuzanne_.translate = { 0,-1,0 };
	transformSuzanne_.rotate = { 0,float(std::numbers::pi),0 };

	// カメラ
	debugCamera_ = new DebugCamera;
	debugCamera_->Initialize();
	camera_ = new Camera;
	camera_->transform_.translate = { 0,0,-15 };
	camera_->UpdateCamera(*graphics, *debugCamera_);

	ImGui::SetNextWindowSize({ 100,300 });
	ImGui::SetNextWindowPos({ 10,10 });
}

void GameScene::Update(Input* input, Audio* audio) {

	debugCamera_->Update(input);

	if(input->IsTrigger(DIK_SPACE)){
		audio->SoundPlay(L"Resources/Alarm01.wav");
	}
}

void GameScene::Draw(Graphics* graphics) {
	ImGui::Begin("Settings");
	if (ImGui::BeginTabBar("tab")) {
		if (ImGui::BeginTabItem("Plane,Sprite,Sphere")) {

			// Obj
			if (ImGui::CollapsingHeader("Plane.obj")) {
				ImGui::PushID("plane");
				ImGui::DragFloat3("Scale", &transformPlane_.scale.x, 0.01f);
				ImGui::DragFloat3("Rotate", &transformPlane_.rotate.x, 0.01f);
				ImGui::DragFloat3("Translate", &transformPlane_.translate.x, 0.01f);
				ImGui::PopID();
			}
			planeModel_->SetTransform(transformPlane_);
			planeModel_->UpdateTransformation(*camera_);
			planeModel_->Draw(*graphics);

			// Sprite
			if (ImGui::CollapsingHeader("Sprite")) {
				checkerSprite_->ImGuiEdit();
			}
			checkerSprite_->UpdateTransform(camera_, float(graphics->GetWindowWidth()), float(graphics->GetWindowHeight()));
			checkerSprite_->Draw(*graphics, checkerSprite_->GetColor());

			// sphere
			if (ImGui::CollapsingHeader("Sphere")) {
				ImGui::PushID("sphere");
				ImGui::DragFloat3("Scale", &transformSphere_.scale.x, 0.01f);
				ImGui::DragFloat3("Rotate", &transformSphere_.rotate.x, 0.01f);
				ImGui::DragFloat3("Translate", &transformSphere_.translate.x, 0.01f);
				ImGui::PopID();
				graphics->ImGuiEditSphere();
			}
			graphics->DrawSphere(transformSphere_, *camera_);

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("MultiMesh")) {
			ImGui::PushID("multiMesh");
			ImGui::DragFloat3("Scale", &transformMultiMesh_.scale.x, 0.01f);
			ImGui::DragFloat3("Rotate", &transformMultiMesh_.rotate.x, 0.01f);
			ImGui::DragFloat3("Translate", &transformMultiMesh_.translate.x, 0.01f);
			ImGui::PopID();
			multiMeshModel_->SetTransform(transformMultiMesh_);
			multiMeshModel_->UpdateTransformation(*camera_);
			multiMeshModel_->Draw(*graphics);

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Teapot,Bunny,Suzanne")) {
			if (ImGui::CollapsingHeader("Teapot")) {
				ImGui::PushID("teapot");
				ImGui::DragFloat3("Scale", &transformTeapot_.scale.x, 0.01f);
				ImGui::DragFloat3("Rotate", &transformTeapot_.rotate.x, 0.01f);
				ImGui::DragFloat3("Translate", &transformTeapot_.translate.x, 0.01f);
				ImGui::PopID();
			}
			teapotModel_->SetTransform(transformTeapot_);
			teapotModel_->UpdateTransformation(*camera_);
			teapotModel_->Draw(*graphics);

			if (ImGui::CollapsingHeader("Bunny")) {
				ImGui::PushID("bunny");
				ImGui::DragFloat3("Scale", &transformBunny_.scale.x, 0.01f);
				ImGui::DragFloat3("Rotate", &transformBunny_.rotate.x, 0.01f);
				ImGui::DragFloat3("Translate", &transformBunny_.translate.x, 0.01f);
				ImGui::PopID();
			}
			bunnyModel_->SetTransform(transformBunny_);
			bunnyModel_->UpdateTransformation(*camera_);
			bunnyModel_->Draw(*graphics);

			if (ImGui::CollapsingHeader("Suzanne")) {
				ImGui::PushID("suzanne");
				ImGui::DragFloat3("Scale", &transformSuzanne_.scale.x, 0.01f);
				ImGui::DragFloat3("Rotate", &transformSuzanne_.rotate.x, 0.01f);
				ImGui::DragFloat3("Translate", &transformSuzanne_.translate.x, 0.01f);
				if (!ImGui::Checkbox("Rainbow", &isRainbow_)) {
					ImGui::ColorEdit3("Color", &color_.x);
				}
				ImGui::PopID();
				if (isRainbow_) {
					time_ += 1.0f / 60.0f;  // 秒数など
					float hue = fmod(time_ * 0.1f, 1.0f);  // H: 0.0〜1.0 をループ
					color_ = HSVtoRGB(hue);
				}
			}
			suzanneModel_->SetTransform(transformSuzanne_);
			suzanneModel_->UpdateTransformation(*camera_); 
			suzanneModel_->Draw(*graphics,color_);

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::Separator();
	ImGui::Spacing();
	if (ImGui::CollapsingHeader("Lighting")) {
		graphics->ImGuiEditLight();
	}
	ImGui::End();
}