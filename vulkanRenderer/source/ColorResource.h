#pragma once
#include "IVulkanResourceManager.h"
#include"../render/VulkanSwapChainManager.h"
class ColorResource :
    public IVulkanResourceManager
{
public:
    ColorResource(ApplicationContext& context,
        VulkanCommandManager& cmdManager,
        VulkanSwapChainManager& swapChainManager);
    void createColorResources();//创建多采样颜色缓冲区
    VkImageView &getColorImageView() { return m_colorImageView; };
    VkImage &getColorImage() { return m_colorImage; }
    VkDeviceMemory &getcolorImageMemory() { return m_colorImageMemory; }


private:
    VkImage m_colorImage;//多采样图像
    VkDeviceMemory m_colorImageMemory;//多采样图像内存
    VkImageView m_colorImageView;//多采样图像视图
    VulkanSwapChainManager &m_swapChainManager;//交换链依赖注入



};

