#pragma once
#define GLFW_INCLUDE_VULKAN
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include<vector>
#include<array>
namespace myVertexData {

    typedef struct Vertex {
        glm::vec2 pos;
        glm::vec3 color;

        // 顶点数据绑定
        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;//每个顶点数据都打包在一个数组中,绑定到第一个索引
            bindingDescription.stride = sizeof(Vertex);//顶点占用的字节数
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;//per-vertex data rendering

            return bindingDescription;
        }
    }Vertex;

    // vertex attributes:position and color
       //描述怎么获取到vertex数据
    //定义一个缓冲区，里面有两个字段：位置和颜色
    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        //位置属性（location = 0）
        attributeDescriptions[0].binding = 0;// 绑定索引0缓冲区
        attributeDescriptions[0].location = 0;//对应顶点着色器中的layout(location = 0) in vec2 inPosition;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT; //vec2（32位浮点数）
        attributeDescriptions[0].offset = offsetof(Vertex, pos);// 偏移量0字节

        // 颜色属性（location = 1）
        attributeDescriptions[1].binding = 0;    // 绑定索引0缓冲区
        attributeDescriptions[1].location = 1;    // 顶点着色器中的 location 1，对应顶点着色器中的layout(location = 1) in vec3 inColor
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT; // vec3（32位浮点数）
        attributeDescriptions[1].offset = offsetof(Vertex, color);   // 偏移量8字节（vec2占8字节）

        //其他属性。。。(location=n)

        return attributeDescriptions;
    }

    //三角形
    const std::vector<Vertex> vertices = {
        { { 0.0f, -0.5f },  { 1.0f, 1.0f, 1.0f } }, // 第一个顶点：位置和颜色
        { { 0.5f, 0.5f },   { 0.0f, 1.0f, 0.0f } }, // 第二个顶点
        { { -0.5f, 0.5f },  { 0.0f, 0.0f, 1.0f } } // 第三个顶点};
    };

    //矩形
    namespace rectangle {
        //矩形顶点
        const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
        };

        //矩形索引
        const std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};
    }


};

