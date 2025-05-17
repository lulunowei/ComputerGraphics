#pragma once
#include"../init/ApplicationContext.h"
#include"../render/VulkanCommandManager.h"
class IVulkanResourceManager
{
public:
    IVulkanResourceManager(ApplicationContext& context, VulkanCommandManager& cmdManager)
        : m_context(context), m_cmdManager(cmdManager) {}

    ~IVulkanResourceManager() = default;
protected:
	ApplicationContext& m_context;
    VulkanCommandManager& m_cmdManager;
protected:
    virtual void createResources() = 0;//创建资源
    virtual void cleanupResources() = 0;//销毁资源

    void createBuffer(VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer& buffer,
        VkDeviceMemory& bufferMemory);//创建缓冲区

    void createImage(uint32_t width,
        uint32_t height,
        uint32_t mipLevels,
        VkSampleCountFlagBits numSamples,//像素采样数
        VkFormat format,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkImage& image,
        VkDeviceMemory& imageMemory);//创建图像缓冲区

    //从CPU内存拷贝到GPU显存
    void copyBuffer(VkBuffer srcBuffer,
        VkBuffer dstBuffer,
        VkDeviceSize size);

    //从CPU内存拷贝到GPU显存(图像)
    void copyBufferToImage(VkBuffer buffer,
        VkImage image,
        uint32_t width, uint32_t height);

    void transitionImageLayout(VkImage image,
        uint32_t mipLevels,
        VkFormat format,
        VkImageLayout oldLayout,
        VkImageLayout newLayout);//处理图像布局转换

    VkImageView createImageView(VkImage image,
        uint32_t mipLevels,
        VkFormat format,
        VkImageAspectFlags aspectFlags);//抽象创建视图
};

