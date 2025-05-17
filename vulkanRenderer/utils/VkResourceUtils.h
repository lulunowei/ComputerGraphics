#pragma once
#include<vulkan/vulkan.h>
class VkResourceUtils
{
    static void createBuffer(
        VkPhysicalDevice physicalDevice,
        VkDevice m_logicalDevice,
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer& buffer,
        VkDeviceMemory& bufferMemory);//创建缓冲区

    static void createImage(
        VkPhysicalDevice physicalDevice,
        VkDevice logicalDevice,
        uint32_t width,
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
};

