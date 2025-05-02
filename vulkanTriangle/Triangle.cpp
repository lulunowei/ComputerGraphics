#include "Triangle.h"
#define STB_IMAGE_IMPLEMENTATION//stb头文件包含实现函数实现，只有#include会发生链接错误
using namespace myVertexData;
using namespace myVertexData::rectangle;
//using namespace myVertexData::circle;
using namespace matrixTransform;
#include <stb_image.h>
/**
 * @descrip 初始化Vulkan
 *
 * @functionName:  initVulkan
 * @functionType:    void
 */
void Triangle::initVulkan()
{
	createInstance();
	setupDebugMessenger();
	createSurface();//创建窗口表面
	pickPhysicalDevice();
	createLogicalDevice();//创建逻辑设备
	createSwapChain();//创建交换链
	createImageViews();//创建图像视图
	createTextureSampler();//创建纹理采样器
	createRenderPass();//创建渲染流程
	createDescriptorSetLayout();//创建描述符布局
	createGraphicsPipeline();//创建图形管道
	createFramebuffers();//创建帧缓冲
	createCommandPool();//创建命令池
	createDepthResources();//创建深度资源
	createTextureImage();//创建纹理图片
	createTextureImageView();//创建纹理视图
	createVertexBuffer();//创建顶点缓冲区
	createIndexBuffer();//创建索引缓冲区
	createUniformBuffers();//创建统一资源缓冲区
	createDescriptorPool();//创建统一资源池
	createDescriptorSets();//创建统一资源集合
	createCommandBuffers();//创建命令缓冲
	createSyncObjects();//创建信号量

}

/**
 * @descrip 主循环
 *
 * @functionName:  mainLoop
 * @functionType:    void
 */
void Triangle::mainLoop()
{
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();//监听窗口事件，移动，缩放等事件
		drawFrame();//绘制一帧
	}

	vkDeviceWaitIdle(device);//等待特定命令队列中的操作完成
}

/**
 * @descrip 窗口销毁
 *
 * @functionName:  cleanup
 * @functionType:    void
 */
void Triangle::cleanup()
{
	cleanupSwapChain();


	vkDestroyPipeline(device, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	vkDestroyRenderPass(device, renderPass, nullptr);

	for (unsigned int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroyBuffer(device, uniformBuffers[i], nullptr);//释放统一资源缓存
		vkFreeMemory(device, uniformBuffersMemory[i], nullptr);//释放统一资源内存
	}

	vkDestroyDescriptorPool(device, descriptorPool, nullptr);//释放描述符池

	vkDestroySampler(device, textureSampler, nullptr);//释放采样器
	vkDestroyImageView(device, textureImageView, nullptr);//释放图像视图
	vkDestroyImage(device, textureImage, nullptr);
	vkFreeMemory(device, textureImageMemory, nullptr);

	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);//释放资源描述符布局

	vkDestroyBuffer(device, indexBuffer, nullptr);//释放索引缓存区
	vkFreeMemory(device, indexBufferMemory, nullptr);//释放索引内存

	vkDestroyBuffer(device, vertexBuffer, nullptr);//释放顶点缓存区
	vkFreeMemory(device, vertexBufferMemory, nullptr);//释放顶点内存

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(device, inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(device, commandPool, nullptr);

	vkDestroyDevice(device, nullptr);

	if (enableValidationLayers) {
		//开启验证层，调用销毁验证层函数
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}

	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);//销毁实例

	glfwDestroyWindow(window);

	glfwTerminate();
}

/**
 * @descrip 初始化窗口
 *
 * @functionName:  initWindow
 * @functionType:    void
 */
void Triangle::initWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "三角形", nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);//回调，如果我改变窗口大小，就会调用这个回调函数
}

/**
 * @descrip 创建实例
 *
 * @functionName:  createInstance
 * @functionType:    void
 */
void Triangle::createInstance()
{
	if (enableValidationLayers && !checkValidationLayerSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();//开启扩展，包括支持surface显示的扩展

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		// 封装调试消息的结构体创建信息
		populateDebugMessengerCreateInfo(debugCreateInfo);
		//一步到位，将调试回调挂在初始化instance中
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	}
}
/**
 * @descrip 创建调试消息的接收器
 *
 * @functionName:  setupDebugMessenger
 * @functionType:    void
 */
void Triangle::setupDebugMessenger()
{
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	//装载信息到结构体
	populateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

/**
 * @descrip 封装调试消息的创建信息
 *
 * @functionName:  populateDebugMessengerCreateInfo
 * @functionType:    void
 * @param createInfo
 */
void Triangle::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
}

/**
 * @descrip 获取与 GLFW 配合使用时所需的 Vulkan 扩展
 *
 * @functionName:  getRequiredExtensions
 * @functionType:    std::vector<const char*>
 * @return
 */
