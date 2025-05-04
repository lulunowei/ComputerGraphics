#pragma once
#define GLFW_INCLUDE_VULKAN
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtx/hash.hpp>
#include<vector>
#include<array>
typedef struct Vertex {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 texCoord;

        // 顶点数据绑定
        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;//每个顶点数据都打包在一个数组中,绑定到第一个索引
            bindingDescription.stride = sizeof(Vertex);//顶点占用的字节数
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;//per-vertex data rendering

            return bindingDescription;
        }

        bool operator==(const Vertex& other) const
        {
            return pos == other.pos &&
                color == other.color &&
                texCoord == other.texCoord;
        }

 }Vertex;

//特化hash模板
namespace std {
    template<>
    struct hash<Vertex> {
        size_t operator()(Vertex const& vertex) const {
            //计算 pos、color 和 texCoord 的哈希值，然后将它们合并到一个单一的哈希值中
            return ((hash<glm::vec3>()(vertex.pos) ^
                (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
};










    //定义一个缓冲区，里面有三个字段：位置和颜色，纹理坐标
    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

        //位置属性（location = 0）
        attributeDescriptions[0].binding = 0;// 绑定索引0缓冲区
        attributeDescriptions[0].location = 0;//对应顶点着色器中的layout(location = 0) in vec2 inPosition;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT; //vec3（32位浮点数）
        attributeDescriptions[0].offset = offsetof(Vertex, pos);// 偏移量0字节

        // 颜色属性（location = 1）
        attributeDescriptions[1].binding = 0;    // 绑定索引0缓冲区
        attributeDescriptions[1].location = 1;    // 顶点着色器中的 location 1，对应顶点着色器中的layout(location = 1) in vec3 inColor
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT; // vec3（32位浮点数）
        attributeDescriptions[1].offset = offsetof(Vertex, color);   // 偏移量8字节（vec2占8字节）

        //纹理坐标(location=2)
        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        //其他属性。。。(location=n)

        return attributeDescriptions;
    }

    //矩形
    namespace rectangle {
        //矩形顶点
        const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f,0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f,1.0f}},
        {{ -0.5f, 0.5f, 0.0f }, {1.0f, 1.0f, 1.0f}, {1.0f,1.0f} },

        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
        };

        //矩形索引
        const std::vector<uint16_t> indices = { 0, 1, 2, 2, 3, 0,4, 5, 6, 6, 7, 4 };
    }





