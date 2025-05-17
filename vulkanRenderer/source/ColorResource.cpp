#include "ColorResource.h"

ColorResource::ColorResource(
	ApplicationContext& context,
	VulkanCommandManager& cmdManager,
	VulkanSwapChainManager& swapChainManager):
	IVulkanResourceManager(context, cmdManager),
	m_swapChainManager(swapChainManager)
{
}

/**
 * @descrip 创建多重采样缓冲区MSAA
 * 
 * @functionName:  createColorResources
 * @functionType:    void
 */
void ColorResource::createColorResources()
{
	VkFormat colorFormat = m_swapChainManager.getSwapChainImageFormat();

	//创建多重采样缓冲区
	createImage(
		m_swapChainManager.getSwapChainExtent().width,
		m_swapChainManager.getSwapChainExtent().height,
		1,
		m_context.deviceContext->getMsaaSamples(),//通过设备获取的最多采样数量
		colorFormat,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		m_colorImage,
		m_colorImageMemory
	);

	//创建多重采样图像视图
	m_colorImageView = createImageView(m_colorImage,
		1,
		colorFormat,
		VK_IMAGE_ASPECT_COLOR_BIT);

}