std::vector<const char*> Triangle::getRequiredExtensions()
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
 * @descrip 初始化 Vulkan 实例之前，确保所需的验证层可以被加载和使用
 *
 * @functionName:  checkValidationLayerSupport
 * @functionType:    bool
 * @return    只有当所有所需的验证层都能找到时，函数才返回 true，否则返回 false
 */
bool Triangle::checkValidationLayerSupport()
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
 * @descrip 选择物理设备
 *
 * @functionName:  pickPhysicalDevice
 * @functionType:    void
 */
void Triangle::pickPhysicalDevice()
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
		if (isDeviceSuitable(device)) {
			physicalDevice = device;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

/**
 * @descrip 判断物理设备是否适合使用
 *
 * @functionName:  isDeviceSuitable
 * @functionType:    bool
 * @param device
 * @return
 */
bool Triangle::isDeviceSuitable(VkPhysicalDevice device)
{
	QueueFamilyIndices indices = findQueueFamilies(device);

	bool extensionsSupported = checkDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	//在扩展可行之后再寻找交换链是否支持
	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();//保证至少有1个格式和1个显示模式
	}
	//
	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures); //查询物理设备支持的可选项

	return indices.isComplete() && // 1. 队列族支持图形和呈现
		extensionsSupported && // 2. 支持交换链扩展
		swapChainAdequate&& // 3. 交换链支持至少一个格式和一个呈现模式
		supportedFeatures.samplerAnisotropy;// 4. 支持各向异性过滤
}

/**
 * @descrip 遍历物理设备的所有队列家族，寻找支持图形操作的队列并且返回
 *
 * @functionName:  findQueueFamilies
 * @functionType:    QueueFamilyIndices
 * @param device
 * @return
 */
QueueFamilyIndices Triangle::findQueueFamilies(VkPhysicalDevice device)
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
			//寻找到支持图形操作的队列，并且存储索引
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
 * @descrip 创建逻辑设备
 *
 * @functionName:  createLogicalDevice
 * @functionType:    void
 */
void Triangle::createLogicalDevice()
{
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);//物理设备寻找列族

	//VkDeviceQueueCreateInfo queueCreateInfo{};
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};//定义动态数组存储创建信息

	std::set<uint32_t> uniqueQueueFamilies =
	{ indices.graphicsFamily.value(), indices.presentFamily.value() };//创建一个集合，用于存储物理设备支持的所有队列家族索引


	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;//使用哪个队列族
		queueCreateInfo.queueCount = 1;//队列个数一般指定为1，可以在多线程构建命令缓冲区，再统一提交
		queueCreateInfo.pQueuePriorities = &queuePriority;//队列优先级设置
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.samplerAnisotropy = VK_TRUE;//启动Anisotropic Filtering，支持纹理缩放

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
	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	//从逻辑设备获取图形队列和显示队列，{逻辑设备句柄，队列族索引，队列索引，队列句柄}
	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);

	
}

/**
 * @descrip 创建表面，调用glfw接口
 *
 * @functionName:  createSurface
 * @functionType:    void
 */
void Triangle::createSurface()
{
	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}
}

/**
 * @descrip 检查设备是否支持扩展
 *
 * @functionName:  checkDeviceExtensionSupport
 * @functionType:    bool
 * @param device
 * @return
 */
bool Triangle::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);//查看可用扩展数量

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());//获取可用扩展

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());//获取包含所有必需扩展名称的容器

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}
	//如果容器为空，则表示所有必需的扩展都已被设备支持
	return requiredExtensions.empty();
}

/**
 * @descrip swapchain支持信息
 *
 * @functionName:  querySwapChainSupport
 * @functionType:    SwapChainSupportDetails
 * @param device
 * @return
 */
SwapChainSupportDetails Triangle::querySwapChainSupport(VkPhysicalDevice device)
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
VkSurfaceFormatKHR Triangle::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
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
VkPresentModeKHR Triangle::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
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
VkExtent2D Triangle::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
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

/**
 * @descrip 创建交换链
 *
 * @functionName:  createSwapChain
 * @functionType:    void
 */
void Triangle::createSwapChain()
{
	//查询到的交换链信息
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

	//从formats,presentModes,capabilities选择
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);//选择合适的交换链格式
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);//选择合适的显示模式
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);//处理分辨率

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;//设置最少支持的图片数量+1，减少延迟

	//设置支持的最大图片数量，规定：maxImageCount==0代表是无限制
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		//超过限制，设置为最大值
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	//填充结构体
	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;//绑定一个surface

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;//图像的层数，2D为1，3D为6
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;//指定交换链图像的用途

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);//获取队列家族索引
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	//不同家族，设置为并发模式
	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;//设置独占模式
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;//默认不进行变换
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;// ignore the alpha channel

	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;//开启裁剪模式，减少渲染，提高效率

	createInfo.oldSwapchain = VK_NULL_HANDLE;//创建操作中没有旧的交换链，不需要重新创建或者销毁

	//创建交换链
	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}


	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);//查询交换链图片数量
	swapChainImages.resize(imageCount);//调整图像句柄大小
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());//存入图像句柄数组

	//保存交换链图像格式和分辨率后续使用
	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;


}
/**
 * @descrip 抽象创建视图
 * 
 * @functionName:  createImageView
 * @functionType:    VkImageView
 * @return    
 */
