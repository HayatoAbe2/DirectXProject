#pragma once
#include "Transform.h"

#include <wrl.h>
#include <d3d12.h>
#include <memory>

class Sprite;
class Model;

class Entity {
public:
	Entity(std::shared_ptr<Sprite> sprite);
	Entity(std::shared_ptr<Model> model);

	///
	/// トランスフォーム操作
	/// 
	
	void SetTransform(const Transform& transform) { transform_ = transform; }
	void SetScale(const Vector3& scale) { transform_.translate = scale; }
	void SetRotate(const Vector3& rotate) { transform_.translate = rotate; }
	void SetTranslate(const Vector3& translate) { transform_.translate = translate; }

	///
	/// 描画物設定
	///

	// モデル
	void SetModel(const std::shared_ptr<Model>& model) { model_ = model; }
	std::shared_ptr<Model> GetModel() const { return model_; }

	// スプライト
	void SetSprite(const std::shared_ptr<Sprite>& sprite) { sprite_ = sprite; }
	std::shared_ptr<Sprite> GetSprite() const { return sprite_; }

	// インスタンスモデル
	//void SetModelInstance(const std::shared_ptr<ModelInstance>& m) { modelInstance_ = m; }
	//std::shared_ptr<ModelInstance> GetModelInstance() const { return modelInstance_; }

	// インスタンススプライト
	//void SetSpriteInstance(const std::shared_ptr<SpriteInstance>& s) { spriteInstance_ = s; }
	//std::shared_ptr<SpriteInstance> GetSpriteInstance() const { return spriteInstance_; }

	uint32_t GetID() const { return id_; }
	Transform GetTransform()const { return transform_; }

	/// <summary>
	/// 描画するもの(モデル/スプライト)があるかどうか
	/// </summary>
	/// <returns></returns>
	bool IsRenderable();

private:
	// エンティティID
	uint32_t id_;
	static uint32_t nextId_;

	// トランスフォーム
	Transform transform_ = { { 1,1,1 },{}, {} };

	// 描画するもの
	std::shared_ptr<Model> model_;   // 通常モデル
	std::shared_ptr<Sprite> sprite_; // 通常スプライト

	// インスタンス描画用
	//std::shared_ptr<ModelInstance> modelInstance_;   // インスタンスモデル
	//std::shared_ptr<SpriteInstance> spriteInstance_; // インスタンススプライト
};

