#include "LoadModels.h"
#include <tiny_obj_loader.h>
#include"Data.h"
/**
 * @descrip 加载obj类型的模型
 * 
 * @functionName:  loadObjModels
 * @functionType:    void
 */
void LoadModels::loadObjModels(
	 std::vector<Vertex>& vertices,
	 std::vector<uint32_t>& indices)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str())) {
		throw std::runtime_error(warn + err);
	}

	std::unordered_map<Vertex, uint32_t> uniqueVertices;

	std::array<glm::vec3, 3> colors = {
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 1.0f)
	};

	size_t colorIndex = 0;
	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			// 检查顶点索引
			if (index.vertex_index < 0 ||
				3 * index.vertex_index + 2 >= attrib.vertices.size()) {
				throw std::runtime_error("Invalid vertex index: " +
					std::to_string(index.vertex_index));
			}

			Vertex vertex{};

			// 顶点坐标
			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			// 纹理坐标（带检查）
			vertex.texCoord = { 0.0f, 0.0f };
			if (index.texcoord_index >= 0 && !attrib.texcoords.empty()) {
				if (2 * index.texcoord_index + 1 >= attrib.texcoords.size()) {
					throw std::runtime_error("Invalid texcoord index: " +
						std::to_string(index.texcoord_index));
				}
				vertex.texCoord = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};
			}

			//vertex.color = { 1.0f, 1.0f, 1.0f };//每个顶点赋值纯白色
			vertex.color = colors[colorIndex % colors.size()];//每个顶点赋值
			colorIndex++;

			// 去重
			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);//存入顶点缓冲区
			}
			indices.push_back(uniqueVertices[vertex]);//存入索引缓冲区
		}
	}
	std::cout << "Loaded " << vertices.size() << " vertices, "
		<< indices.size() << " indices." << std::endl;
}

/**
 * @descrip 加载四边形
 * 
 * @functionName:  loadRectangle
 * @functionType:    void
 */
void LoadModels::loadRectangle(
	std::vector<Vertex>& vertices,
	std::vector<uint32_t>& indices
){
	for (auto vertex : rectangle::vertices) {
		vertices.push_back(vertex);
	}

	for (auto indice : rectangle::indices) {
		indices.push_back(indice);
	}
}

/**
 * @descrip 加载立方体
 * 
 * @functionName:  loadCubes
 * @functionType:    void
 * @param vertices
 * @param indices
 */
void LoadModels::loadCubes(
	std::vector<Vertex>& vertices,
	std::vector<uint32_t>& indices
)
{
	//加载顶点属性
	for (auto vertex : cube::cubeVertices) {
		vertices.push_back(vertex);
	}
	//for (auto indice : cube::cubeIndices) {
	//	indices.push_back(indice);
	//}
}
