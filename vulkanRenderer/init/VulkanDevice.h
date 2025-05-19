#pragma once
#include<vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include"../const.h"
#include"../const/Data.h"
class VulkanDevice
{
public:
	VulkanDevice(VkInstance instance, VkSurfaceKHR surface);
	~VulkanDevice() = default;
	void deviceCleanup();

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);//查找queue族
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);//swapchain的属性查询
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);//选择合适的swapchain格式
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);//选择合适的swapchain显示模式
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);//选择合适的swapchain分辨率


	void pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);//选择一个物理设备
	void createLogicalDevice(VkSurfaceKHR surface);//创建逻辑设备
	
	VkDevice &getLogicalDevice() { return m_logicalDevice; }
	VkQueue &getGraphicsQueue() { return m_graphicsQueue; }
	VkQueue &getPresentQueue() { return m_presentQueue; }
	VkPhysicalDevice &getPhysicalDevice() { return m_physicalDevice; }
	VkSampleCountFlagBits &getMsaaSamples() { return msaaSamples; }

private:
	bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
	VkSampleCountFlagBits getMaxUsableSampleCount();//获取深度和颜色采样数

private:
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;//物理设备
	VkDevice m_logicalDevice;//逻辑设备
	VkQueue m_graphicsQueue;//图形队列
	VkQueue m_presentQueue;//显示队列
	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;//采样点

};

