#pragma once
#include<vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include"const.h"
#include"const/Data.h"
class VulkanDevice
{
public:
	VulkanDevice() = default;
	~VulkanDevice() = default;
	void deviceCleanup();

	void pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);//选择一个物理设备
	void createLogicalDevice(VkSurfaceKHR surface);//创建逻辑设备
	
	VkDevice getDevice() { return m_device; }
	VkQueue getGraphicsQueue() { return m_graphicsQueue; }
	VkQueue getPresentQueue() { return m_presentQueue; }



private:
	bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);//查找queue族
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);//swapchain的属性查询
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);//选择合适的swapchain格式
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);//选择合适的swapchain显示模式
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);//选择合适的swapchain分辨率


private:
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;//物理设备
	VkDevice m_device;//逻辑设备
	VkQueue m_graphicsQueue;//图形队列
	VkQueue m_presentQueue;//显示队列
};

