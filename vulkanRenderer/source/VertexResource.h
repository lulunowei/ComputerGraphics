#pragma once
#include "IVulkanResourceManager.h"
class VertexResource :
    public IVulkanResourceManager
{
public:
    VertexResource(ApplicationContext& context,
        VulkanCommandManager& cmdManager);
    void createVertexBuffer();//创建顶点缓冲区

private:
    std::vector<Vertex> m_vertices;//自定义顶点属性
    VkBuffer m_vertexBuffer;//顶点缓冲区
    VkDeviceMemory m_vertexBufferMemory;//顶点缓冲区内存
};

