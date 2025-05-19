#include "VulkanSyncObjectsManager.h"
#include"../const/Data.h"
#include"../init/AllHeads.h"

VulkanSyncObjectsManager::VulkanSyncObjectsManager(ApplicationContext& context,
	VkDevice device):
	m_context(context)
{
	createSyncObjects(device);
}




/**
 * @descrip 清理函数
 *
 * @functionName:  syncObjectsCleanup
 * @functionType:    void
 */
void VulkanSyncObjectsManager::syncObjectsCleanup()
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(m_context.deviceContext->getLogicalDevice(),
			m_renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(m_context.deviceContext->getLogicalDevice(),
			m_imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(m_context.deviceContext->getLogicalDevice(),
			m_inFlightFences[i], nullptr);
	}
}





/**
 * @descrip 创建同步对象
 * 
 * @functionName:  createSyncObjects
 * @functionType:    void
 */
void VulkanSyncObjectsManager::createSyncObjects(VkDevice device){
	m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;//创建同步信号的时候，设置初始状态为已设置,防止死锁

	//创建同步信号
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(device, &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS) {

			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}

}
