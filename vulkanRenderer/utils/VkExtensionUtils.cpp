#include "VkExtensionUtils.h"
#include<vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include"../const.h"
/**
 * @descrip 获取与 GLFW 配合使用时所需的 Vulkan 扩展
 * 
 * @functionName:  getRequiredExtensions
 * @functionType:    std::vector<const char*>
 * @return    
 */
std::vector<const char*> VkExtensionUtils::getRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;

}

/**
 * @descrip 封装调试消息的创建信息
 * 
 * @functionName:  populateDebugMessengerCreateInfo
 * @functionType:    void
 * @param createInfo
 */
void VkExtensionUtils::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
}

/**
 * @descrip 创建调试消息的接收器，由于扩展函数不能直接调用，需要获取地址
 * 
 * @functionName:  CreateDebugUtilsMessengerEXT
 * @functionType:    VkResult
 * @param instance
 * @param pCreateInfo
 * @param pAllocator
 * @param pDebugMessenger
 * @return    
 */
VkResult VkExtensionUtils::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	//获取指定 Vulkan 实例中的扩展的函数地址
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

/**
 * @descrip 销毁调试消息的接收器
 * 
 * @functionName:  DestroyDebugUtilsMessengerEXT
 * @functionType:    void
 * @param instance
 * @param debugMessenger
 * @param pAllocator
 */
void VkExtensionUtils::DestroyDebugUtilsMessengerEXT(VkInstance instance,
	VkDebugUtilsMessengerEXT debugMessenger,
	const VkAllocationCallbacks* pAllocator)
{
	//成功，直接调用销毁函数
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

/**
 * @descrip 初始化 Vulkan 实例之前，确保所需的验证层可以被加载和使用
 * 
 * @functionName:  checkValidationLayerSupport
 * @functionType:    bool
 * @return    只有当所有所需的验证层都能找到时，函数才返回 true，否则返回 false
 */
bool VkExtensionUtils::checkValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

/**
 * @descrip 检查物理设备是否支持需要的扩展
 * 
 * @functionName:  checkDeviceExtensionSupport
 * @functionType:    bool
 * @param device
 * @return    
 */
bool VkExtensionUtils::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);//查看可用扩展数量

	//获取物理设备支持的拓展
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());//获取可用扩展

	//定义所有需要的扩展名字
	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());//获取包含所有必需扩展名称的容器

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}
	//如果容器为空，则表示所有必需的扩展都已被设备支持
	return requiredExtensions.empty();

}

/**
 * @descrip
 * 
 * @functionName:  findMemoryType
 * @functionType:    uint32_t
 * @param typeFilter
 * @param properties
 * @return    
 */
uint32_t VkExtensionUtils::findMemoryType(
	VkPhysicalDevice device,
	uint32_t typeFilter,
	VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(device, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) &&
			(memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	throw std::runtime_error("failed to find suitable memory type!");
}

/**
 * @descrip 静态的回调函数
 * 
 * @functionName:  debugCallback
 * @functionType:    VKAPI_ATTR VkBool32 VKAPI_CALL
 * @param messageSeverity
 * @param messageType
 * @param pCallbackData
 * @param pUserData
 * @return    
 */
VKAPI_ATTR VkBool32 VKAPI_CALL VkExtensionUtils::debugCallback
(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;//返回true调用程序会终止
}
