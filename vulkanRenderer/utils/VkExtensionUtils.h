#pragma once
#define GLFW_INCLUDE_VULKAN
#include"../const/Data.h"

class VkExtensionUtils
{
public:
	static std::vector<const char*> getRequiredExtensions();
	static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);//封装调试消息的创建信息
	static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger);
	static void DestroyDebugUtilsMessengerEXT(VkInstance instance,
		VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks* pAllocator);
	static bool checkValidationLayerSupport();//检查一个或多个 Vulkan 验证层是否在系统的 Vulkan 安装中可用
	static 	bool checkDeviceExtensionSupport(VkPhysicalDevice device);//检查物理设备是否支持需要的扩展


private:
	//静态的回调函数
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);
};

