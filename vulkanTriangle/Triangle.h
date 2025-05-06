#pragma once
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
//#define STB_IMAGE_IMPLEMENTATION
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>//用于生成MVP变换
#include <tiny_obj_loader.h>
#include"VertexData.h"
#include"const.h"
#include "FileUtils.h"
#include"Matrix.h"



constexpr std::uint32_t WIDTH = 800;
constexpr std::uint32_t HEIGHT = 600;
const int MAX_FRAMES_IN_FLIGHT = 2;//定义同时处理的帧数

const std::string MODEL_PATH = "models/viking_room.obj";//模型路径
const std::string TEXTURE_PATH = "textures/viking_room.png";//纹理路径


const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif // !NDEBUG



VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger);

void DestroyDebugUtilsMessengerEXT(VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator);

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



class Triangle
{
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();  //主循环
        cleanup();  //释放资源
    }

private:

    void initVulkan();

    void mainLoop();

    void cleanup();

    void initWindow();

    void createInstance();//创建vulkan实例

    void setupDebugMessenger();//创建调试消息的接收器

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);//封装调试消息的创建信息

    std::vector<const char*> getRequiredExtensions();//获取与 GLFW 配合使用时所需的 Vulkan 扩展

    bool checkValidationLayerSupport();//检查一个或多个 Vulkan 验证层是否在系统的 Vulkan 安装中可用

    void pickPhysicalDevice();//选择一个物理设备

    bool isDeviceSuitable(VkPhysicalDevice device);//检查物理设备是否满足我们的需求

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);//查找queue族

    void createLogicalDevice();//创建逻辑设备

    void createSurface();//创建表面

    bool checkDeviceExtensionSupport(VkPhysicalDevice device);//检查物理设备是否支持需要的扩展

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);//swapchain的属性查询

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);//选择合适的swapchain格式

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);//选择合适的swapchain显示模式

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);//选择合适的swapchain分辨率

    void createSwapChain();//创建swapchain

    VkImageView createImageView(VkImage image,
        uint32_t mipLevels,
        VkFormat format,
        VkImageAspectFlags aspectFlags);//抽象创建视图

    void createImageViews();//创建图像视图

    void createDescriptorSetLayout();//创建描述符布局

    void createGraphicsPipeline();//创建图形管道

    VkShaderModule createShaderModule(const std::vector<char>& code);//创建着色器模块

    void createRenderPass();//创建渲染流程

    void createFramebuffers();//创建framebuffers

    void createCommandPool();//创建命令池

    void createCommandBuffers();//创建命令缓冲区

    void generateMipmaps(VkImage image,
        VkFormat imageFormat,
        int32_t texWidth,
        int32_t texHeight,
        int32_t mipLevels);//生成mipmaps

    void transitionImageLayout(VkImage image,
        uint32_t mipLevels,
        VkFormat format,
        VkImageLayout oldLayout, 
        VkImageLayout newLayout);//处理图像布局转换

    void createTextureImage();//创建纹理图

    void createTextureImageView();//创建纹理视图

    void createTextureSampler();//创建纹理采样器

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);//记录命令缓冲区

    void updateUniformBuffer(uint32_t currentImage);//更新全局数据

    void loadModel();//加载obj模型

    void drawFrame();//绘制一帧

    void createSyncObjects();//创建同步对象

    void cleanupSwapChain();//清理swapchain

    void recreateSwapChain();//重新创建swapchain，用于窗口大小改变

    void createVertexBuffer();//创建顶点缓冲区

    void createIndexBuffer();//创建索引缓冲区

    void createUniformBuffers();//创建统一资源缓冲区

    void createDescriptorPool();//创建描述集池

    void createDescriptorSets();//创建描述符集

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    void createBuffer(VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer& buffer,
        VkDeviceMemory& bufferMemory);//创建缓冲区

    void createImage(uint32_t width, uint32_t height, uint32_t mipLevels,
        VkFormat format,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkImage& image,
        VkDeviceMemory& imageMemory);//创建图像缓冲区

    VkCommandBuffer beginSingleTimeCommands();//单次开始创建命令

    void endSingleTimeCommands(VkCommandBuffer commandBuffer);//单次结束录制命令

    //从CPU内存拷贝到GPU显存
    void copyBuffer(VkBuffer srcBuffer,
        VkBuffer dstBuffer,
        VkDeviceSize size);

    //从CPU内存拷贝到GPU显存(图像)
    void copyBufferToImage(VkBuffer buffer,
        VkImage image,
        uint32_t width, uint32_t height);

    //查找支持的格式
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates,
        VkImageTiling tiling,
        VkFormatFeatureFlags features);

    VkFormat findDepthFormat();//选择深度组件格式

    bool hasStencilComponent(VkFormat format);//检查深度格式包含模板组件

    void createDepthResources();//创建深度资源





    //静态的回调函数
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;//返回true调用程序会终止
    }

    // 窗口大小改变的回调函数
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<Triangle*>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
    }

private:
    GLFWwindow* window;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;//句柄，引用调试消息的接收器
    VkSurfaceKHR surface;//Vulkan 窗口表面


    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;//物理设备句柄，隐式销毁
    VkDevice device;//逻辑设备句柄

    VkQueue graphicsQueue;//图形队列

    VkQueue presentQueue;//显示队列

    VkSwapchainKHR swapChain;//swapchain句柄

    std::vector<VkImage> swapChainImages;//swapchain图像句柄，资源图，不允许采样或者渲染

    VkFormat swapChainImageFormat;//swapchain图像格式

    VkExtent2D swapChainExtent;//swapchain图像分辨率

    std::vector<VkImageView> swapChainImageViews;//swapchain图像视图

    VkRenderPass renderPass;//渲染流程

    VkDescriptorSetLayout descriptorSetLayout;//描述符绑定，指定管线访问的资源
    VkPipelineLayout pipelineLayout;//图形管道布局

    VkPipeline graphicsPipeline;//图形管道,用于被继承

    std::vector<VkFramebuffer> swapChainFramebuffers;//framebuffers

    VkCommandPool commandPool;//命令池

    std::vector<VkCommandBuffer> commandBuffers;;//命令

    //显式等待信号量
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    //显式等待信号量
    std::vector<VkFence> inFlightFences;

    uint32_t currentFrame = 0;

    bool framebufferResized = false;//窗口大小是否改变

    std::vector<Vertex> vertices;//自定义顶点属性
    std::vector<uint32_t> indices;//自定义索引

    VkBuffer vertexBuffer;//顶点缓冲区
    VkDeviceMemory vertexBufferMemory;//顶点缓冲区内存

    VkBuffer indexBuffer;//索引缓冲区
    VkDeviceMemory indexBufferMemory;//索引缓冲区内存

    std::vector<VkBuffer> uniformBuffers;//统一资源缓冲区
    std::vector<VkDeviceMemory> uniformBuffersMemory;//统一资源内存
    std::vector<void*> uniformBuffersMapped;//统一资源映射

    VkDescriptorPool descriptorPool;//描述符集池
    std::vector<VkDescriptorSet> descriptorSets;//描述符集合

    VkBuffer stagingBuffer;//暂存缓存
    VkDeviceMemory stagingBufferMemory;//暂存缓存内存

    uint32_t mipLevels;//mipmap层数
    VkImage textureImage;//图像缓存
    VkDeviceMemory textureImageMemory;//图像缓存内存
    VkImageView textureImageView;//图像视图
    VkSampler textureSampler;//采样器

    VkImage depthImage;//深度图像
    VkDeviceMemory depthImageMemory;//深度图像内存
    VkImageView depthImageView;//深度视图

};


