#include "IndexResource.h"
#include"../const/Data.h"
#include"../init/AllHeads.h"

IndexResource::IndexResource(ApplicationContext& context, VulkanCommandManager& cmdManager):
	IVulkanResourceManager(context, cmdManager)

{
}

/**
 * @descrip 创建索引缓冲区
 * 
 * @functionName:  createIndexBuffer
 * @functionType:    void
 */
void IndexResource::createIndexBuffer()
{
	//auto bufferSize=sizeof(rectangle::indices[0])* (rectangle::indices.size());
	auto bufferSize = sizeof(m_indices[0]) * (m_indices.size());

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	createBuffer(bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer,
		stagingBufferMemory);

	void* data;
	vkMapMemory(m_context.deviceContext->getLogicalDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	//memcpy(data, rectangle::indices.data(), (size_t)bufferSize);
	memcpy(data, m_indices.data(), (size_t)bufferSize);
	vkUnmapMemory(m_context.deviceContext->getLogicalDevice(), stagingBufferMemory);

	createBuffer(bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		m_indexBuffer,
		m_indexBufferMemory);

	copyBuffer(stagingBuffer, m_indexBuffer, bufferSize);

	vkDestroyBuffer(m_context.deviceContext->getLogicalDevice(), stagingBuffer, nullptr);
	vkFreeMemory(m_context.deviceContext->getLogicalDevice(), stagingBufferMemory, nullptr);
}
