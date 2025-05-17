#include "VertexResource.h"

VertexResource::VertexResource(ApplicationContext& context, VulkanCommandManager& cmdManager):
	IVulkanResourceManager(context, cmdManager)
{
}

/**
 * @descrip 创建顶点缓冲
 * 
 * @functionName:  createVertexBuffer
 * @functionType:    void
 */
void VertexResource::createVertexBuffer()
{
	//VkDeviceSize bufferSize = sizeof(rectangle::vertices[0]) * (rectangle::vertices.size());
	VkDeviceSize bufferSize = sizeof(m_vertices[0]) * (m_vertices.size());

	VkBuffer stagingBuffer;//缓冲区
	VkDeviceMemory stagingBufferMemory;//缓冲区内存

	//创建CPU内存
	createBuffer(bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,// 缓冲区用作内存传的数据源
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer,
		stagingBufferMemory);

	void* data;//data是能直接内存拷贝的指针
	vkMapMemory(m_context.deviceContext->getLogicalDevice(), stagingBufferMemory, 0, (size_t)bufferSize, 0, &data);//GPU映射到CPU
	//memcpy(data, rectangle::vertices.data(), (size_t)bufferSize);//通过data拷贝数据到data指向的stagingBufferMemory
	memcpy(data, m_vertices.data(), (size_t)bufferSize);//通过data拷贝数据到data指向的stagingBufferMemory
	vkUnmapMemory(m_context.deviceContext->getLogicalDevice(), stagingBufferMemory);//取消映射

	//创建GPU内存
	createBuffer(bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,// 缓冲区用作内存传的输出
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		m_vertexBuffer,
		m_vertexBufferMemory);

	//录制拷贝命令，将CPU准备好的内存数据移动到GPU显存
	copyBuffer(stagingBuffer, m_vertexBuffer, bufferSize);

	vkDestroyBuffer(m_context.deviceContext->getLogicalDevice(), stagingBuffer, nullptr);
	vkFreeMemory(m_context.deviceContext->getLogicalDevice(), stagingBufferMemory, nullptr);

}
