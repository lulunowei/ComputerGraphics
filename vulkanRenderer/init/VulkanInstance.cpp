#include "VulkanInstance.h"
#include"../utils/VkExtensionUtils.h"

VulkanInstance::VulkanInstance(GLFWwindow* m_window)
{
	createInstance();//创建vulkan实例
	setupDebugMessenger();//debug信息
	createSurface(m_window);
}

void VulkanInstance::intanceCleanup()
{
	if (enableValidationLayers) {
		VkExtensionUtils::DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
	}
	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	vkDestroyInstance(m_instance, nullptr);
}

/**
 * @descrip 创建vulkan实例
 * 
 * @functionName:  createInstance
 * @functionType:    void
 */
void VulkanInstance::createInstance()
{
	if (enableValidationLayers && !VkExtensionUtils::checkValidationLayerSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Model";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = VkExtensionUtils::getRequiredExtensions();//获取拓展指针
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();//开启扩展，包括支持surface显示的扩展

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		// 封装调试消息的结构体创建信息
		VkExtensionUtils::populateDebugMessengerCreateInfo(debugCreateInfo);
		//一步到位，将调试回调挂在初始化instance中
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	}
}

/**
 * @descrip 创建调试消息的接收器
 * 
 * @functionName:  setupDebugMessenger
 * @functionType:    void
 */
void VulkanInstance::setupDebugMessenger()
{
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	//装载信息到结构体
	VkExtensionUtils::populateDebugMessengerCreateInfo(createInfo);

	if (VkExtensionUtils::CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

/**
 * @descrip 创建表面，调用glfw接口
 * 
 * @functionName:  createSurface
 * @functionType:    void
 */
void VulkanInstance::createSurface(GLFWwindow* m_window)
{
	if (glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}
}






