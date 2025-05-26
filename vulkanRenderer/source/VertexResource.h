#pragma once
#include "IVulkanResourceManager.h"
#include"../const/Data.h"
class VertexResource :
    public IVulkanResourceManager
{
public:
    VertexResource(ApplicationContext& context,
        VulkanCommandManager& cmdManager);
    
    std::vector<Vertex> &getVertices() { return m_vertices; }//返回顶点属性
    std::vector<uint32_t> &getIndices() { return m_indices; }// 索引
    VkBuffer &getVertexBuffer() { return m_vertexBuffer; }
    VkBuffer &getIndexBuffer() { return m_indexBuffer; }
    VkBuffer &getInstanceBuffer() { return m_instanceBuffer; }

    void VertexCleanup();//销毁


private:
    std::vector<Vertex> m_vertices;//自定义顶点属性
    VkBuffer m_vertexBuffer;//顶点缓冲区
    VkDeviceMemory m_vertexBufferMemory;//顶点缓冲区内存

    std::vector<uint32_t> m_indices;//自定义索引
    VkBuffer m_indexBuffer;//索引缓冲区
    VkDeviceMemory m_indexBufferMemory;//索引缓冲区内存

    std::vector<uint32_t>m_instanceIDs;//实例ID
    VkBuffer m_instanceBuffer;
    VkDeviceMemory m_instanceBufferMemory;
private:
    void createVertexBuffer();//创建顶点缓冲区
    void createIndexBuffer();//创建索引缓冲区
    void createInstanceBuffer();//创建实例缓冲区
};

