#pragma once
#include<vector>
#include<vulkan/vulkan.h>
class ApplicationContext;

class VulkanSyncObjectsManager
{
public:
    VulkanSyncObjectsManager(ApplicationContext& m_context,VkDevice device);
    void syncObjectsCleanup();
    std::vector<VkSemaphore> &getImageAvailableSemaphores() {
        return m_imageAvailableSemaphores;
    }
    std::vector<VkSemaphore> &getRenderFinishedSemaphores() {
        return m_renderFinishedSemaphores;
    }
    std::vector<VkFence> &getinFlightFences() {
        return m_inFlightFences;
    }
private:
	void createSyncObjects(VkDevice device);//创建同步对象




private:
    ApplicationContext& m_context;

    //显式等待信号量
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    //显式等待信号量
    std::vector<VkFence> m_inFlightFences;
};

