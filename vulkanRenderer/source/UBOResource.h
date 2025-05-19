#pragma once
#include "IVulkanResourceManager.h"
class UBOResource :
    public IVulkanResourceManager
{
public:
    UBOResource(ApplicationContext& context,
        VulkanCommandManager& cmdManager);
    std::vector<VkBuffer> &getUniformBuffers() { return m_uniformBuffers; }
    std::vector<void*> &getUniformBuffersMapped() { return m_uniformBuffersMapped; }
    void UBOCleanup();
private:
    void createUniformBuffers();//创建统一资源缓冲区

private:
    std::vector<VkBuffer> m_uniformBuffers;//统一资源缓冲区
    std::vector<VkDeviceMemory> m_uniformBuffersMemory;//统一资源内存
    std::vector<void*> m_uniformBuffersMapped;//统一资源映射

};

