#pragma once
#include <string>
#include <map>
class Model;
class ModelLoader;

class ModelManager {
public:
	/// <summary>
	/// モデルの読み込み(すでに登録済みならそれを返す)
	/// </summary>
	/// <param name="key">登録するキー(取得時に使用)</param>
	/// <param name="directoryPath">objファイルのパス(/Resource/xx/xx.obj)</param>
	/// <returns>モデルのポインタ</returns>
	Model* Load(const std::string& key, const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// モデルの取得
	/// </summary>
	/// <param name="key">登録時のキー</param>
	/// <returns>モデルポインタ</returns>
	Model* Get(const std::string& key);

	/// <summary>
	/// モデルの破棄
	/// </summary>
	/// <param name="key"></param>
	/// <returns></returns>
	Model* Unload(const std::string& key);

private:
	// モデル読み込みクラスのポインタ
	ModelLoader* modelLoader_;

	// 読み込んだモデルのキャッシュ
	std::map<std::string, Model*> modelCache_; // キー、モデル

};

