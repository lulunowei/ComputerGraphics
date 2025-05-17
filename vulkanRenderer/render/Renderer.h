#pragma once
#include"../const.h"
#include"../init/ApplicationContext.h"
#include"VulkanSwapChainManager.h"
#include"VulkanRenderPassManager.h"
class Renderer
{
public:
	Renderer(ApplicationContext& context);
	~Renderer() = default;

	void init();
	void draw();
	void rendererCleanup();
private:
	ApplicationContext& m_context;

	std::unique_ptr<VulkanSwapChainManager> m_swapchainManager;
	std::unique_ptr<VulkanRenderPassManager> m_renderpassManager;
};