VkImageView Triangle::createImageView(VkImage image, 
	VkFormat format,
	VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.format = format;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	//指定纹理视图看到的是图像的哪一部分
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture image view!");
	}

	return imageView;
}

/**
 * @descrip 创建图像视图，每张VKImage都是一个资源图，不能直接用来采样或者渲染
 *
 * @functionName:  createImageViews
 * @functionType:    void
 */
void Triangle::createImageViews()
{
	//通过资源图创建图像视图
	swapChainImageViews.resize(swapChainImages.size());

	for (size_t i = 0; i < swapChainImages.size(); i++) {
		swapChainImageViews[i] = 
			createImageView(swapChainImages[i],
				swapChainImageFormat,
				VK_IMAGE_ASPECT_COLOR_BIT);//创建普通图像视图

	}
}
/**
 * @descrip 创建描述符布局，类似绑定顶点属性和location索引
 * 
 * @functionName:  createDescriptorSetLayout
 * @functionType:    void
 */
void Triangle::createDescriptorSetLayout()
{
	//ubo描述符布局
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;//只有一个uniform buffer
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;//着色器阶段
	uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

	//sampler描述符布局
	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;//片段着色器中使用组合图像采样器描述符
	samplerLayoutBinding.pImmutableSamplers = nullptr;

	//布局存放到一个数组
	std::array<VkDescriptorSetLayoutBinding, 2> bindings{ uboLayoutBinding,samplerLayoutBinding };

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

/**
 * @descrip 创建图形管道
 *
 * @functionName:  createGraphicsPipeline
 * @functionType:    void
 */
void Triangle::createGraphicsPipeline()
{
	auto vertShaderCode = FileUtils::readFile("shaders/vert.spv");
	auto fragShaderCode = FileUtils::readFile("shaders/frag.spv");

	//封装字节码
	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	//创建vertext shader
	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;//着色阶段：顶点着色阶段
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

	//创建fragment shader
	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };//保存到数组

	//vertex shader顶点的输入信息
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};

		auto bindingDescription = myVertexData::Vertex::getBindingDescription();
		auto attributeDescriptions = myVertexData::getAttributeDescriptions();

		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		//vertexInputInfo.vertexBindingDescriptionCount = 0;
		//vertexInputInfo.vertexAttributeDescriptionCount = 0;

		//绑定描述和属性
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());//顶点属性数量
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;//描述顶点结构
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();//描述结构体字段



	//顶点数据组织成几何图元
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;//每三个顶点组成三角形
		//inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

	//视口和裁剪
	VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

	//光栅化变成片元
	VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;//
		rasterizer.depthBiasEnable = VK_FALSE;

	//多重采样
	VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;//默认一次采样
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;//不使用MSAA

	//颜色混合
	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;

	//全局配置颜色混合
	VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

	//声明视口和剪裁区域为动态状态
	std::vector<VkDynamicState> dynamicStates = {
		  VK_DYNAMIC_STATE_VIEWPORT,
		  VK_DYNAMIC_STATE_SCISSOR
	};
	VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

	//声明管线布局，用于访问外部资源
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;//创建的管线绑定描述符布局
	//pipelineLayoutInfo.pushConstantRangeCount = 0;

	//创建管线布局
	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}

	//定义管线
	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;

	pipelineInfo.pVertexInputState = &vertexInputInfo;//管线的输入
	pipelineInfo.pInputAssemblyState = &inputAssembly;//管线的顶点组成assmebler
	pipelineInfo.pViewportState = &viewportState;//视口
	pipelineInfo.pRasterizationState = &rasterizer;//光栅化
	pipelineInfo.pMultisampleState = &multisampling;//多采样
	pipelineInfo.pDepthStencilState = nullptr; // Optional
	pipelineInfo.pColorBlendState = &colorBlending;//颜色混合
	pipelineInfo.pDynamicState = &dynamicState;//动态裁剪
	pipelineInfo.layout = pipelineLayout;//布局
	pipelineInfo.renderPass = renderPass;//绑定的RenderPass
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional

	//创建管线
	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	vkDestroyShaderModule(device, fragShaderModule, nullptr);
	vkDestroyShaderModule(device, vertShaderModule, nullptr);
}

/**
 * @descrip 创建着色器模块,封装着色器字节码
 *
 * @functionName:  createShaderModule
 * @functionType:    VkShaderModule
 * @param code
 * @return
 */
