#pragma once
#include "Material.h"
#include "VertexData.h"
#include "Node.h"
#include "SubMesh.h"
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <memory>

class Mesh {
public:
	// submeshを取得
	const std::vector<SubMesh>& GetPrimitives() const { return primitives_; }
	std::vector<SubMesh>& GetPrimitives() { return primitives_; }
private:

	// サブメッシュ
	std::vector<SubMesh> primitives_{};
};

