#pragma once
#include"../init/ApplicationContext.h"
class VulkanSwapChainManager
{
public:
	VulkanSwapChainManager(ApplicationContext& context);
	~VulkanSwapChainManager() = default;
	void swapChainCleanup();//销毁资源
	void createSwapChain();//创建交换链
	void createImageViews();//创建图像视图

	VkFormat getSwapChainImageFormat()const { return m_swapChainImageFormat; }
	VkExtent2D getSwapChainExtent() const { return m_swapChainExtent; }
private:
	VkImageView createImageView(VkImage image,
		uint32_t mipLevels,
		VkFormat format,
		VkImageAspectFlags aspectFlags);//抽象创建视图
private:
	ApplicationContext& m_context;
	VkSwapchainKHR m_swapChain;//swapchain句柄
	std::vector<VkImage> m_swapChainImages;//swapchain图像句柄，资源图，不允许采样或者渲染
	std::vector<VkImageView> m_swapChainImageViews;//swapchain图像视图
	VkFormat m_swapChainImageFormat;//swapchain图像格式
	VkExtent2D m_swapChainExtent;//swapchain图像分辨率
};

