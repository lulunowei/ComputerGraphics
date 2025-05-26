#pragma once
//#include"init/VulkanInstance.h"
//#include"init/Window.h"
//#include"init/VulkanDevice.h"
#include"init/ApplicationContext.h"
#include"render/Renderer.h"
#include"Uncopyable.h"


class Application:
	private Uncopyable
{
public:

	Application();
	 ~Application();

	void run();


private:	
	void initVulkan();
	void mainloop();
	void cleanup();

	std::unique_ptr<ApplicationContext> m_context;

	std::unique_ptr<Renderer> m_renderer;

	
};

