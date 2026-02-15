#pragma once
#include <vector>
#include "SubMesh.h"

class Mesh {
public:
	// submeshを取得
	const std::vector<SubMesh>& GetPrimitives() const { return primitives_; }
	std::vector<SubMesh>& GetPrimitives() { return primitives_; }
private:

	// サブメッシュ
	std::vector<SubMesh> primitives_{};
};

