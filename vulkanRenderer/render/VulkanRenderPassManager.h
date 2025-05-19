#pragma once
#include<vulkan/vulkan.h>
#include"../init/ApplicationContext.h"

class VulkanRenderPassManager
{
public:
	VulkanRenderPassManager(
		ApplicationContext& context,
		VkFormat colorFormat,
		VkFormat depthFormat,
		VkSampleCountFlagBits msaaSamples);
	~VulkanRenderPassManager() = default;
	void createRenderPass(
		VkFormat colorFormat,
		VkFormat depthFormat,
		VkSampleCountFlagBits msaaSamples
	);//创建渲染流程
	VkRenderPass& getRenderPass() { return m_renderPass; }
	void renderpassCleanup();
private:
	ApplicationContext& m_context;

	VkRenderPass m_renderPass;//渲染流程
};

