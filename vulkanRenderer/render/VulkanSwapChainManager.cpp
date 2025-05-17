#include "VulkanSwapChainManager.h"
#include<vulkan/vulkan.h>

VulkanSwapChainManager::VulkanSwapChainManager(ApplicationContext& context):
	m_context(context)
{
	
}

void VulkanSwapChainManager::swapChainCleanup()
{
	for (auto imageView : m_swapChainImageViews) {
		vkDestroyImageView(m_context.deviceContext->getLogicalDevice(),
			imageView, 
			nullptr);
	}
	vkDestroySwapchainKHR(m_context.deviceContext->getLogicalDevice(), 
		m_swapChain,
		nullptr);
}

/**
 * @descrip 创建交换链
 * 
 * @functionName:  createSwapChain
 * @functionType:    void
 */
void VulkanSwapChainManager::createSwapChain()
{
	//查询到的交换链信息
	auto swapChainSupport =m_context.deviceContext->querySwapChainSupport(
		m_context.deviceContext->getPhysicalDevice(),
		m_context.instanceContext->getSurface());

	//从formats,presentModes,capabilities选择
	VkSurfaceFormatKHR surfaceFormat = m_context.deviceContext->chooseSwapSurfaceFormat(swapChainSupport.formats);//选择合适的交换链格式
	VkPresentModeKHR presentMode = m_context.deviceContext->chooseSwapPresentMode(swapChainSupport.presentModes);//选择合适的显示模式
	VkExtent2D extent = m_context.deviceContext->chooseSwapExtent(swapChainSupport.capabilities,m_context.windowContext->getWindow());//处理分辨率

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;//设置最少支持的图片数量+1，减少延迟

	//设置支持的最大图片数量，规定：maxImageCount==0代表是无限制
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		//超过限制，设置为最大值
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	//填充结构体
	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_context.instanceContext->getSurface();//绑定一个surface

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;//图像的层数，2D为1，3D为6
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;//指定交换链图像的用途

	QueueFamilyIndices indices = m_context.deviceContext->findQueueFamilies(
		m_context.deviceContext->getPhysicalDevice(),
		m_context.instanceContext->getSurface());//获取队列家族索引
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(),
		indices.presentFamily.value() };
	//不同家族，设置为并发模式
	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;//设置独占模式
	}
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;//默认不进行变换
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;// ignore the alpha channel
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;//开启裁剪模式，减少渲染，提高效率
	createInfo.oldSwapchain = VK_NULL_HANDLE;//创建操作中没有旧的交换链，不需要重新创建或者销毁

	//创建交换链
	if (vkCreateSwapchainKHR(m_context.deviceContext->getLogicalDevice(), &createInfo, nullptr, &m_swapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}
	vkGetSwapchainImagesKHR(m_context.deviceContext->getLogicalDevice(),
		m_swapChain, 
		&imageCount, 
		nullptr);//查询交换链图片数量
	m_swapChainImages.resize(imageCount);//调整图像句柄大小

	vkGetSwapchainImagesKHR(m_context.deviceContext->getLogicalDevice(),
		m_swapChain,
		&imageCount,
		m_swapChainImages.data());//存入图像句柄数组
	//保存交换链图像格式和分辨率后续使用
	m_swapChainImageFormat = surfaceFormat.format;
	m_swapChainExtent = extent;

}

/**
 * @descrip 创建图像视图，每张VKImage都是一个资源图，不能直接用来采样或者渲染
 * 
 * @functionName:  createImageViews
 * @functionType:    void
 */
void VulkanSwapChainManager::createImageViews()
{
	//通过资源图创建图像视图
	m_swapChainImageViews.resize(m_swapChainImages.size());

	for (size_t i = 0; i < m_swapChainImages.size(); i++) {
		m_swapChainImageViews[i] =
			createImageView(m_swapChainImages[i],
				1,
				m_swapChainImageFormat,
				VK_IMAGE_ASPECT_COLOR_BIT);//创建普通图像视图

	}
}

/**
 * @descrip 抽象创建视图
 * 
 * @functionName:  createImageView
 * @functionType:    VkImageView
 * @param image
 * @param mipLevels
 * @param format
 * @param aspectFlags
 * @return    
 */
VkImageView VulkanSwapChainManager::createImageView(VkImage image, uint32_t mipLevels, VkFormat format, VkImageAspectFlags aspectFlags)
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
