#pragma once
#include"../const.h"
#include"../init/ApplicationContext.h"
#include"VulkanSwapChainManager.h"
#include"VulkanRenderPassManager.h"
#include"VulkanCommandManager.h"
#include"VulkanSyncObjectsManager.h"
#include"../source/AllSource.h"
#include"../const/Data.h"
class Renderer
{
public:
	Renderer(ApplicationContext& context);
	~Renderer() = default;

	void init();
	void draw();
	void rendererCleanup();
private:
	//基础组件
	ApplicationContext& m_context;

	//渲染流程类
	std::shared_ptr<VulkanSwapChainManager> m_swapchainManager;
	std::shared_ptr<VulkanRenderPassManager> m_renderpassManager;
	std::shared_ptr<VulkanCommandManager> m_commandManager;
	std::shared_ptr<VulkanSyncObjectsManager> m_syncObjectManager;

	//资源类
	std::unique_ptr<SourceContext> m_SourceContext;

	uint32_t currentFrame = 0;
private:
	void drawFrame();//绘制一帧
	void recreateSwapChain();//重新绘制交换链
	void cleanupSwapChain();//清理交换链
	void updateUniformBuffer(uint32_t currentImage);//更新全局数据
	void updateCubeUniformBuffer(uint32_t currentImage);//更新立方体全局数据
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);//记录命令缓冲区

};

