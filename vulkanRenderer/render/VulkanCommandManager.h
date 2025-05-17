#pragma once
#include"../init/ApplicationContext.h"
class VulkanCommandManager
{
public:
	VulkanCommandManager(ApplicationContext& context);
	~VulkanCommandManager() = default;

	void createCommandPool();//创建命令池
	void createCommandBuffers();//创建命令缓冲区

	VkCommandBuffer beginSingleTimeCommands();//单次开始创建命令
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);//单次结束录制命令

	VkCommandPool getCommandPool() { return m_commandPool; }
	std::vector<VkCommandBuffer> getCommandBuffers() { return m_commandBuffers; }


private:
	ApplicationContext& m_context;

	VkCommandPool m_commandPool;//命令池
	std::vector<VkCommandBuffer> m_commandBuffers;//命令
};