VkShaderModule Triangle::createShaderModule(const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

/**
 * @descrip 渲染流程，定义渲染前后怎么处理像素数据
 *
 * @functionName:  createRenderPass
 * @functionType:    void
 */
void Triangle::createRenderPass()
{
	//颜色附件
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;//渲染前清除附件内容(清除为黑色)
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;//渲染后保存附件内容
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;//不关心模板数据
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;//不关心模板数据
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;// 初始布局	,视为“未初始化”状态
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;// 最终布局,图像将被提交到交换链进行屏幕呈现。

	//深度附件
	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = findDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;//渲染前清除附件内容(清除为黑色)
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;//渲染后不用保存附件内容
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;//不关心模板数据
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;//不关心模板数据
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;// 初始布局	,视为“未初始化”状态
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;// 最适合 GPU 读取深度和模板缓冲的布局


	//。。。其他附件（深度等）


	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0; //引用主流程附件列表中的第一个附件
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;// 子流程运行时附件的布局

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1; //引用主流程附件列表中的第二个附件
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;// 子流程运行时附件的布局


	//子流程创建
	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;// 声明为图形渲染子流程
	subpass.colorAttachmentCount = 1;// 使用 1 个颜色附件	
	subpass.pColorAttachments = &colorAttachmentRef;// 指向颜色附件引用
	subpass.pDepthStencilAttachment = &depthAttachmentRef;//指向深度附件引用

	//创建渲染流程
	VkRenderPassCreateInfo renderPassInfo{};
	std::array<VkAttachmentDescription, 2> attachments = 
								{ colorAttachment, depthAttachment };
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());// 主流程的附件数量（此处只有颜色附件）
	renderPassInfo.pAttachments = attachments.data();// 主流程附件列表
	renderPassInfo.subpassCount = 1;// 子流程数量
	renderPassInfo.pSubpasses = &subpass;// 子流程配置

	//描述子通道的依赖关系
	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;//源
	dependency.dstSubpass = 0;//目标
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT|
		VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT|
		VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT|
		VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}
}

/**
 * @descrip 创建帧缓冲区
 *
 * @functionName:  createFramebuffers
 * @functionType:    void
 */
void Triangle::createFramebuffers()
{
	swapChainFramebuffers.resize(swapChainImageViews.size());//初始化大小与交换区图像的数量一致

	//遍历交换区视图
	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		VkImageView attachments[] = {
			swapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;//一个renderpass对应一个framebuffer
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

/**
 * @descrip 创建命令池
 *
 * @functionName:  createCommandPool
 * @functionType:    void
 */
void Triangle::createCommandPool()
{
	//获取图形家族和呈现家族队列索引
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;//保证可以每一帧单独重置命令缓冲区
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();//图形队列族索引

	//创建命令池
	if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}

/**
 * @descrip 开启一个一次性使用的命令缓冲区
 * 
 * @functionName:  beginSingleTimeCommands
 * @functionType:    VkCommandBuffer
 * @return    
 */
VkCommandBuffer Triangle::beginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;//指定哪个池子
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;//1。primary 从主命令缓冲区 2.SECONDARY 从次级命令缓冲区
	allocInfo.commandBufferCount = 1;//分配的缓冲区数量

	VkCommandBuffer commandBuffer;
	//1分配临时命令缓冲区
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	//2开始录制命令
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // 仅使用一次命令缓冲区

	vkBeginCommandBuffer(commandBuffer, &beginInfo);
	return commandBuffer;
}

/**
 * @descrip 单次结束录制命令
 * 
 * @functionName:  endSingleTimeCommands
 * @functionType:    void
 */
void Triangle::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
	//结束录制命令
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	//3提交命令
	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);//等待GPU完成所有任务，CPU阻塞

	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

/**
 * @descrip 创建命令缓冲区
 *
 * @functionName:  createCommandBuffer
 * @functionType:    void
 */
void Triangle::createCommandBuffers()
{
	commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);//命令缓冲区的大小
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;//指定哪个池子
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;//1。primary 从主命令缓冲区 2.SECONDARY 从次级命令缓冲区
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();//分配的缓冲区数量

	if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

/**
 * @descrip 处理图像布局转换，定义了图像内存屏障
 * 
 * @functionName:  transitionImageLayout
 * @functionType:    void
 * @param image  待处理图像
 * @param format 图像格式
 * @param oldLayout 原始布局
 * @param newLayout 转换布局
 */
void Triangle::transitionImageLayout(VkImage image, 
	VkFormat format,
	VkImageLayout oldLayout,
	VkImageLayout newLayout)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;

		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		barrier.srcAccessMask = 0; // 指定 在这个 barrier 之前 需要完成的访问类型
		barrier.dstAccessMask = 0; // 指定 在这个 barrier 之后 将会进行的访问类型


		VkPipelineStageFlags sourceStage;//它控制前一个操作在哪个阶段结束之后，barrier 才生效
		VkPipelineStageFlags destinationStage;//它控制新的操作在 barrier 之后、在哪个阶段才能开始执行。

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;//不需要等待之前的操作完成，设置为0
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;//目标掩码

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;//源阶段中，图像没有参与其他操作
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;//目标阶段是传输
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;//等待传输完成
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;//完成后进行shader读取

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;//源阶段是传输
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;//目标阶段是 片段着色器中读取纹理
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;//管线最初始阶段
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;//目标阶段，读取深度缓存
		}
		else {
			throw std::invalid_argument("unsupported layout transition!");
		}
		//提交屏障
		vkCmdPipelineBarrier(
			commandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);
	endSingleTimeCommands(commandBuffer);
}

