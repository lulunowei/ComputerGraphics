#pragma once
#include"../const.h"
#include"../Application.h"
class Extension
{
public:
	static std::vector<const char*> getRequiredExtensions();
	static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);//封装调试消息的创建信息

};

