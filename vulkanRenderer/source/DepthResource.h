#pragma once
#include "IVulkanResourceManager.h"
#include"../render/VulkanSwapChainManager.h"
class DepthResource :
    public IVulkanResourceManager
{
public:
    DepthResource(ApplicationContext& context,
        VulkanCommandManager& cmdManager,
        VulkanSwapChainManager &swapChainManager);
    void createDepthResources();//创建深度资源

private:
    VkImage m_depthImage;//深度图像
    VkDeviceMemory m_depthImageMemory;//深度图像内存
    VkImageView m_depthImageView;//深度视图
    VulkanSwapChainManager m_swapChainManager;//交换链依赖注入
};

