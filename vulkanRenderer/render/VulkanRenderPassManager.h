#pragma once
#include"../init/ApplicationContext.h"
class VulkanRenderPassManager
{
public:
	VulkanRenderPassManager(
		ApplicationContext& context);
	~VulkanRenderPassManager() = default;
	void createRenderPass(
		VkFormat colorFormat,
		VkFormat depthFormat,
		VkSampleCountFlagBits msaaSamples
	);//创建渲染流程
private:
	ApplicationContext& m_context;

	VkRenderPass m_renderPass;//渲染流程
};

