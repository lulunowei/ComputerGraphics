#pragma once
#define GLFW_INCLUDE_VULKAN
#define GLM_ENABLE_EXPERIMENTAL
#include<vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtx/hash.hpp>
#include<vector>
#include<array>
#include"../const.h"

inline const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

inline const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

constexpr std::uint32_t WIDTH = 800;
constexpr std::uint32_t HEIGHT = 600;
const int MAX_FRAMES_IN_FLIGHT = 2;//定义同时处理的帧数

const std::string MODEL_PATH = "models/asterion.obj";//模型路径
const std::string TEXTURE_PATH = "textures/砖块.png";//纹理路径

/**
 * 存储 Vulkan 设备队列族
 */
struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

/**
 * 存储 Vulkan 设备支持的交换链相关信息.
 */
struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};


struct UniformBufferObject {
   alignas(16) glm::mat4 model[10];
   alignas(16) glm::mat4 view;
   alignas(16) glm::mat4 proj;
   alignas(16) float time;
};

struct UniformBufferObjectSingle {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    alignas(16) float time;
};

typedef struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    // 顶点数据缓冲绑定
    static  std::array<VkVertexInputBindingDescription, 2> getBindingDescription() {
        std::array<VkVertexInputBindingDescription, 2> bindingDescriptions{};

        bindingDescriptions[0].binding = 0;//每个顶点数据都打包在一个数组中,绑定到第一个索引
        bindingDescriptions[0].stride = sizeof(Vertex);//顶点占用的字节数
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;//per-vertex data rendering

        bindingDescriptions[1].binding = 1;//实例数据
        bindingDescriptions[1].stride = sizeof(uint32_t); // 每个实例一个 uint
        bindingDescriptions[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

        return bindingDescriptions;
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
static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

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

    //实例ID属性(location=3)
    attributeDescriptions[3].binding = 1;            // 来自绑定1（专门用于实例数据）
    attributeDescriptions[3].location = 3;           // 着色器中的 layout(location = 3)
    attributeDescriptions[3].format = VK_FORMAT_R32_UINT; // 对应 uint 类型
    attributeDescriptions[3].offset = 0;             // 从缓冲区开始位置读取   
    
    //其他属性。。。(location=n)

    return attributeDescriptions;
}

//矩形
namespace rectangle {
    //矩形顶点
    extern const std::vector<Vertex> vertices;

    //矩形索引
    extern const std::vector<uint16_t> indices;
}

//cube
namespace cube {

    //立方体顶点
    extern const std::vector<Vertex> cubeVertices;
    //矩形索引
    extern const std::vector<uint16_t> cubeIndices;
    //平移向量
    extern const glm::vec3 cubePositions[];
}

