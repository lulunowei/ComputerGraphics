#define STB_IMAGE_IMPLEMENTATION//stb头文件包含实现函数实现，只有#include会发生链接错误
//#define TINYOBJLOADER_IMPLEMENTATION
#include "TextureResource.h"
//#include <tiny_obj_loader.h>
#include <stb_image.h>

TextureResource::TextureResource(ApplicationContext& context, VulkanCommandManager& cmdManager):
	IVulkanResourceManager(context, cmdManager)
{
}

/**
 * @descrip 创建纹理图
 * 
 * @functionName:  createTextureImage
 * @functionType:    void
 */
void TextureResource::createTextureImage()
{
	int texWidth, texHeight, texChannels;
	//返回像素值数组的第一个元素
	stbi_uc* pixels = stbi_load(TEXTURE_PATH.c_str(),
		&texWidth,
		&texHeight,
		&texChannels,
		STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * texHeight * 4;
	if (!pixels) {
		throw std::runtime_error("failed to load texture image!");
	}
	//获取纹理图的mipmap等级
	m_mipLevels = static_cast<uint32_t>(
		std::floor(std::log2(std::max(texWidth, texHeight))))
		+ 1;

	createBuffer(imageSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,//分配GPU可读，CPU可写的内存
		m_stagingBuffer,
		m_stagingBufferMemory
	);//创建了一块vulkan管理的内存区，绑定到缓冲区对象stagingBuffer
	void* data;
	vkMapMemory(m_context.deviceContext->getLogicalDevice(), m_stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));//将内存读取的纹理数据拷贝到GPU可见CPU可写的内存
	vkUnmapMemory(m_context.deviceContext->getLogicalDevice(), m_stagingBufferMemory);
	stbi_image_free(pixels);

	createImage(static_cast<uint32_t>(texWidth),
		static_cast<uint32_t>(texHeight),
		m_mipLevels,
		VK_SAMPLE_COUNT_1_BIT,
		VK_FORMAT_R8G8B8A8_SRGB,//指定图像格式，与输入的纹理图一致
		VK_IMAGE_TILING_OPTIMAL,//指定纹素排列顺序
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,//指定用途
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,//分配到GPU显存
		m_textureImage,
		m_textureImageMemory
	);
	//转换图像布局
	transitionImageLayout(m_textureImage,
		m_mipLevels,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	//CPU缓存到GPU图像对象
	copyBufferToImage(m_stagingBuffer,
		m_textureImage,
		static_cast<uint32_t>(texWidth),
		static_cast<uint32_t>(texHeight));

	generateMipmaps(m_textureImage,
		VK_FORMAT_R8G8B8A8_SRGB,
		texWidth,
		texHeight,
		m_mipLevels);

	vkDestroyBuffer(m_context.deviceContext->getLogicalDevice(), m_stagingBuffer, nullptr);
	vkFreeMemory(m_context.deviceContext->getLogicalDevice(), m_stagingBufferMemory, nullptr);
}

/**
 * @descrip 抽象创建图像视图
 * 
 * @functionName:  createTextureImageView
 * @functionType:    void
 */
void TextureResource::createTextureImageView()
{
	m_textureImageView = createImageView(m_textureImage,
		m_mipLevels,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_ASPECT_COLOR_BIT);//创建纹理视图
}

/**
 * @descrip 创建纹理采样器，结果保存在成员对象的textureSampler中
 * 
 * @functionName:  createTextureSampler
 * @functionType:    void
 */
void TextureResource::createTextureSampler()
{
	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(m_context.deviceContext->getPhysicalDevice(), &properties); //检索物理设备的属性
	//std::cout << "GPU supports maxSamplerAnisotropy: " << properties.limits.maxSamplerAnisotropy << std::endl;


	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;//放大采用线性过滤
	samplerInfo.minFilter = VK_FILTER_LINEAR;//缩小采用线性过滤
	//纹理坐标超出了[0, 1] 范围，就重复纹理（平铺）
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;//设备最高性能
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;//超出边界填充的颜色，当且仅当超出纹理模式设为才生效
	samplerInfo.unnormalizedCoordinates = VK_FALSE;//设置[0,1]定义纹理坐标，即归一化
	samplerInfo.compareEnable = VK_FALSE;//禁用比较采样
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	//mipmapping
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.minLod = 0.0f; // Optional
	//samplerInfo.minLod = static_cast<float>(mipLevels / 2);; // Optional
	samplerInfo.maxLod = static_cast<float>(m_mipLevels);
	samplerInfo.mipLodBias = 0.0f; // Optional

	//创建采样器
	if (vkCreateSampler(m_context.deviceContext->getLogicalDevice(), 
		&samplerInfo,
		nullptr,
		&m_textureSampler) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture sampler!");
	}

}

/**
 * @descrip 生成Mipmaps
 * 
 * @functionName:  generateMipmaps
 * @functionType:    void
 * @param image
 * @param imageFormat
 * @param texWidth
 * @param texHeight
 * @param mipLevels
 */
void TextureResource::generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, int32_t mipLevels)
{
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(m_context.deviceContext->getPhysicalDevice(), imageFormat, &formatProperties);

	//检查对线性过滤功能的支持
	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
		throw std::runtime_error("texture image format does not support linear blitting!");
	}
	//录制任务
	auto commandbuffer=m_cmdManager.beginSingleTimeCommands();

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.image = image;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.levelCount = 1;

	int32_t mipWidth = texWidth;
	int32_t mipHeight = texHeight;
	for (int32_t i = 1; i < mipLevels; i++) {
		barrier.subresourceRange.baseMipLevel = i - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;//等待之前写操作完成，例如copyBufferToImage()的操作
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;//接下来进行读操作，例如vkcmdblitimage()

		//提交屏障
		vkCmdPipelineBarrier(commandbuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT,//源用于内存拷贝
			VK_PIPELINE_STAGE_TRANSFER_BIT,//目标用于vkcmdblitimage(),本质也是传输
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		VkImageBlit blit{};
		blit.srcOffsets[0] = { 0,0,0 };
		blit.srcOffsets[1] = { mipWidth,mipHeight,1 };
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;

		blit.dstOffsets[0] = { 0,0,0 };
		blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1,mipHeight > 1 ? mipHeight / 2 : 1 ,1 };
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;

		//录制命令到命令缓冲区command buffer中
		vkCmdBlitImage(commandbuffer,
			image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &blit,
			VK_FILTER_LINEAR);//线性过滤缩放

		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;//读mip[i-1]
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;//mip[i-1]只供shader使用，这个转换等待复制命令完成

		vkCmdPipelineBarrier(commandbuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		if (mipWidth > 1) mipWidth /= 2;
		if (mipHeight > 1) mipHeight /= 2;
	}

	//处理mip[i],从"写"状态到"着色器"可读
	barrier.subresourceRange.baseMipLevel = mipLevels - 1;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(commandbuffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
		0, nullptr,
		0, nullptr,
		1, &barrier);
	m_cmdManager.endSingleTimeCommands(commandbuffer);
}
