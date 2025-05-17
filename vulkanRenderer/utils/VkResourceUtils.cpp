#include "VkResourceUtils.h"
#include"VkExtensionUtils.h"
#include <stdexcept>

/**
 * @descrip 创建缓冲区用于传递信息
 * 
 * @functionName:  createBuffer
 * @functionType:    void
 * @param size 缓冲区大小
 * @param usage 缓冲区用途
 * @param properties 内存属性
 * @param buffer 缓冲区写入句柄
 * @param bufferMemory 内存句柄
 */
void VkResourceUtils::createBuffer(
	VkPhysicalDevice physicalDevice,
	VkDevice logicalDevice,
	VkDeviceSize size,
	VkBufferUsageFlags usage,
	VkMemoryPropertyFlags properties,
	VkBuffer& buffer,
	VkDeviceMemory& bufferMemory)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;//独占

	//1.创建缓冲区
	if (vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}
	//2.查询传入参数buffer的内存需求
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(logicalDevice, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	//查询物理设备能支持的内存类型
	allocInfo.memoryTypeIndex = VkExtensionUtils::findMemoryType(
		physicalDevice,
		memRequirements.memoryTypeBits, 
		properties);

	//3.分配内存
	if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	//4.绑定缓冲区和内存
	vkBindBufferMemory(logicalDevice, buffer, bufferMemory, 0);
}

/**
 * @descrip 创建图像缓冲区
 * 
 * @functionName:  createImage
 * @functionType:    void
 * @param width
 * @param height
 * @param mipLevels
 * @param numSamples
 * @param format
 * @param tiling
 * @param usage
 * @param properties
 * @param image
 * @param imageMemory
 */
void VkResourceUtils::createImage(
	VkPhysicalDevice physicalDevice,
	VkDevice logicalDevice,
	uint32_t width,
	uint32_t height, 
	uint32_t mipLevels,
	VkSampleCountFlagBits numSamples, 
	VkFormat format,
	VkImageTiling tiling, //读取方式
	VkImageUsageFlags usage, 
	VkMemoryPropertyFlags properties,
	VkImage& image,
	VkDeviceMemory& imageMemory)
{
	//创建图像对象，用于存储纹素
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = mipLevels;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;//设置与读取纹素相同的格式
	imageInfo.tiling = tiling;//纹素以实现定义的顺序排列，GPU高效访问
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;//图像在上传时，图像的内容没有意义
	imageInfo.usage = usage;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = numSamples;//样本采样数量
	//imageInfo.flags = 0; // Optional

	if (vkCreateImage(logicalDevice, &imageInfo, nullptr, &image) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(logicalDevice, image, &memRequirements);//获取图像对象的内存需求

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = VkExtensionUtils::findMemoryType(
		physicalDevice,
		memRequirements.memoryTypeBits, 
		properties);

	if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(logicalDevice, image, imageMemory, 0);
}
