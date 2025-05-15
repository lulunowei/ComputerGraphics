#include "Application.h"
#include"utils/VkExtensionUtils.h"

Application::Application()
{
	m_Window = 
		std::make_unique<Window>(800, 600, "Model");
}

Application::~Application()
{
	
}

void Application::run()
{
	initVulkan();
	mainloop();
	cleanup();
}

void Application::initVulkan()
{
	m_instance->createInstance();//创建vulkan实例
	m_instance->setupDebugMessenger();//debug信息
	m_instance->createSurface(m_Window->getWindow());//创建一个窗口扩展
	m_device->pickPhysicalDevice(
		m_instance->getInstance(),
		m_instance->getSurface()
	);//选择物理设备
	m_device->createLogicalDevice(m_instance->getSurface());//创建逻辑设备
}

void Application::mainloop()
{
	while (!glfwWindowShouldClose(m_Window->getWindow())) {
		glfwPollEvents();
	}
}

void Application::cleanup()
{
	m_device->deviceCleanup();
	m_instance->intanceCleanup();
	m_Window->windowCleanup();
	glfwTerminate();
}
