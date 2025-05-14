#pragma once
#include"Window.h"


#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif // 调试模式就开启验证

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

class Application
{
public:
	//静态的回调函数
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;//返回true调用程序会终止
	}

	Application();
	virtual ~Application();

	void run();


private:	
	void initVulkan();
	void mainloop();
	void cleanup();


	std::unique_ptr<Window> m_Window;
};

