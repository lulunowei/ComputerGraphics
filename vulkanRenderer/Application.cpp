#include "Application.h"

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
	mainloop();
	cleanup();
}

void Application::initVulkan()
{

}

void Application::mainloop()
{
	while (!glfwWindowShouldClose(m_Window->getWindow())) {
		glfwPollEvents();
	}
}

void Application::cleanup()
{
	glfwTerminate();
}
