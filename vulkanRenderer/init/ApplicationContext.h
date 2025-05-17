#pragma once
#include"Window.h"
#include"VulkanInstance.h"
#include"VulkanDevice.h"

struct ApplicationContext {
	Window* windowContext=nullptr;
	VulkanInstance *instanceContext=nullptr;
	VulkanDevice *deviceContext=nullptr;
};

