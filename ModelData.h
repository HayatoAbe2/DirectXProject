#pragma once
#include "Material.h"
#include <vector>
#include "VertexData.h"

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};