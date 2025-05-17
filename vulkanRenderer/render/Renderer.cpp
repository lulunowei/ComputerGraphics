#include "Renderer.h"
#include"../utils/VkFormatUtils.h"

Renderer::Renderer(ApplicationContext& context):
	m_context(context),
	m_swapchainManager(std::make_unique<VulkanSwapChainManager>(context)),
	m_renderpassManager(std::make_unique<VulkanRenderPassManager>(context))
{
	
}

void Renderer::init()
{
	m_swapchainManager->createSwapChain();
	m_swapchainManager->createImageViews();
	m_renderpassManager->createRenderPass(
		m_swapchainManager->getSwapChainImageFormat(),
		//VK_FORMAT_D32_SFLOAT,
		VkFormatUtils::
		findDepthFormat(m_context.deviceContext->getPhysicalDevice()),
		VK_SAMPLE_COUNT_1_BIT//temp
	);
}

void Renderer::draw()
{
	//TODO
}

void Renderer::rendererCleanup()
{
	//销毁swapchain类
	if (m_swapchainManager) {
		m_swapchainManager->swapChainCleanup();
		m_swapchainManager.reset(); // 安全释放
	}
}
