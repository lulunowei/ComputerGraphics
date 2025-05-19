#include "DepthResource.h"
#include"../utils/VkFormatUtils.h"
#include"../init/AllHeads.h"

DepthResource::DepthResource(
	ApplicationContext& context,
	VulkanCommandManager& cmdManager,
	VulkanSwapChainManager& swapChainManager):
	IVulkanResourceManager(context, cmdManager),
	m_swapChainManager(swapChainManager)
{
	createDepthResources();
}
/**
 * @descrip 创建深度资源
 * 
 * @functionName:  createDepthResources
 * @functionType:    void
 */
void DepthResource::createDepthResources()
{
	//查找格式
	VkFormat depthFormat = VkFormatUtils::findDepthFormat(m_context.deviceContext->getPhysicalDevice());
	//构建深度图像缓存
	createImage(m_swapChainManager.getSwapChainExtent().width,
		m_swapChainManager.getSwapChainExtent().height,
		1,
		m_context.deviceContext->getMsaaSamples(),
		depthFormat,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		m_depthImage,
		m_depthImageMemory);

	m_depthImageView = createImageView(m_depthImage,
		1,
		depthFormat,
		VK_IMAGE_ASPECT_DEPTH_BIT);//创建深度视图

	//转换布局
	transitionImageLayout(m_depthImage,
		1,
		depthFormat,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

}
