#pragma once
#include<vulkan/vulkan.h>
#include"const.h"
#include"utils/Extension.h"

class VulkanInstance
{
public:
	VulkanInstance();
	virtual ~VulkanInstance();
	void createInstance();
private:
	VkInstance  m_instance;
};

