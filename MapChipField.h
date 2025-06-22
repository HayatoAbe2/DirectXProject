#pragma once
#include <vector>
#include "Math.h"
#include <string>

enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
};

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

/// <summary>
/// マップチップフィールド
/// </summary>
class MapChipField {
public:
	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};

	struct Rect {
		float left;
		float right;
		float bottom;
		float top;
	};

	/// <summary>
	/// マップチップデータをリセット
	/// </summary>
	void ResetMapChipData();

	/// <summary>
	/// CSVファイルを読み込む
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	void LoadMapChipCsv(const std::string& filePath);

	/// <summary>
	/// マップチップ種別の取得
	/// </summary>
	/// <param name="xIndex">インデックスX</param>
	/// <param name="yIndex">インデックスY</param>
	/// <returns>マップチップ種別(範囲外なら空白チップ)</returns>
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	/// <summary>
	/// マップチップ座標取得
	/// </summary>
	/// <param name="xIndex">インデックスX</param>
	/// <param name="yIndex">インデックスY</param>
	/// <returns>マップチップのワールド座標(上=Y+)</returns>
	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

	 /// <summary>
	/// ブロックの水平方向の個数を取得
	/// </summary>
	/// <returns>ブロックの水平方向の個数</returns>
	uint32_t GetNumBlockHorizontal() const { return kNumBlockHorizontal; }

	/// <summary>
	/// ブロックの垂直方向の個数を取得
	/// </summary>
	/// <returns>ブロックの垂直方向の個数</returns>
	uint32_t GetNumBlockVirtical() const { return kNumBlockVirtical; }

	/// <summary>
	/// 座標からマップチップのインデックスを取得
	/// </summary>
	/// <param name="position">座標</param>
	/// <returns>マップチップ番号</returns>
	IndexSet GetMapChipIndexSetByPosition(const Vector3& position);

	/// <summary>
	/// インデックスからブロックの範囲を取得
	/// </summary>
	/// <param name="xIndex">x方向のマップチップ番号</param>
	/// <param name="yIndex">y方向のマップチップ番号</param>
	/// <returns>その位置にあるブロックの左右上下端</returns>
	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

private:
	// 1ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;

	// ブロックの個数
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

	// マップチップデータ
	MapChipData mapChipData_;
};