#include "Application.h"
#include"utils/VkExtensionUtils.h"
#include"init/AllHeads.h"
Application::Application()
{
	m_context = std::make_unique<ApplicationContext>();
	//初始化window
	m_context->windowContext =
		std::make_shared<Window>(800, 600, "Model");
	auto window = m_context->windowContext->getWindow();

	//初始化vulkan实例
	m_context->instanceContext =
		std::make_shared<VulkanInstance>(window);
	auto instance = m_context->instanceContext->getInstance();
	auto surface = m_context->instanceContext->getSurface();

	//初始化设备
	m_context->deviceContext =
		std::make_shared<VulkanDevice>(instance, surface);

	//初始化Render类
	m_renderer = std::make_unique<Renderer>(*m_context);
}

Application::~Application()
{
	
}

void Application::run()
{
	//initVulkan();
	mainloop();
	cleanup();
}

void Application::initVulkan()
{

	

}

void Application::mainloop()
{
	auto window=m_context->windowContext->getWindow();
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		m_renderer->draw();//绘制一帧
	}
	vkDeviceWaitIdle(m_context->deviceContext->getLogicalDevice());//等待特定命令队列中的操作完成

}

void Application::cleanup()
{
	m_renderer->rendererCleanup();
	m_context->deviceContext->deviceCleanup();
	m_context->instanceContext->intanceCleanup();
	m_context->windowContext->windowCleanup();
	glfwTerminate();
}
