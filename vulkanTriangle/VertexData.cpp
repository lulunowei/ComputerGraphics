#include "VertexData.h"
using namespace myVertexData;


//const std::vector<Vertex> myVertexData::circle::createCircleVertices()
//{
//    std::vector<Vertex> vertices;//顶点
//    //设置圆心
//    vertices.push_back({ glm::vec3(0.0f,0.0f,0.0f),
//        glm::vec3(1.0f, 1.0f, 1.0f),
//        glm::vec2(0.5f, 0.5f) });
//
//    for (int i = 0; i < NUM_SEGMENTS+1; i++) {
//        float angle = 2.0f * glm::pi<float>() * i / NUM_SEGMENTS;
//        float x = RADIUS * cos(angle);
//        float y = RADIUS * sin(angle);
//        float z = RADIUS * sin(angle);
//
//        glm::vec3 color = glm::vec3((cos(angle) + 1) / 2, (sin(angle) + 1) / 2, 0.5f); // 彩色渐变
//        glm::vec2 texCoord = glm::vec2((x + 1.0f) / 2.0f, (y + 1.0f) / 2.0f); // 简单纹理映射
//
//        vertices.push_back({ glm::vec3(x, y, z), color, texCoord });//todo
//    }
//    return vertices;
//}
//
//const std::vector<uint16_t> myVertexData::circle::createCircleIndices()
//{
//    std::vector<uint16_t> indices;
//    for (uint16_t i = 1; i <= NUM_SEGMENTS; ++i) {
//        indices.push_back(0);       // 圆心
//        indices.push_back(i);       // 当前边界点
//        indices.push_back(i + 1);   // 下一个边界点
//    }
//    return indices;
//}
//
//// 定义全局静态常量
//const std::vector<myVertexData::Vertex> myVertexData::circle::vertices = myVertexData::circle::createCircleVertices();
//const std::vector<uint16_t> myVertexData::circle::indices = myVertexData::circle::createCircleIndices();