/**
 * @descrip 创建纹理图
 * 
 * @functionName:  createTextureImage
 * @functionType:    void
 */
void Triangle::createTextureImage()
{
	int texWidth, texHeight, texChannels;
	//返回像素值数组的第一个元素
	stbi_uc* pixels = stbi_load("textures/texture.jpg",
		&texWidth,
		&texHeight,
		&texChannels,
		STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * texHeight * 4;

	if (!pixels) {
		throw std::runtime_error("failed to load texture image!");
	}

	createBuffer(imageSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer,
		stagingBufferMemory
	);//创建了一块vulkan管理的内存区，绑定到缓冲区对象stagingBuffer

	void* data;
	vkMapMemory(device,stagingBufferMemory,0,imageSize,0,&data);
		memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(device, stagingBufferMemory);

	stbi_image_free(pixels);

	createImage(static_cast<uint32_t>(texWidth),
		static_cast<uint32_t>(texHeight),
		VK_FORMAT_R8G8B8A8_SRGB,//指定图像格式，与输入的纹理图一致
		VK_IMAGE_TILING_OPTIMAL,//指定纹素排列顺序
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,//指定用途
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,//指定图像内存的属性，存储在GPU本地，不同与HOST_VISIBLE
		textureImage,
		textureImageMemory
	);
	//转换图像布局
	transitionImageLayout(textureImage,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	//CPU缓存到GPU图像对象
	copyBufferToImage(stagingBuffer,
		textureImage,
		static_cast<uint32_t>(texWidth),
		static_cast<uint32_t>(texHeight));
	//转换图像布局，用于着色器访问
	transitionImageLayout(textureImage,
		VK_FORMAT_R8G8B8A8_SRGB, 
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);

}

void Triangle::createTextureImageView()
{
	textureImageView=createImageView(textureImage,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_ASPECT_COLOR_BIT);//创建纹理视图

}

/**
 * @descrip 创建纹理采样器，结果保存在成员对象的textureSampler中
 * 
 * @functionName:  createTextureSampler
 * @functionType:    void
 */
void Triangle::createTextureSampler()
{
	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(physicalDevice, &properties); //检索物理设备的属性

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;//放大采用线性过滤
	samplerInfo.minFilter = VK_FILTER_LINEAR;//缩小采用线性过滤
	//纹理坐标超出了[0, 1] 范围，就重复纹理（平铺）
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = properties.limits.maxBoundDescriptorSets;//设备最高性能
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;//超出边界填充的颜色，当且仅当超出纹理模式设为才生效
	samplerInfo.unnormalizedCoordinates = VK_FALSE;//设置[0,1]定义纹理坐标，即归一化
	samplerInfo.compareEnable = VK_FALSE;//禁用比较采样
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	//mipmapping
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	//samplerInfo.mipLodBias = 0.0f;
	//samplerInfo.minLod = 0.0f;
	//samplerInfo.maxLod = 0.0f;
	
	//创建采样器
	if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture sampler!");
	}
}

/**
 * @descrip 记录命令缓冲区
 *
 * @functionName:  recordCommandBuffer
 * @functionType:    void
 * @param commandBuffer
 * @param imageIndex
 */
void Triangle::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	//开始录制命令
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}
	//2.设置渲染通道开始信息
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swapChainExtent;

	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);//启动渲染通道

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);//绑定图形管线

		//动态设置视口和裁剪矩形
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapChainExtent.width);
		viewport.height = static_cast<float>(swapChainExtent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = swapChainExtent;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		VkBuffer vertexBuffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);//绑定顶点缓冲

		vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);//绑定索引缓冲
		//使用描述符集，每帧的描述符集绑定到对应着色器
		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,//绑定到图形管线
			pipelineLayout, 
			0,//set编号：0
			1,//一次绑定一个set
			&descriptorSets[currentFrame],//绑定的descriptor set
			0,
			nullptr);

		//vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);//绑定索引缓冲

		//vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);
		//vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(rectangle::indices.size()), 1, 0, 0, 0);

	vkCmdEndRenderPass(commandBuffer);//结束渲染通道

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}
}

/**
 * @descrip 更新全局数据
 * 
 * @functionName:  updateUniformBuffer
 * @functionType:    void
 * @param currentImage
 */
