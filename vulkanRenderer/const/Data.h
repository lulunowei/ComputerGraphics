#pragma once
#include<vulkan/vulkan.h>
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
const std::string TEXTURE_PATH = "textures/wrj.png";//纹理路径

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
