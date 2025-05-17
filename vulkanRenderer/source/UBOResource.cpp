#include "UBOResource.h"
#include"../const/Data.h"

UBOResource::UBOResource
(ApplicationContext& context, VulkanCommandManager& cmdManager):
	IVulkanResourceManager(context, cmdManager)
{
}

/**
 * @descrip 创建统一资源缓冲区
 * 
 * @functionName:  createUniformBuffers
 * @functionType:    void
 */
void UBOResource::createUniformBuffers()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	m_uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	m_uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
	m_uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);


	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		createBuffer(bufferSize,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			m_uniformBuffers[i], //逻辑内存
			m_uniformBuffersMemory[i]);//物理内存
		//持久映射，uniformBuffersMapped指向缓冲区数据
		vkMapMemory(m_context.deviceContext->getLogicalDevice(), m_uniformBuffersMemory[i], 0, bufferSize, 0, &m_uniformBuffersMapped[i]);
	}

}
