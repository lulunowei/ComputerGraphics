#include "VulkanCommandManager.h"
#include"../init/AllHeads.h"


VulkanCommandManager::VulkanCommandManager(ApplicationContext& context):
	m_context(context)
{
	createCommandPool();
	createCommandBuffers();
}
/**
 * @descrip 创建命令池
 * 
 * @functionName:  createCommandPool
 * @functionType:    void
 */
void VulkanCommandManager::createCommandPool()
{
	//获取图形家族和呈现家族队列索引
	QueueFamilyIndices queueFamilyIndices = m_context.deviceContext->findQueueFamilies(
		m_context.deviceContext->getPhysicalDevice(),
		m_context.instanceContext->getSurface()
	);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;//保证可以每一帧单独重置命令缓冲区
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();//图形队列族索引

	//创建命令池
	if (vkCreateCommandPool(
		m_context.deviceContext->getLogicalDevice(),
		&poolInfo,
		nullptr,
		&m_commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}

}

/**
 * @descrip 创建命令缓冲区
 * 
 * @functionName:  createCommandBuffers
 * @functionType:    void
 */
void VulkanCommandManager::createCommandBuffers()
{
	m_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);//命令缓冲区的大小
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_commandPool;//指定哪个池子
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;//1。primary 从主命令缓冲区 2.SECONDARY 从次级命令缓冲区
	allocInfo.commandBufferCount = (uint32_t)m_commandBuffers.size();//分配的缓冲区数量

	if (vkAllocateCommandBuffers(
		m_context.deviceContext->getLogicalDevice(),
		&allocInfo,
		m_commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}

}

/**
 * @descrip 记录命令缓冲区
 * 
 * @functionName:  beginSingleTimeCommands
 * @functionType:    VkCommandBuffer
 * @return    
 */
VkCommandBuffer VulkanCommandManager::beginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_commandPool;//指定哪个池子
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;//1。primary 从主命令缓冲区 2.SECONDARY 从次级命令缓冲区
	allocInfo.commandBufferCount = 1;//分配的缓冲区数量

	VkCommandBuffer commandBuffer;
	//1分配临时命令缓冲区
	vkAllocateCommandBuffers(
		m_context.deviceContext->getLogicalDevice(),
		&allocInfo,
		&commandBuffer);

	//2开始录制命令
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // 仅使用一次命令缓冲区

	vkBeginCommandBuffer(commandBuffer, &beginInfo);
	return commandBuffer;

}

/**
 * @descrip 单次结束录制命令
 * 
 * @functionName:  endSingleTimeCommands
 * @functionType:    void
 * @param commandBuffer
 */
void VulkanCommandManager::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
	//结束录制命令
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	//3提交命令
	vkQueueSubmit(m_context.deviceContext->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(m_context.deviceContext->getGraphicsQueue());//等待GPU完成所有任务，CPU阻塞

	vkFreeCommandBuffers(m_context.deviceContext->getLogicalDevice(), m_commandPool, 1, &commandBuffer);

}

/**
 * @descrip 销毁池
 * 
 * @functionName:  commandCleanup
 * @functionType:    void
 */
void VulkanCommandManager::commandCleanup()
{
	vkDestroyCommandPool(m_context.deviceContext->getLogicalDevice(), m_commandPool, nullptr);

}
