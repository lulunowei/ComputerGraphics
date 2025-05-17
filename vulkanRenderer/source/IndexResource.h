#pragma once
#include "IVulkanResourceManager.h"
class IndexResource :
    public IVulkanResourceManager
{
public:
    IndexResource(ApplicationContext& context,
        VulkanCommandManager& cmdManager);
    void createIndexBuffer();//创建索引缓冲区

private:
    std::vector<uint32_t> m_indices;//自定义索引
    VkBuffer m_indexBuffer;//索引缓冲区
    VkDeviceMemory m_indexBufferMemory;//索引缓冲区内存
};

