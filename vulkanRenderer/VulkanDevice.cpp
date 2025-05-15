#include "VulkanDevice.h"
#include"utils/VkExtensionUtils.h"
void VulkanDevice::deviceCleanup()
{
	vkDestroyDevice(m_device, nullptr);
}

/**
 * @descrip 选择物理设备
 * 
 * @functionName:  pickPhysicalDevice
 * @functionType:    void
 */
void VulkanDevice::pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface)
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);//获取设备数量

	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
	//检查是否满足
	for (const auto& device : devices) {
		if (isDeviceSuitable(device, surface)) {
			m_physicalDevice = device;
			//msaaSamples = getMaxUsableSampleCount();//选择设备支持的采样点
			break;
		}
	}

	if (m_physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}
/**
 * @descrip 创建逻辑设备
 * 
 * @functionName:  createLogicalDevice
 * @functionType:    void
 */
void VulkanDevice::createLogicalDevice(VkSurfaceKHR surface)
{
	QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice,surface);//物理设备寻找列族

	//VkDeviceQueueCreateInfo queueCreateInfo{};
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};//定义动态数组存储创建信息

	std::set<uint32_t> uniqueQueueFamilies =
	{ indices.graphicsFamily.value(), indices.presentFamily.value() };//创建一个集合，用于存储物理设备支持的所有队列家族索引


	float queuePriority = 1.0f;//优先级
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;//存入队列索引，使用哪个队列族
		queueCreateInfo.queueCount = 1;//队列个数一般指定为1，可以在多线程构建命令缓冲区，再统一提交
		queueCreateInfo.pQueuePriorities = &queuePriority;//队列优先级设置
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.samplerAnisotropy = VK_TRUE;//启动Anisotropic Filtering，支持纹理缩放
	deviceFeatures.sampleRateShading = VK_TRUE;

	VkDeviceCreateInfo createInfo{};//指定我需要的队列，来自哪些队列
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

	createInfo.pEnabledFeatures = &deviceFeatures;//启用硬件功能

	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());;
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}
	//实现实例化逻辑设备
	if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	//从逻辑设备获取图形队列和显示队列，{逻辑设备句柄，队列族索引，队列索引，队列句柄}
	vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
	vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_graphicsQueue);


}



/**
 * @descrip 判断物理设备是否适合使用
 * 
 * @functionName:  isDeviceSuitable
 * @functionType:    bool
 * @param device
 * @param surface
 * @return    
 */
bool VulkanDevice::isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	QueueFamilyIndices indices =findQueueFamilies(device, surface);

	bool extensionsSupported = VkExtensionUtils::checkDeviceExtensionSupport(device);//物理设备适用swapchain拓展

	bool swapChainAdequate = false;
	//在扩展可行之后再寻找交换链是否支持
	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, surface);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();//保证至少有1个格式和1个显示模式
	}

	VkPhysicalDeviceFeatures supportedFeatures;
	//supportedFeatures.samplerAnisotropy = VK_TRUE;  // 必须启用这个功能
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures); //查询物理设备支持的可选项

	return indices.isComplete() && // 1. 队列族支持图形和呈现
		extensionsSupported && // 2. 支持交换链扩展
		swapChainAdequate && // 3. 交换链支持至少一个格式和一个呈现模式
		supportedFeatures.samplerAnisotropy;// 4. 支持各向异性过滤


}

/**
 * @descrip 
 * 
 * @functionName:  findQueueFamilies
 * @functionType:    QueueFamilyIndices
 * @param device
 * @return    
 */
QueueFamilyIndices VulkanDevice::findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);//获取队列家族的数量

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	//获取队列家族的属性
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	//寻找逻辑队列族的索引，是否满足图形渲染以及支持显示
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			//寻找到支持图形操作的队列，并且存储队列家族索引
			indices.graphicsFamily = i;
		}

		//检查给定的物理设备是否支持显示
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (presentSupport) {
			//寻找到支持呈现的队列，并且存储索引
			indices.presentFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;

}

/**
 * @descrip swapchain的属性查询
 * 
 * @functionName:  querySwapChainSupport
 * @functionType:    SwapChainSupportDetails
 * @param device
 * @return    
 */
SwapChainSupportDetails VulkanDevice::querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	SwapChainSupportDetails details;

	//1获取交换链信息
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
	//2获取交换链支持的格式
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}
	//3获取交换链显示模式
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}
	return details;
}

/**
 * @descrip 选择合适的swapchain交换格式
 * 
 * @functionName:  chooseSwapSurfaceFormat
 * @functionType:    VkSurfaceFormatKHR
 * @param availableFormats
 * @return    
 */
VkSurfaceFormatKHR VulkanDevice::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	//将结构体中查询到的格式数组遍历
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

/**
 * @descrip 选择合适的swapchain显示模式
 * 
 * @functionName:  chooseSwapPresentMode
 * @functionType:    VkPresentModeKHR
 * @param availablePresentModes
 * @return    
 */
VkPresentModeKHR VulkanDevice::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	//优先选择MAILBOX模式
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}
	//如果没有，则选择FIFO模式
	return VK_PRESENT_MODE_FIFO_KHR;
}

/**
 * @descrip 处理分辨率
 * 
 * @functionName:  chooseSwapExtent
 * @functionType:    VkExtent2D
 * @param capabilities
 * @return    
 */
VkExtent2D VulkanDevice::chooseSwapExtent(
	const VkSurfaceCapabilitiesKHR& capabilities,
	GLFWwindow* window)
{
	//这个分辨率不是 uint32_t 的最大值，那么它表示窗口管理器已经为我们提供了一个固定的分辨率,直接使用
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);//返回的是以像素为单位的分辨率

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};
		// 限制分辨率范围
		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}