void Triangle::updateUniformBuffer(uint32_t currentImage)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	//以浮点精度计算自渲染开始以来的秒数
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	
	UniformBufferObject ubo{};
	ubo.model = glm::rotate(glm::mat4(1.0f),//单位矩阵
		time * glm::radians(90.0f),//每秒旋转90°
		glm::vec3(0.0f, 0.0f, 1.0f));

	//从上方以 45 度角观察几何形状
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
		glm::vec3(0.0f, 0.0f, 0.0f), 
		glm::vec3(0.0f, 0.0f, 1.0f));

	// 45 度垂直视场角的透视投影
	ubo.proj = glm::perspective(glm::radians(25.0f),
		swapChainExtent.width / (float)swapChainExtent.height,
		0.1f, 
		10.0f);

	//翻转 Y 轴缩放因子，与OPENGL相反
	ubo.proj[1][1] *= -1;
	//复制数据到统一缓冲区，uniformBuffersMapped指针指向该内存
	memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}


/**
 * @descrip 绘制一帧
 *
 * @functionName:  drawFrame
 * @functionType:    void
 */
void Triangle::drawFrame()
{
	//1.等待前一帧，等待fence触发
	vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);//cpu等待gpu画完
	//vkResetFences(device, 1, &inFlightFences[currentFrame]);//重置信号量
	//2.获取交换区图像
	uint32_t imageIndex;//保存交换区图像索引

	//获取成功，发送imageAvailableSemaphore信号量
	//vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
	VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	//如果获取图像失败，重新创建交换链
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	updateUniformBuffer(currentFrame);//更新全局数据

	// Only reset the fence if we are submitting work,延迟重置,仅在确认提交任务前重置栅栏，防止重新创建交换链的时候直接返回，导致fence永远不能重置
	vkResetFences(device, 1, &inFlightFences[currentFrame]);//重置栅栏
	//3.提交到命令缓冲区
	vkResetCommandBuffer(commandBuffers[currentFrame], 0);//重置命令缓冲区
	recordCommandBuffer(commandBuffers[currentFrame], imageIndex);//录制新的命令缓冲区
	//4.提交到命令缓冲区到队列 
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };//执行前等待的获取的图像信号
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };//图像可用信号量触发，且到达颜色附件输出阶段时，命令才真正开始执行。
	submitInfo.waitSemaphoreCount = 1;

	//信号量与pWaitDstStageMask中相同的索引关系相对应
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;//指明信号阻塞在哪里
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

	// 执行提交的命令缓冲区中的所有操作后发送renderFinishedSemaphore信号
	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	//5.提交到图形队列，非阻塞操作
	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	//6.展示到屏幕
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;//等待提交缓冲区信号,表示渲染任务已完成

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	//vkQueuePresentKHR(presentQueue, &presentInfo);
	result = vkQueuePresentKHR(presentQueue, &presentInfo);

	//显示检查是否需要重新创建交换链
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
		framebufferResized = false;
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

/**
 * @descrip 创建同步对象
 *
 * @functionName:  createSyncObjects
 * @functionType:    void
 */
void Triangle::createSyncObjects()
{
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;//创建同步信号的时候，设置初始状态为已设置,防止死锁

	//创建同步信号
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}

/**
 * @descrip  再创建新的交换链之前需要清理旧的交换链的内容
 *
 * @functionName:  cleanupSwapChain
 * @functionType:    void
 */
void Triangle::cleanupSwapChain()
{
	for (size_t i = 0; i < swapChainFramebuffers.size(); i++) {
		vkDestroyFramebuffer(device, swapChainFramebuffers[i], nullptr);
	}

	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		vkDestroyImageView(device, swapChainImageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(device, swapChain, nullptr);
}

/**
 * @descrip 新建交换链
 *
 * @functionName:  recreateSwapChain
 * @functionType:    void
 */
void Triangle::recreateSwapChain()
{
	int width = 0, height = 0;
	glfwGetFramebufferSize(window, &width, &height);// 获取窗口的帧缓冲区
	while (width == 0 || height == 0) {//如果宽度或高度为0（窗口最小化或尺寸异常），进入等待状态。
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();//阻塞线程，等待窗口尺寸变化
	}

	vkDeviceWaitIdle(device);//等待设备处理完空闲之后

	cleanupSwapChain();

	createSwapChain();
	createImageViews();
	createFramebuffers();
}

/**
 * @descrip 创建顶点缓冲区
 *
 * @functionName:  createVertexBuffer
 * @functionType:    void
 */
void Triangle::createVertexBuffer()
{
	//VkDeviceSize bufferSize = sizeof(rectangle::vertices[0]) * (rectangle::vertices.size());
	VkDeviceSize bufferSize = sizeof(rectangle::vertices[0]) * (rectangle::vertices.size());
	
	VkBuffer stagingBuffer;//缓冲区
	VkDeviceMemory stagingBufferMemory;//缓冲区内存

	//创建CPU内存
	createBuffer(bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,// 缓冲区用作内存传的数据源
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer,
		stagingBufferMemory);

	void* data;//data是能直接内存拷贝的指针
	vkMapMemory(device, stagingBufferMemory, 0, (size_t)bufferSize, 0, &data);//GPU映射到CPU
	//memcpy(data, rectangle::vertices.data(), (size_t)bufferSize);//通过data拷贝数据到data指向的stagingBufferMemory
	memcpy(data, rectangle::vertices.data(), (size_t)bufferSize);//通过data拷贝数据到data指向的stagingBufferMemory
	vkUnmapMemory(device, stagingBufferMemory);//取消映射

	//创建GPU内存
	createBuffer(bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,// 缓冲区用作内存传的输出
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		vertexBuffer,
		vertexBufferMemory);

	//录制拷贝命令，将CPU准备好的内存数据移动到GPU显存
	copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}

/**
 * @descrip 创建索引缓冲区
 * 
 * @functionName:  createIndexBuffer
 * @functionType:    void
 */
void Triangle::createIndexBuffer()
{
	//auto bufferSize=sizeof(rectangle::indices[0])* (rectangle::indices.size());
	auto bufferSize=sizeof(rectangle::indices[0])* (rectangle::indices.size());

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	createBuffer(bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer,
		stagingBufferMemory);

	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	//memcpy(data, rectangle::indices.data(), (size_t)bufferSize);
	memcpy(data, rectangle::indices.data(), (size_t)bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);

	createBuffer(bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		indexBuffer,
		indexBufferMemory);

	copyBuffer(stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}

/**
 * @descrip 创建统一资源缓冲区
 * 
 * @functionName:  createUniformBuffers
 * @functionType:    void
 */
void Triangle::createUniformBuffers()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
	uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);


	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		createBuffer(bufferSize,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			uniformBuffers[i], //逻辑内存
			uniformBuffersMemory[i]);//物理内存
		//持久映射，uniformBuffersMapped指向缓冲区数据
		vkMapMemory(device, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
	}
}

