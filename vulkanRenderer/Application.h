#pragma once
#include"init/VulkanInstance.h"
#include"init/Window.h"
#include"init/VulkanDevice.h"
#include"render/Renderer.h"


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

	std::unique_ptr<Renderer> m_renderer;

	
};

