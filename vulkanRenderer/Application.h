#pragma once
#include "VulkanInstance.h"
#include"Window.h"
#include"VulkanDevice.h"


class Application
{
public:

	Application();
	virtual ~Application();

	void run();


private:	
	void initVulkan();
	void mainloop();
	void cleanup();


	std::unique_ptr<Window> m_Window;
	std::unique_ptr<VulkanInstance> m_instance;
	std::unique_ptr<VulkanDevice> m_device;
	//VulkanInstance m_instance;
};

