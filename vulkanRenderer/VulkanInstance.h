#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include<vulkan/vulkan.h>
#include"const.h"

/** vulkan实例类负责实例+debug
 * .
 */
class VulkanInstance
{
public:
	VulkanInstance() = default;
	~VulkanInstance() = default;
	void intanceCleanup();
	void createInstance();//创建实例
	void setupDebugMessenger();//创建调试消息的接收器
	void createSurface(GLFWwindow* m_window);//创建表面

	VkInstance getInstance() { return m_instance; }
	VkSurfaceKHR getSurface() { return m_surface; }

private:
	VkInstance  m_instance;
	VkDebugUtilsMessengerEXT m_debugMessenger;//句柄，引用调试消息的接收器
	VkSurfaceKHR m_surface;//Vulkan 窗口表面
};