/**
 * @descrip 创建描述集池
 * 
 * @functionName:  createDescriptorPool
 * @functionType:    void
 */
void Triangle::createDescriptorPool()
{
	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;//枚举值：6
	poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);//总共的槽位
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;//枚举值：1
	poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);//总共的槽位

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;//枚举值:33
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);//最大描述符集数量

	//创建描述符集池
	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

/**
 * @descrip 创建描述符集
 * 
 * @functionName:  createDescriptorSets
 * @functionType:    void
 */
void Triangle::createDescriptorSets()
{
	//一次性创建多个set,绑定到同个描述符布局
	std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;//通过池分配set
	allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);//指定layouts长度
	allocInfo.pSetLayouts = layouts.data();//为每个分配的描述符集指明描述符布局

	descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	//分配描述符集，设置句柄保留sets副本
	if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	//填充描述符，把哪个 VkBuffer 绑定到哪个 set 的哪个 binding 插槽
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffers[i];//指定缓冲区
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkDescriptorImageInfo imageInfo{};
		imageInfo.sampler = textureSampler;
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = textureImageView;

		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = descriptorSets[i];
		descriptorWrites[0].dstBinding = 0;//指定统一描述符索引是0
		descriptorWrites[0].dstArrayElement = 0;//没使用数组，从索引为0开始
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;//再次指定描述符类型
		descriptorWrites[0].descriptorCount = 1;//更新描述符数组元素数量
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = descriptorSets[i];
		descriptorWrites[1].dstBinding = 1;//指定统一描述符索引是0
		descriptorWrites[1].dstArrayElement = 0;//没使用数组，从索引为0开始
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;//再次指定描述符类型
		descriptorWrites[1].descriptorCount = 1;//更新描述符数组元素数量
		descriptorWrites[1].pImageInfo = &imageInfo;

	
		//vulkan进行绑定实际资源到set的binding槽上
		vkUpdateDescriptorSets(device, 
			static_cast<uint32_t>(descriptorSets.size()),
			descriptorWrites.data(), 
			0,
			nullptr);
	}


}

/**
 * @descrip
 *
 * @functionName:  findMemoryType
 * @functionType:    uint32_t
 * @param typeFilter 位掩码uint32_t类型，表示内存类型
 * @param requiredProperties 位掩码uint32_t类型，表示内存类型
 * @return    返回位掩码，即合适的内存类型
 */
uint32_t Triangle::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) &&
			(memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	throw std::runtime_error("failed to find suitable memory type!");
}

/**
 * @descrip 创建缓冲区用于传递信息
 * 
 * @functionName:  createBuffer
 * @functionType:    void
 * @param size 缓冲区大小
 * @param usage 缓冲区用途
 * @param properties 内存属性
 * @param buffer 缓冲区写入句柄
 * @param bufferMemory 内存句柄
 */
void Triangle::createBuffer(VkDeviceSize size,
	VkBufferUsageFlags usage, 
	VkMemoryPropertyFlags properties,
	VkBuffer& buffer, 
	VkDeviceMemory& bufferMemory)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;//独占

	//1.创建缓冲区
	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}
	//2.查询vertexBuffer内存需求
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	//查询物理设备能支持的内存类型
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	//3.分配内存
	if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	//4.绑定缓冲区和内存
	vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

