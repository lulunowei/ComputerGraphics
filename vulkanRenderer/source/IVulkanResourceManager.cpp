#include "IVulkanResourceManager.h"
#include"../utils/VkExtensionUtils.h"
#include"../utils/VkFormatUtils.h"
#include"../init/AllHeads.h"



/**
 * @descrip 创建缓冲区
 * 
 * @functionName:  createBuffer
 * @functionType:    void
 * @param size
 * @param usage
 * @param properties
 * @param buffer
 * @param bufferMemory
 */
void IVulkanResourceManager::createBuffer(
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
	if (vkCreateBuffer(m_context.deviceContext->getLogicalDevice(), 
		&bufferInfo, 
		nullptr,
		&buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}
	//2.查询传入参数buffer的内存需求
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(
		m_context.deviceContext->getLogicalDevice(), 
		buffer, 
		&memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	//查询物理设备能支持的内存类型
	allocInfo.memoryTypeIndex = VkExtensionUtils::findMemoryType(
		m_context.deviceContext->getPhysicalDevice(),
		memRequirements.memoryTypeBits, properties);

	//3.分配内存
	if (vkAllocateMemory(m_context.deviceContext->getLogicalDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	//4.绑定缓冲区和内存
	vkBindBufferMemory(m_context.deviceContext->getLogicalDevice(), buffer, bufferMemory, 0);

}

/**
 * @descrip  创建图像缓冲区
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
void IVulkanResourceManager::createImage(
	uint32_t width,
	uint32_t height,
	uint32_t mipLevels,
	VkSampleCountFlagBits numSamples,
	VkFormat format,
	VkImageTiling tiling,
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

	if (vkCreateImage(m_context.deviceContext->getLogicalDevice(), &imageInfo, nullptr, &image) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(m_context.deviceContext->getLogicalDevice(), image, &memRequirements);//获取图像对象的内存需求

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = VkExtensionUtils::findMemoryType(
		m_context.deviceContext->getPhysicalDevice(),
		memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(m_context.deviceContext->getLogicalDevice(), &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(m_context.deviceContext->getLogicalDevice(), image, imageMemory, 0);

}

/**
 * @descrip 从CPU内存拷贝到GPU显存
 * 
 * @functionName:  copyBuffer
 * @functionType:    void
 * @param srcBuffer
 * @param dstBuffer
 * @param size
 */
void IVulkanResourceManager::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBuffer commandBuffer=m_cmdManager.beginSingleTimeCommands();
		VkBufferCopy copyRegion{};
		//copyRegion.srcOffset = 0; // Optional
		//copyRegion.dstOffset = 0; // Optional
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
	m_cmdManager.endSingleTimeCommands(commandBuffer);
}

/**
 * @descrip 从CPU内存拷贝到GPU显存(图像)
 * 
 * @functionName:  copyBufferToImage
 * @functionType:    void
 * @param buffer
 * @param image
 * @param width
 * @param height
 */
void IVulkanResourceManager::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
	VkCommandBuffer commandBuffer = m_cmdManager.beginSingleTimeCommands();
		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageExtent = {
			width,
			height,
			1
		};
		region.imageOffset = { 0,0,0 };
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		vkCmdCopyBufferToImage(
			commandBuffer,
			buffer,
			image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,//表示图像当前正在使用哪种布局
			1,
			&region
		);//缓冲区到图像的复制
	m_cmdManager.endSingleTimeCommands(commandBuffer);
}

/**
 * @descrip 处理图像布局转换，定义了图像内存屏障
 * 
 * @functionName:  transitionImageLayout
 * @functionType:    void
 * @param image
 * @param mipLevels
 * @param format
 * @param oldLayout
 * @param newLayout
 */
void IVulkanResourceManager::transitionImageLayout(VkImage image, uint32_t mipLevels, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkCommandBuffer commandBuffer = m_cmdManager.beginSingleTimeCommands();
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;

		if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

			if (VkFormatUtils::hasStencilComponent(format)) {//如果还有模板测试
				barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}
		else {
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}
		//barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = mipLevels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		barrier.srcAccessMask = 0; // 指定 在这个 barrier 之前 需要完成的访问类型
		barrier.dstAccessMask = 0; // 指定 在这个 barrier 之后 将会进行的访问类型


		VkPipelineStageFlags sourceStage;//它控制前一个操作在哪个阶段结束之后，barrier 才生效
		VkPipelineStageFlags destinationStage;//它控制新的操作在 barrier 之后、在哪个阶段才能开始执行。

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;//不需要等待之前的操作完成，设置为0
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;//目标掩码

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;//源阶段中，图像没有参与其他操作
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;//目标阶段是传输
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;//等待传输完成
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;//完成后进行shader读取

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;//源阶段是传输
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;//目标阶段是 片段着色器中读取纹理
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;//管线最初始阶段
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;//目标阶段，读取深度缓存
		}
		else {
			throw std::invalid_argument("unsupported layout transition!");
		}
		//提交屏障
		vkCmdPipelineBarrier(
			commandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);
	m_cmdManager.endSingleTimeCommands(commandBuffer);
}

/**
 * @descrip 抽象创建图像视图
 * 
 * @functionName:  createImageView
 * @functionType:    VkImageView
 * @param image
 * @param mipLevels
 * @param format
 * @param aspectFlags
 * @return    
 */
VkImageView IVulkanResourceManager::createImageView(VkImage image, uint32_t mipLevels, VkFormat format, VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.format = format;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	//指定纹理视图看到的是图像的哪一部分
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = mipLevels;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView(m_context.deviceContext->getLogicalDevice(),
		&viewInfo,
		nullptr,
		&imageView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture image view!");
	}

	return imageView;
}