/**
 * @descrip 创建图像缓冲区
 * 
 * @functionName:  createImage
 * @functionType:    void
 * @param width 图像宽
 * @param height 图像高
 * @param format 图像格式
 * @param tiling 读取方式
 * @param usage
 * @param properties
 * @param image
 * @param imageMemory
 */
void Triangle::createImage(uint32_t width, uint32_t height,
	VkFormat format, 
	VkImageTiling tiling,
	VkImageUsageFlags usage, 
	VkMemoryPropertyFlags properties,
	VkImage& image, 
	VkDeviceMemory& imageMemory)
{
	//创建图像对象，用于存储纹素
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;//设置与读取纹素相同的格式
	imageInfo.tiling = tiling;//纹素以实现定义的顺序排列，GPU高效访问
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;//图像在上传时，图像的内容没有意义
	imageInfo.usage = usage;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;//单样本采样
	//imageInfo.flags = 0; // Optional

	if (vkCreateImage(device, &imageInfo, nullptr, &textureImage) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, image, &memRequirements);//获取图像对象的内存需求

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(device, image, imageMemory, 0);
}

/**
 * @descrip 从CPU内存拷贝到GPU显存
 * 
 * @functionName:  copyBuffer
 * @functionType:    void
 * @param srcBuffer 源缓存区
 * @param size 缓存区大小
 * @param dstBuffer 目标缓存区
 */
void Triangle::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBuffer commandBuffer=beginSingleTimeCommands();
		
		VkBufferCopy copyRegion{};
		//copyRegion.srcOffset = 0; // Optional
		//copyRegion.dstOffset = 0; // Optional
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	endSingleTimeCommands(commandBuffer);

}

/**
 * @descrip 从CPU内存拷贝到GPU显存(图像)
 * 
 * @functionName:  copyBufferToImage
 * @functionType:    void
 * @param buffer 缓冲区
 * @param image  vulkan图像对象
 * @param width  宽
 * @param height 长
 */
void Triangle::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();
	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageExtent = {
		width,
		height,
		1
	};
	region.imageOffset = { 0,0,0 };
	region.imageSubresource.aspectMask= VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	vkCmdCopyBufferToImage(
		commandBuffer,
		buffer,
		image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,//表示图像当前正在使用哪种布局
		1,
		&region
	);//缓冲区到图像的复制

	endSingleTimeCommands(commandBuffer);
}

/**
 * @descrip 查找支持的格式，从最期望到最不期望的数组中找到期望的格式
 * 遍历 candidates 列表，查找 在指定 tiling 模式下，支持要求功能的第一个格式
 * 
 * @functionName:  findSupportedFormat
 * @functionType:    VkFormat
 * @param candidates 格式列表
 * @param tiling 图像的布局模式
 * @param features 格式支持的功能
 * @return    
 */
VkFormat Triangle::findSupportedFormat(const std::vector<VkFormat>& candidates,
	VkImageTiling tiling, 
	VkFormatFeatureFlags features)
{
	for (auto format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

		if(tiling==VK_IMAGE_TILING_LINEAR&&
			(props.linearTilingFeatures & features)==
			features) {
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
			(props.optimalTilingFeatures & features) ==
			features) {
			return format;
		}
	}
	throw std::runtime_error("failed to find supported format!");
}

/**
 * @descrip 选择深度组件格式
 * 
 * @functionName:  findDepthFormat
 * @functionType:    VkFormat
 * @return    
 */
VkFormat Triangle::findDepthFormat()
{
	return findSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },//格式列表
		VK_IMAGE_TILING_OPTIMAL,//最适合 GPU 访问的图像布局模式
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT//用作深度/模板附件
	);
}

/**
 * @descrip 检查深度格式包含模板组件
 * 
 * @functionName:  hasStencilComponent
 * @functionType:    bool
 * @return    
 */
bool Triangle::hasStencilComponent(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

/**
 * @descrip 创建深度资源
 * 
 * @functionName:  createDepthResources
 * @functionType:    void
 */
void Triangle::createDepthResources()
{
	//查找格式
	VkFormat depthFormat = findDepthFormat();
	//构建深度图像缓存
	createImage(swapChainExtent.width,
		swapChainExtent.height,
		depthFormat,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		depthImage,
		depthImageMemory);
	depthImageView=createImageView(depthImage,
		depthFormat, 
		VK_IMAGE_ASPECT_DEPTH_BIT);//创建深度视图

	//转换布局
	transitionImageLayout(depthImage, 
		depthFormat, 
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}


	




/**
 * @descrip    创建调试消息的接收器，由于扩展函数不能直接调用，需要获取地址
 *
 * @functionName:  CreateDebugUtilsMessengerEXT
 * @functionType:    VkResult
 * @param instance
 * @param pCreateInfo
 * @param pAllocator
 * @param pDebugMessenger
 * @return
 */
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pDebugMessenger)
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
void DestroyDebugUtilsMessengerEXT(VkInstance instance,
	VkDebugUtilsMessengerEXT debugMessenger,
	const VkAllocationCallbacks* pAllocator)
{
	//成功，直接调用销毁函数
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}